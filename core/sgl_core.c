// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#include "sgl_core.h"
#include "draw/sgl_common.h"

#define SGL_SLICE_STATE_IDLE    0
#define SGL_SLICE_STATE_START   1
#define SGL_SLICE_STATE_RUNNING 2

static inline void sgl_set_rect(sgl_rect_t *rect, int32_t x, int32_t y,
                                int32_t w, int32_t h) {
    rect->x = x;
    rect->y = y;
    rect->w = w;
    rect->h = h;
}

static inline void sgl_set_area(sgl_area_t *area, int32_t left, int32_t top,
                                int32_t right, int32_t bottom) {
    area->left = left;
    area->top = top;
    area->right = right;
    area->bottom = bottom;
}

static inline void sgl_area2rect(const sgl_area_t *area, sgl_rect_t *rect) {
    rect->x = area->left;
    rect->y = area->top;
    rect->w = area->right - area->left + 1;
    rect->h = area->bottom - area->top + 1;
}

static inline void sgl_rect2area(const sgl_rect_t *rect, sgl_area_t *area) {
    area->left = rect->x;
    area->top = rect->y;
    if (rect->w > 0)
        area->right = rect->x + rect->w - 1;
    else
        area->right = rect->x - rect->w - 1;
    if (rect->h > 0)
        area->bottom = rect->y + rect->h - 1;
    else
        area->bottom = rect->y - rect->h - 1;
}

static inline int sgl_set_area_within(sgl_area_t *area,
                                      const sgl_area_t *bounds, int32_t left,
                                      int32_t top, int32_t right,
                                      int32_t bottom) {
    if (sgl_check_area(bounds, left, top, right, bottom))
        return -1;
    if (left < bounds->left)
        left = bounds->left;
    if (top < bounds->top)
        top = bounds->top;
    if (right > bounds->right)
        right = bounds->right;
    if (bottom > bounds->bottom)
        bottom = bounds->bottom;
    sgl_set_area(area, left, top, right, bottom);
    return 0;
}

static void sgl_buffer_slice(sgl_display_t *disp) {
    uint32_t w_piece, h_piece;
    sgl_rect_t temp;
    if (disp->slice_state == SGL_SLICE_STATE_IDLE) {
        disp->slice_state = SGL_SLICE_STATE_START;
    }
    if (disp->slice_state == SGL_SLICE_STATE_START) {
        if (disp->dirty_rect.w == 0 || disp->dirty_rect.h == 0)
            return;
        if (disp->frame_start)
            disp->frame_start(disp->user_data);
        disp->frame_rect = disp->dirty_rect;
        sgl_rotate_rect_ccw(disp, &disp->frame_rect.x, &disp->frame_rect.y,
                            &disp->frame_rect.w, &disp->frame_rect.h);
        sgl_normalize_rect(&disp->frame_rect.x, &disp->frame_rect.y,
                           &disp->frame_rect.w, &disp->frame_rect.h);
        disp->slice_count = 0;
        disp->slice_state = SGL_SLICE_STATE_RUNNING;
    }
    if (disp->slice_state == SGL_SLICE_STATE_RUNNING) {
        disp->buffer_offset_x = disp->frame_rect.x;
        disp->buffer_offset_y = disp->frame_rect.y + disp->slice_count;
        w_piece = disp->frame_rect.w;
        h_piece = (disp->fb.pixel_num / disp->frame_rect.w)
                  << disp->fb.pixel_index;
        if (h_piece > disp->frame_rect.h - disp->slice_count)
            h_piece = disp->frame_rect.h - disp->slice_count;
        disp->fb.buffer_width = w_piece;
        disp->slice_count += h_piece;
        sgl_set_rect(&disp->slice_rect, disp->buffer_offset_x,
                     disp->buffer_offset_y, w_piece, h_piece);
        temp = disp->slice_rect;
        sgl_rotate_rect_cw(disp, &temp.x, &temp.y, &temp.w, &temp.h);
        sgl_normalize_rect(&temp.x, &temp.y, &temp.w, &temp.h);
        sgl_rect2area(&temp, &disp->slice_area);
        if (disp->slice_count == disp->frame_rect.h) {
            disp->slice_state = SGL_SLICE_STATE_IDLE;
        }
    }
}

static void sgl_draw_proc(sgl_display_t *disp, sgl_widget_t *widget,
                          int32_t offset_x, int32_t offset_y,
                          const sgl_area_t *parent_bounds) {
    sgl_widget_t *child;
    sgl_area_t bounds;
    disp->logical_offset_x = offset_x;
    disp->logical_offset_y = offset_y;
    if (sgl_set_area_within(&bounds, parent_bounds, offset_x, offset_y,
                            offset_x + widget->rect.w - 1,
                            offset_y + widget->rect.h - 1) == 0) {
        disp->widget_bounds = bounds;
        disp->drawable_area = bounds;
        if (widget->vtable->draw)
            widget->vtable->draw(disp, widget);
    }
    sgl_list_for_each_entry(child, &widget->children, sibling) {
        sgl_draw_proc(disp, child, offset_x + child->rect.x,
                      offset_y + child->rect.y, &bounds);
    }
}

void sgl_handler(sgl_display_t *disp) {
    sgl_buffer_slice(disp);
    sgl_draw_proc(disp, &disp->root, disp->root.rect.x, disp->root.rect.y,
                  &disp->slice_area);
    disp->flush(disp->fb.buffer, &disp->slice_rect);
    if (disp->slice_state == SGL_SLICE_STATE_IDLE) {
        ++disp->frame_count;
        if (disp->frame_end)
            disp->frame_end(disp->user_data);
    }
}

void sgl_set_dirty_area(sgl_display_t *disp, int32_t left, int32_t top,
                        int32_t right, int32_t bottom) {
    // sgl_set_area_within(&disp->dirty_area, &disp->root.area, left, top,
    //                     right, bottom);
}

void sgl_reset_dirty_area(sgl_display_t *disp) {
    // disp->dirty_area = disp->root.area;
}

int sgl_set_drawable_area(sgl_display_t *disp, int32_t left, int32_t top,
                          int32_t right, int32_t bottom) {
    return sgl_set_area_within(&disp->drawable_area, &disp->widget_bounds, left,
                               top, right, bottom);
}

void sgl_reset_drawable_area(sgl_display_t *disp) {
    disp->drawable_area = disp->widget_bounds;
}

void sgl_set_display_rotation(sgl_display_t *disp, sgl_rotate_t rotate) {
    disp->rotate = rotate;
    switch (rotate) {
        case SGL_ROTATE_0:
        case SGL_ROTATE_180:
            disp->max_x = disp->hor_res - 1;
            disp->max_y = disp->ver_res - 1;
            break;
        case SGL_ROTATE_90:
        case SGL_ROTATE_270:
            disp->max_x = disp->ver_res - 1;
            disp->max_y = disp->hor_res - 1;
            break;
    }
    // sgl_set_area(&disp->root.area, 0, 0, disp->max_x, disp->max_y);
    sgl_set_rect(&disp->root.rect, 0, 0, disp->max_x + 1, disp->max_y + 1);
}
