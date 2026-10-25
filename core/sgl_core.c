// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#include "sgl_core.h"
#include "draw/sgl_common.h"
#include "sgl_list.h"

#define SGL_STATE_SLICE_PREPARE    0
#define SGL_STATE_SLICE_RUNNING    1

#define SGL_STATE_DIRTY_SWITCH     0
#define SGL_STATE_DIRTY_IN_FRAME   1
#define SGL_DIRTY_STATE_SHIFT      0
#define SGL_DIRTY_STATE_MASK       0x03U

#define SGL_FLAG_DIRTY_FULL        0
#define SGL_FLAG_DIRTY_IN_FRAME    1
#define SGL_FLAG_DIRTY_FULL_SCREEN 2
#define SGL_FLAG_DIRTY_IDLE_SEND   3
#define SGL_DIRTY_FLAG_SHIFT       2
#define SGL_DIRTY_FLAG_MASK        0x3CU

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

static inline void sgl_invalidate_root(sgl_display_t *disp) {
    sgl_rect_t *root_rect = &disp->root.rect;
    sgl_set_area(&disp->dirty.dirty_buffer[0], root_rect->x, root_rect->y,
                 root_rect->x + root_rect->w - 1,
                 root_rect->y + root_rect->h - 1);
}

static void sgl_merge_invalidate_area(sgl_display_t *disp, int32_t left,
                                      int32_t top, int32_t right,
                                      int32_t bottom) {
    int i = 0;
    sgl_dirty_t *dirty = &disp->dirty;
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
    // buffer full
    if (dirty->dirty_count == dirty->dirty_num) {
        sgl_flag_set(dirty->flag, SGL_DIRTY_FLAG_SHIFT, SGL_FLAG_DIRTY_FULL);
        sgl_invalidate_root(disp);
        dirty->dirty_count = 1;
        return;
    }
    // add new dirty
    sgl_set_area(&dirty->dirty_buffer[dirty->dirty_count], left, top, right,
                 bottom);
    dirty->dirty_count += 1;
}

void sgl_invalidate_area(sgl_display_t *disp, int32_t left, int32_t top,
                         int32_t right, int32_t bottom) {
    sgl_dirty_t *dirty = &disp->dirty;
    if (sgl_flag_get(dirty->flag, SGL_DIRTY_FLAG_SHIFT, SGL_FLAG_DIRTY_FULL))
        return;
    sgl_area_t area;
    sgl_rect2area(&disp->root.rect, &area);
    if (!sgl_area_set_intersect(&area, left, top, right, bottom))
        return;
    if (sgl_flag_get(dirty->flag, SGL_DIRTY_FLAG_SHIFT,
                     SGL_FLAG_DIRTY_FULL_SCREEN)) {
        dirty->dirty_count = 1;
        return;
    }
    sgl_merge_invalidate_area(disp, area.left, area.top, area.right,
                              area.bottom);
}

static int sgl_buffer_slice(sgl_display_t *disp, sgl_area_t *dirty_area) {
    sgl_slice_t *slice = &disp->slice;
    if (slice->slice_state == SGL_STATE_SLICE_PREPARE) {
        sgl_area2rect(dirty_area, &slice->frame_rect);
        slice->slice_count = 0;
        slice->slice_state = SGL_STATE_SLICE_RUNNING;
    }
    if (slice->slice_state == SGL_STATE_SLICE_RUNNING) {
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
            slice->slice_state = SGL_STATE_SLICE_PREPARE;
            return 1;
        }
    }
    return 0;
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

static int sgl_draw_dirty(sgl_display_t *disp, sgl_widget_t *invalidate_widget,
                          sgl_area_t *dirty_area, sgl_area_t *flush_area) {
    int ret = sgl_buffer_slice(disp, dirty_area);
    *flush_area = disp->slice.slice_area;
    int32_t ox, oy;
    sgl_widget_get_origin_coords(invalidate_widget, &ox, &oy);
    sgl_draw_proc(disp, invalidate_widget, ox, oy, &disp->slice.slice_area);
    return ret;
}

int sgl_draw(sgl_display_t *disp, sgl_area_t *flush_area) {
    sgl_dirty_t *dirty = &disp->dirty;
    if (sgl_state_get(dirty->flag, SGL_DIRTY_STATE_SHIFT,
                      SGL_DIRTY_STATE_MASK) == SGL_STATE_DIRTY_SWITCH) {
        if (dirty->dirty_count == 0 &&
            sgl_flag_get(dirty->flag, SGL_DIRTY_FLAG_SHIFT,
                         SGL_FLAG_DIRTY_IDLE_SEND)) {
            sgl_invalidate_root(disp);
            dirty->dirty_count = 1;
        }
        if (dirty->dirty_count) {
            if (sgl_flag_get(dirty->flag, SGL_DIRTY_FLAG_SHIFT,
                             SGL_FLAG_DIRTY_FULL_SCREEN)) {
                sgl_invalidate_root(disp);
            }
            // switch buffer
            sgl_area_t *temp = dirty->dirty_buffer;
            dirty->dirty_buffer = dirty->dirty_pending;
            dirty->dirty_pending = temp;
            dirty->pending_count = dirty->dirty_count;
            // reset dirty
            dirty->dirty_count = 0;
            sgl_flag_reset(dirty->flag, SGL_DIRTY_FLAG_SHIFT,
                           SGL_FLAG_DIRTY_FULL);
            // switch state
            sgl_state_set(dirty->flag, SGL_DIRTY_STATE_SHIFT,
                          SGL_DIRTY_STATE_MASK, SGL_STATE_DIRTY_IN_FRAME);
            // a frame start
            if (disp->frame_start)
                disp->frame_start(disp->user_data);
        }
    }
    if (sgl_state_get(dirty->flag, SGL_DIRTY_STATE_SHIFT,
                      SGL_DIRTY_STATE_MASK) == SGL_STATE_DIRTY_IN_FRAME) {
        if (sgl_draw_dirty(disp, &disp->root,
                           &disp->dirty.dirty_pending[dirty->pending_count - 1],
                           flush_area)) {
            dirty->pending_count -= 1;
        }
        // a frame end
        if (dirty->pending_count == 0) {
            disp->frame_count += 1;
            if (disp->frame_end)
                disp->frame_end(disp->user_data);
            // switch state
            sgl_state_set(dirty->flag, SGL_DIRTY_STATE_SHIFT,
                          SGL_DIRTY_STATE_MASK, SGL_STATE_DIRTY_SWITCH);
        }
        return 1;
    }
    return 0;
}

int sgl_set_clip_area(sgl_display_t *disp, int32_t left, int32_t top,
                      int32_t right, int32_t bottom) {
    disp->clip_area = disp->widget_bounds;
    return sgl_area_set_intersect(&disp->clip_area, left, top, right, bottom);
}

void sgl_reset_clip_area(sgl_display_t *disp) {
    disp->clip_area = disp->widget_bounds;
}

void sgl_set_full_screen(sgl_display_t *disp, uint8_t value) {
    if (value)
        sgl_flag_set(disp->dirty.flag, SGL_DIRTY_FLAG_SHIFT,
                     SGL_FLAG_DIRTY_FULL_SCREEN);
    else
        sgl_flag_reset(disp->dirty.flag, SGL_DIRTY_FLAG_SHIFT,
                       SGL_FLAG_DIRTY_FULL_SCREEN);
}

void sgl_set_idle_send(sgl_display_t *disp, uint8_t value) {
    if (value)
        sgl_flag_set(disp->dirty.flag, SGL_DIRTY_FLAG_SHIFT,
                     SGL_FLAG_DIRTY_IDLE_SEND);
    else
        sgl_flag_reset(disp->dirty.flag, SGL_DIRTY_FLAG_SHIFT,
                       SGL_FLAG_DIRTY_IDLE_SEND);
}
