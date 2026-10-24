// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#include "sgl_core.h"
#include "draw/sgl_common.h"
#include "sgl_list.h"

#define SGL_SLICE_STATE_IDLE    0
#define SGL_SLICE_STATE_START   1
#define SGL_SLICE_STATE_RUNNING 2

static inline void sgl_area_get_intersect(sgl_area_t *area, int32_t left,
                                          int32_t top, int32_t right,
                                          int32_t bottom) {
    if (area->left < left)
        area->left = left;
    if (area->top < top)
        area->top = top;
    if (area->right > right)
        area->right = right;
    if (area->bottom > bottom)
        area->bottom = bottom;
}

static inline void sgl_area_get_union(sgl_area_t *area, int32_t left,
                                      int32_t top, int32_t right,
                                      int32_t bottom) {
    if (area->left > left)
        area->left = left;
    if (area->top > top)
        area->top = top;
    if (area->right < right)
        area->right = right;
    if (area->bottom < bottom)
        area->bottom = bottom;
}

static inline int sgl_area_set_intersect(sgl_area_t *area, int32_t left,
                                         int32_t top, int32_t right,
                                         int32_t bottom) {
    if (!sgl_area_intersects(area, left, top, right, bottom))
        return 0;
    sgl_area_get_intersect(area, left, top, right, bottom);
    return 1;
}

static inline int sgl_area_set_union(sgl_area_t *area, int32_t left,
                                     int32_t top, int32_t right,
                                     int32_t bottom) {
    if (!sgl_area_intersects(area, left, top, right, bottom))
        return 0;
    sgl_area_get_union(area, left, top, right, bottom);
    return 1;
}

static void sgl_merge_invalidate_rect(sgl_dirty_t *dirty, int32_t left,
                                      int32_t top, int32_t right,
                                      int32_t bottom) {
    int i = 0;
    while (i < dirty->dirty_count) {
        sgl_area_t *area = &dirty->dirty_buffer[i];
        if (!sgl_area_set_union(area, left, top, right, bottom)) {
            ++i;
            continue;
        }
        left = area->left;
        top = area->top;
        right = area->right;
        bottom = area->bottom;
        dirty->dirty_count -= 1;
        *area = dirty->dirty_buffer[dirty->dirty_count];
        i = 0;
    }
    if (dirty->dirty_count == dirty->dirty_num) {
        dirty->flag_full_screen = 1;
        return;
    }
    sgl_set_area(&dirty->dirty_buffer[dirty->dirty_count], left, top, right,
                 bottom);
    dirty->dirty_count += 1;
}

void sgl_invalidate_rect(sgl_display_t *disp, int32_t x, int32_t y, int32_t w,
                         int32_t h) {
    sgl_dirty_t *dirty = &disp->dirty;
    if (!dirty->dirty_buffer || dirty->flag_full_screen)
        return;
    sgl_area_t bounds;
    sgl_rect2area(&disp->root.rect, &bounds);
    sgl_normalize_rect(&x, &y, &w, &h);
    if (sgl_clip_rect(&bounds, &x, &y, &w, &h))
        return;
    sgl_merge_invalidate_rect(dirty, x, y, x + w - 1, y + h - 1);
}

static void sgl_reset_dirty(sgl_dirty_t *dirty) {
    dirty->dirty_count = 0;
    dirty->flag_full_screen = 0;
}

static void sgl_buffer_slice(sgl_display_t *disp, sgl_slice_t *slice,
                             sgl_rect_t dirty_rect) {
    if (slice->slice_state == SGL_SLICE_STATE_IDLE) {
        slice->slice_state = SGL_SLICE_STATE_START;
    }
    if (slice->slice_state == SGL_SLICE_STATE_START) {
        if (dirty_rect.w == 0 || dirty_rect.h == 0)
            return;
        if (disp->frame_start)
            disp->frame_start(disp->user_data);
        slice->frame_rect = dirty_rect;
        slice->slice_count = 0;
        slice->slice_state = SGL_SLICE_STATE_RUNNING;
    }
    if (slice->slice_state == SGL_SLICE_STATE_RUNNING) {
        int32_t w_piece, h_piece;
        disp->buffer_ox = slice->frame_rect.x;
        disp->buffer_oy = slice->frame_rect.y + slice->slice_count;
        w_piece = slice->frame_rect.w;
        h_piece = (disp->fb.pixel_num / slice->frame_rect.w)
                  << disp->fb.pixel_index;
        if (h_piece > slice->frame_rect.h - slice->slice_count)
            h_piece = slice->frame_rect.h - slice->slice_count;
        disp->fb.dirty_width = w_piece;
        slice->slice_count += h_piece;
        sgl_set_area(&slice->slice_area, disp->buffer_ox, disp->buffer_oy,
                     disp->buffer_ox + w_piece - 1,
                     disp->buffer_oy + h_piece - 1);
        if (slice->slice_count == slice->frame_rect.h) {
            slice->slice_state = SGL_SLICE_STATE_IDLE;
        }
    }
}

static void sgl_draw_proc(sgl_display_t *disp, sgl_widget_t *widget, int32_t ox,
                          int32_t oy, const sgl_area_t *parent_bounds) {
    sgl_area_t bounds = *parent_bounds;
    if (!sgl_area_set_intersect(&bounds, ox, oy, ox + widget->rect.w - 1,
                                oy + widget->rect.h - 1))
        return;
    disp->logical_ox = ox;
    disp->logical_oy = oy;
    disp->widget_bounds = bounds;
    disp->clip_area = bounds;
    if (widget->cls->draw)
        widget->cls->draw(disp, widget);
    sgl_widget_t *child;
    sgl_list_for_each_entry(child, &widget->children, sibling) {
        sgl_draw_proc(disp, child, ox + child->rect.x, oy + child->rect.y,
                      &bounds);
    }
}

int sgl_draw(sgl_display_t *disp, sgl_area_t *flush_area) {
    sgl_widget_t *invalidate_widget = &disp->root;
    sgl_rect_t dirty_rect = invalidate_widget->rect;
    int32_t ox, oy;
    sgl_buffer_slice(disp, &disp->slice, dirty_rect);
    *flush_area = disp->slice.slice_area;
    sgl_widget_get_origin_coords(invalidate_widget, &ox, &oy);
    sgl_draw_proc(disp, invalidate_widget, ox, oy, &disp->slice.slice_area);
    if (disp->slice.slice_state == SGL_SLICE_STATE_IDLE) {
        ++disp->frame_count;
        if (disp->frame_end)
            disp->frame_end(disp->user_data);
    }
    return 1;
}

int sgl_set_clip_area(sgl_display_t *disp, int32_t left, int32_t top,
                      int32_t right, int32_t bottom) {
    disp->clip_area = disp->widget_bounds;
    return sgl_area_set_intersect(&disp->clip_area, left, top, right, bottom);
}

void sgl_reset_clip_area(sgl_display_t *disp) {
    disp->clip_area = disp->widget_bounds;
}
