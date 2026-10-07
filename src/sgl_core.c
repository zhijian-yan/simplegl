// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#include "../inc/sgl_core.h"
#include "../inc/sgl_common.h"
#include "../inc/sgl_pixel.h"
#include "../inc/sgl_widget.h"
#include <string.h>

static int sgl_set_color_format(sgl_display_t *disp, uint32_t buffer_size,
                                sgl_color_format_t color_format);

int sgl_init(sgl_display_t *disp, sgl_config_t *config) {
    if (!disp || !config || !config->buffer)
        return -1;
    memset(disp, 0, sizeof(sgl_display_t));
    disp->buffer = config->buffer;
    disp->hor_res = config->hor_res;
    disp->ver_res = config->ver_res;
    disp->frame_start = config->frame_start;
    disp->frame_end = config->frame_end;
    if (sgl_set_color_format(disp, config->buffer_size, config->color_format))
        return -1;
    INIT_LIST_HEAD(&disp->root.sibling);
    INIT_LIST_HEAD(&disp->root.children);
    disp->root.vtable = &disp->background;
    sgl_set_display_rotation(disp, config->rotate);
    disp->dirty_rect = disp->root.rect;
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
        h_piece = (disp->pixel_num / disp->frame_rect.w) << disp->pixel_index;
        if (h_piece > disp->frame_rect.h - disp->slice_count)
            h_piece = disp->frame_rect.h - disp->slice_count;
        disp->buffer_width = w_piece;
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

static void sgl_draw(sgl_display_t *disp, sgl_widget_t *widget,
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
    list_for_each_entry(child, &widget->children, sibling) {
        sgl_draw(disp, child, offset_x + child->rect.x,
                 offset_y + child->rect.y, &bounds);
    }
}

void sgl_handler(sgl_display_t *disp) {
    sgl_buffer_slice(disp);
    sgl_draw(disp, &disp->root, disp->root.rect.x, disp->root.rect.y,
             &disp->slice_area);
    disp->flush(disp->buffer, &disp->slice_rect);
    if (disp->slice_state == SGL_SLICE_STATE_IDLE) {
        ++disp->frame_count;
        if (disp->frame_end)
            disp->frame_end(disp->user_data);
    }
}

void sgl_set_flush(sgl_display_t *disp,
                   void (*flush)(void *buffer, sgl_rect_t *refresh)) {
    disp->flush = flush;
}

int sgl_set_draw_pixel(sgl_display_t *disp, uint32_t hor_res,
                       uint32_t buffer_size, uint32_t pixel_size,
                       void (*draw_pixel)(sgl_display_t *disp, int32_t x,
                                          int32_t y, uint32_t color)) {
    uint32_t pixel_num;
    if (buffer_size == 0 || pixel_size == 0)
        return -1;
    pixel_num = buffer_size / pixel_size;
    if (pixel_num < hor_res)
        return -1;
    disp->draw_pixel = draw_pixel;
    disp->pixel_num = pixel_num;
    disp->pixel_size = pixel_size;
    disp->buffer_size = pixel_num * pixel_size;
    return 0;
}

static int sgl_set_color_format(sgl_display_t *disp, uint32_t buffer_size,
                                sgl_color_format_t color_format) {
    int ret = 0;
    disp->pixel_index = 0;
    switch (color_format) {
        case SGL_COLOR_FORMAT_MONO:
            ret = sgl_set_draw_pixel(disp, disp->hor_res, buffer_size, 1,
                                     sgl_draw_pixel_mono);
            disp->pixel_index = 3;
            break;
        case SGL_COLOR_FORMAT_RGB332:
            ret = sgl_set_draw_pixel(disp, disp->hor_res, buffer_size, 1,
                                     sgl_draw_pixel_rgb332);
            break;
        case SGL_COLOR_FORMAT_RGB565:
            ret = sgl_set_draw_pixel(disp, disp->hor_res, buffer_size, 2,
                                     sgl_draw_pixel_rgb565);
            break;
        case SGL_COLOR_FORMAT_RGB565SWAP:
            ret = sgl_set_draw_pixel(disp, disp->hor_res, buffer_size, 2,
                                     sgl_draw_pixel_rgb565swap);
            break;
        case SGL_COLOR_FORMAT_BGR565:
            ret = sgl_set_draw_pixel(disp, disp->hor_res, buffer_size, 2,
                                     sgl_draw_pixel_bgr565);
            break;
        case SGL_COLOR_FORMAT_RGB888:
            ret = sgl_set_draw_pixel(disp, disp->hor_res, buffer_size, 3,
                                     sgl_draw_pixel_rgb888);
            break;
        case SGL_COLOR_FORMAT_BGR888:
            ret = sgl_set_draw_pixel(disp, disp->hor_res, buffer_size, 3,
                                     sgl_draw_pixel_bgr888);
            break;
        case SGL_COLOR_FORMAT_XRGB8888:
            ret = sgl_set_draw_pixel(disp, disp->hor_res, buffer_size, 4,
                                     sgl_draw_pixel_xrgb8888);
            break;
        case SGL_COLOR_FORMAT_XBGR8888:
            ret = sgl_set_draw_pixel(disp, disp->hor_res, buffer_size, 4,
                                     sgl_draw_pixel_xbgr8888);
            break;
        case SGL_COLOR_FORMAT_ARGB8888:
            ret = sgl_set_draw_pixel(disp, disp->hor_res, buffer_size, 4,
                                     sgl_draw_pixel_argb8888);
            break;
        case SGL_COLOR_FORMAT_ABGR8888:
            ret = sgl_set_draw_pixel(disp, disp->hor_res, buffer_size, 4,
                                     sgl_draw_pixel_abgr8888);
            break;
        case SGL_COLOR_FORMAT_RGBA8888:
            ret = sgl_set_draw_pixel(disp, disp->hor_res, buffer_size, 4,
                                     sgl_draw_pixel_rgba8888);
            break;
        case SGL_COLOR_FORMAT_BGRA8888:
            ret = sgl_set_draw_pixel(disp, disp->hor_res, buffer_size, 4,
                                     sgl_draw_pixel_bgra8888);
            break;
    }
    return ret;
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

uint32_t sgl_get_frame_count(sgl_display_t *disp) {
    return disp->frame_count;
}

void sgl_reset_frame_count(sgl_display_t *disp) {
    disp->frame_count = 0;
}

void sgl_clear_buffer(sgl_display_t *disp, uint8_t value) {
    memset(disp->buffer, value, disp->buffer_size);
}
