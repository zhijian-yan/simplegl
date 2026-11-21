// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#include "sgl_core.h"
#include "draw/sgl_common.h"
#include "sgl_list.h"
#include "sgl_port.h"

#define SGL_STATE_SLICE_PREPARE    0
#define SGL_STATE_SLICE_RUNNING    1

#define SGL_STATE_DIRTY_IDLE       0
#define SGL_STATE_DIRTY_FLUSHING   1

#define SGL_FLAG_DIRTY_FULL        0
#define SGL_FLAG_DIRTY_FULL_SCREEN 1
#define SGL_DIRTY_FLAG_SHIFT       0
#define SGL_DIRTY_FLAG_MASK        0x03U

static inline void sgl_area_get_intersect(sgl_area_t *area, sgl_int_t left,
                                          sgl_int_t top, sgl_int_t right,
                                          sgl_int_t bottom) {
    if (area->left < left)
        area->left = left;
    if (area->top < top)
        area->top = top;
    if (area->right > right)
        area->right = right;
    if (area->bottom > bottom)
        area->bottom = bottom;
}

static inline void sgl_area_get_union(sgl_area_t *area, sgl_int_t left,
                                      sgl_int_t top, sgl_int_t right,
                                      sgl_int_t bottom) {
    if (area->left > left)
        area->left = left;
    if (area->top > top)
        area->top = top;
    if (area->right < right)
        area->right = right;
    if (area->bottom < bottom)
        area->bottom = bottom;
}

static inline int sgl_area_set_intersect(sgl_area_t *area, sgl_int_t left,
                                         sgl_int_t top, sgl_int_t right,
                                         sgl_int_t bottom) {
    if (!sgl_area_intersects(area, left, top, right, bottom))
        return 0;
    sgl_area_get_intersect(area, left, top, right, bottom);
    return 1;
}

static inline int sgl_area_set_union(sgl_area_t *area, sgl_int_t left,
                                     sgl_int_t top, sgl_int_t right,
                                     sgl_int_t bottom) {
    if (!sgl_area_intersects(area, left, top, right, bottom))
        return 0;
    sgl_area_get_union(area, left, top, right, bottom);
    return 1;
}

static inline void sgl_invalidate_root(sgl_display_t *disp) {
    sgl_rect_t *root_rect = &disp->root.rect;
    sgl_set_area(&disp->dirty.dirty_areas[0], root_rect->x, root_rect->y,
                 root_rect->x + root_rect->w - 1,
                 root_rect->y + root_rect->h - 1);
    disp->dirty.dirty_count = 1;
}

static void sgl_merge_dirty(sgl_dirty_t *dirty) {
    sgl_area_t *areas = dirty->dirty_pending_areas;
    uint8_t count = dirty->pending_count;
    uint8_t merged = 0;
    for (uint8_t i = 0; i < count; ++i) {
        sgl_area_t area = areas[i];
        uint8_t j = 0;
        while (j < merged) {
            if (!sgl_area_set_union(&areas[j], area.left, area.top, area.right,
                                    area.bottom)) {
                ++j;
                continue;
            }
            area = areas[j];
            --merged;
            areas[j] = areas[merged];
            j = 0;
        }
        areas[merged] = area;
        ++merged;
    }
    dirty->pending_count = merged;
}

void sgl_invalidate_area(sgl_display_t *disp, sgl_int_t left, sgl_int_t top,
                         sgl_int_t right, sgl_int_t bottom) {
    sgl_check_param(disp);
    sgl_dirty_t *dirty = &disp->dirty;
    sgl_area_t area;
    sgl_rect2area(&disp->root.rect, &area);
    if (!sgl_area_set_intersect(&area, left, top, right, bottom))
        return;
    int lock_state = sgl_lock();
    if (dirty->dirty_count == dirty->dirty_cap)
        dirty->flag_full = 1;
    if (dirty->flag_full) {
        dirty->dirty_count = 1;
        sgl_unlock(lock_state);
        return;
    }
    sgl_set_area(&dirty->dirty_areas[dirty->dirty_count], area.left, area.top,
                 area.right, area.bottom);
    dirty->dirty_count += 1;
    sgl_unlock(lock_state);
}

void sgl_invalidate_widget(sgl_widget_t *widget) {
    sgl_check_param(widget);
    if (!widget->display)
        return;
    sgl_area_t area = sgl_widget_get_bounds(widget);
    sgl_invalidate_area(widget->display, area.left, area.top, area.right,
                        area.bottom);
}

void sgl_invalidate_display(sgl_display_t *disp) {
    sgl_check_param(disp);
    sgl_invalidate_root(disp);
}

static int sgl_slice(sgl_display_t *disp, sgl_area_t *dirty_area) {
    sgl_slice_t *slice = &disp->slice;
    if (slice->slice_state == SGL_STATE_SLICE_PREPARE) {
        sgl_area2rect(dirty_area, &slice->slice_rect);
        slice->slice_height = 0;
        slice->slice_state = SGL_STATE_SLICE_RUNNING;
    }
    if (slice->slice_state == SGL_STATE_SLICE_RUNNING) {
        sgl_int_t slice_w, slice_h;
        disp->buffer_x = slice->slice_rect.x;
        disp->buffer_y = slice->slice_rect.y + slice->slice_height;
        slice_w = slice->slice_rect.w;
        size_t slice_num = (disp->fb.pixel_num / slice->slice_rect.w)
                           << disp->fb.pixel_shift;
        sgl_int_t slice_need = slice->slice_rect.h - slice->slice_height;
        slice_h = (slice_need <= slice_num) ? slice_need : (sgl_int_t)slice_num;
        disp->fb.slice_width = slice_w;
        slice->slice_height += slice_h;
        sgl_set_area(&slice->slice_area, disp->buffer_x, disp->buffer_y,
                     disp->buffer_x + slice_w - 1,
                     disp->buffer_y + slice_h - 1);
        if (slice->slice_height == slice->slice_rect.h) {
            slice->slice_state = SGL_STATE_SLICE_PREPARE;
            return 1;
        }
    }
    return 0;
}

static void sgl_draw_process(sgl_display_t *disp, sgl_widget_t *widget,
                             sgl_int_t ox, sgl_int_t oy,
                             const sgl_area_t *parent_bounds) {
    sgl_area_t bounds = *parent_bounds;
    if (!sgl_area_set_intersect(&bounds, ox, oy, ox + widget->rect.w - 1,
                                oy + widget->rect.h - 1))
        return;
    disp->logical_x = ox;
    disp->logical_y = oy;
    disp->widget_bounds = bounds;
    disp->clip_area = bounds;
    if (widget->cls->draw)
        widget->cls->draw(disp, widget);
    sgl_widget_t *child;
    sgl_list_for_each_entry(child, sgl_widget_t, &widget->children, sibling) {
        sgl_draw_process(disp, child, ox + child->rect.x, oy + child->rect.y,
                         &bounds);
    }
}

static int sgl_draw_dirty(sgl_display_t *disp, sgl_widget_t *invalidate_widget,
                          sgl_area_t *dirty_area, sgl_area_t *flush_area) {
    int ret = sgl_slice(disp, dirty_area);
    if (flush_area)
        *flush_area = disp->slice.slice_area;
    sgl_int_t ox, oy;
    sgl_widget_get_origin_coords(invalidate_widget, &ox, &oy);
    sgl_draw_process(disp, invalidate_widget, ox, oy, &disp->slice.slice_area);
    return ret;
}

int sgl_draw(sgl_display_t *disp, sgl_area_t *flush_area) {
    sgl_check_param(disp);
    sgl_dirty_t *dirty = &disp->dirty;
    if (dirty->state == SGL_STATE_DIRTY_IDLE) {
        if (dirty->dirty_count) {
            int lock_state = sgl_lock();
            // full refresh
            if (dirty->flag_full)
                sgl_invalidate_root(disp);
            // switch buffer
            sgl_area_t *temp = dirty->dirty_areas;
            dirty->dirty_areas = dirty->dirty_pending_areas;
            dirty->dirty_pending_areas = temp;
            dirty->pending_count = dirty->dirty_count;
            // reset dirty
            dirty->dirty_count = 0;
            dirty->flag_full = 0;
            sgl_unlock(lock_state);
            // merge dirty
            sgl_merge_dirty(dirty);
            // switch state
            dirty->state = SGL_STATE_DIRTY_FLUSHING;
            // a frame start
            if (disp->frame_start)
                disp->frame_start(disp->user_data);
        }
    }
    if (dirty->state == SGL_STATE_DIRTY_FLUSHING) {
        if (sgl_draw_dirty(
                disp, &disp->root,
                &disp->dirty.dirty_pending_areas[dirty->pending_count - 1],
                flush_area)) {
            dirty->pending_count -= 1;
        }
        // a frame end
        if (dirty->pending_count == 0) {
            disp->frame_count += 1;
            if (disp->frame_end)
                disp->frame_end(disp->user_data);
            // switch state
            dirty->state = SGL_STATE_DIRTY_IDLE;
        }
        return 1;
    }
    return 0;
}

int sgl_set_clip_area(sgl_display_t *disp, sgl_int_t left, sgl_int_t top,
                      sgl_int_t right, sgl_int_t bottom) {
    sgl_check_param(disp);
    disp->clip_area = disp->widget_bounds;
    return sgl_area_set_intersect(&disp->clip_area, left, top, right, bottom);
}

void sgl_restore_clip_area(sgl_display_t *disp) {
    sgl_check_param(disp);
    disp->clip_area = disp->widget_bounds;
}

void sgl_set_invalidate_full_screen(sgl_display_t *disp, uint8_t enable) {
    sgl_check_param(disp);
    disp->dirty.flag_full = enable;
}
