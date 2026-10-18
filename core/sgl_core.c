// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#include "sgl_core.h"
#include "draw/sgl_common.h"

#define SGL_SLICE_STATE_IDLE    0
#define SGL_SLICE_STATE_START   1
#define SGL_SLICE_STATE_RUNNING 2

static void sgl_buffer_slice(sgl_display_t *disp, sgl_slice_t *slice,
                             sgl_area_t *flush_area, sgl_rect_t dirty_rect) {
    if (slice->slice_state == SGL_SLICE_STATE_IDLE) {
        slice->slice_state = SGL_SLICE_STATE_START;
    }
    if (slice->slice_state == SGL_SLICE_STATE_START) {
        if (dirty_rect.w == 0 || dirty_rect.h == 0)
            return;
        if (disp->frame_start)
            disp->frame_start(disp->user_data);
        slice->frame_rect = dirty_rect;
        sgl_rotate_rect_ccw(disp, &slice->frame_rect.x, &slice->frame_rect.y,
                            &slice->frame_rect.w, &slice->frame_rect.h);
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
        sgl_set_area(flush_area, disp->buffer_ox, disp->buffer_oy,
                     disp->buffer_ox + w_piece - 1,
                     disp->buffer_oy + h_piece - 1);
        slice->slice_area = *flush_area;
        sgl_rotate_area_cw(disp, &slice->slice_area.left,
                           &slice->slice_area.top, &slice->slice_area.right,
                           &slice->slice_area.bottom);
        if (slice->slice_count == slice->frame_rect.h) {
            slice->slice_state = SGL_SLICE_STATE_IDLE;
        }
    }
}

static void sgl_draw_proc(sgl_display_t *disp, sgl_widget_t *widget, int32_t ox,
                          int32_t oy, const sgl_area_t *parent_bounds) {
    sgl_area_t bounds;
    if (sgl_set_area_within(&bounds, parent_bounds, ox, oy,
                            ox + widget->rect.w - 1, oy + widget->rect.h - 1))
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
    sgl_buffer_slice(disp, &disp->slice, flush_area, dirty_rect);
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
    return sgl_set_area_within(&disp->clip_area, &disp->widget_bounds, left,
                               top, right, bottom);
}

void sgl_reset_clip_area(sgl_display_t *disp) {
    disp->clip_area = disp->widget_bounds;
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
    sgl_set_rect(&disp->root.rect, 0, 0, disp->max_x + 1, disp->max_y + 1);
}
