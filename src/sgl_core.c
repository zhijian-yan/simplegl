// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#include "../inc/sgl_core.h"
#include "../inc/sgl_common.h"
#include "../inc/sgl_pixel.h"
#include "../inc/sgl_widget.h"
#include <string.h>

static int sgl_set_color_format(sgl_screen_t *scr, uint32_t buffer_size,
                                sgl_color_format_t color_format);

int sgl_init(sgl_screen_t *scr, sgl_config_t *config) {
    if (!scr || !config || !config->buffer)
        return -1;
    memset(scr, 0, sizeof(sgl_screen_t));
    scr->buffer = config->buffer;
    scr->hor_res = config->hor_res;
    scr->ver_res = config->ver_res;
    scr->frame_start = config->frame_start;
    scr->frame_end = config->frame_end;
    if (sgl_set_color_format(scr, config->buffer_size, config->color_format))
        return -1;
    INIT_LIST_HEAD(&scr->root.sibling);
    INIT_LIST_HEAD(&scr->root.children);
    sgl_set_screen_rotation(scr, config->rotate);
    scr->dirty_rect = scr->root.rect;
    return 0;
}

static void sgl_buffer_slice(sgl_screen_t *scr) {
    uint32_t w_piece, h_piece;
    sgl_rect_t temp;
    if (scr->slice_state == SGL_SLICE_STATE_IDLE) {
        scr->slice_state = SGL_SLICE_STATE_START;
    }
    if (scr->slice_state == SGL_SLICE_STATE_START) {
        if (scr->dirty_rect.w == 0 || scr->dirty_rect.h == 0)
            return;
        if (scr->frame_start)
            scr->frame_start(scr->user_data);
        scr->frame_rect = scr->dirty_rect;
        sgl_rotate_rect_ccw(scr, &scr->frame_rect.x, &scr->frame_rect.y,
                            &scr->frame_rect.w, &scr->frame_rect.h);
        sgl_normalize_rect(&scr->frame_rect.x, &scr->frame_rect.y,
                           &scr->frame_rect.w, &scr->frame_rect.h);
        scr->slice_count = 0;
        scr->slice_state = SGL_SLICE_STATE_RUNNING;
    }
    if (scr->slice_state == SGL_SLICE_STATE_RUNNING) {
        scr->buffer_offset_x = scr->frame_rect.x;
        scr->buffer_offset_y = scr->frame_rect.y + scr->slice_count;
        w_piece = scr->frame_rect.w;
        h_piece = (scr->pixel_num / scr->frame_rect.w) << scr->pixel_index;
        if (h_piece > scr->frame_rect.h - scr->slice_count)
            h_piece = scr->frame_rect.h - scr->slice_count;
        scr->buffer_width = w_piece;
        scr->slice_count += h_piece;
        sgl_set_rect(&scr->slice_rect, scr->buffer_offset_x,
                     scr->buffer_offset_y, w_piece, h_piece);
        temp = scr->slice_rect;
        sgl_rotate_rect_cw(scr, &temp.x, &temp.y, &temp.w, &temp.h);
        sgl_normalize_rect(&temp.x, &temp.y, &temp.w, &temp.h);
        sgl_rect2area(&temp, &scr->slice_area);
        if (scr->slice_count == scr->frame_rect.h) {
            scr->slice_state = SGL_SLICE_STATE_IDLE;
        }
    }
}

static void sgl_draw(sgl_screen_t *scr, sgl_widget_t *widget, int32_t offset_x,
                     int32_t offset_y, const sgl_area_t *parent_bounds) {
    sgl_widget_t *child;
    sgl_widget_draw_t draw;
    sgl_area_t bounds;
    scr->logical_offset_x = offset_x;
    scr->logical_offset_y = offset_y;
    if (sgl_set_area_within(&bounds, parent_bounds, offset_x, offset_y,
                            offset_x + widget->rect.w - 1,
                            offset_y + widget->rect.h - 1) == 0) {
        scr->widget_bounds = bounds;
        scr->drawable_area = bounds;
        draw = sgl_widget_get_draw(widget->draw_index);
        if (draw)
            draw(scr, widget);
    }
    list_for_each_entry(child, &widget->children, sibling) {
        sgl_draw(scr, child, offset_x + child->rect.x, offset_y + child->rect.y,
                 &bounds);
    }
}

void sgl_handler(sgl_screen_t *scr) {
    sgl_buffer_slice(scr);
    sgl_draw(scr, &scr->root, scr->root.rect.x, scr->root.rect.y,
             &scr->slice_area);
    scr->flush(scr->buffer, &scr->slice_rect);
    if (scr->slice_state == SGL_SLICE_STATE_IDLE) {
        ++scr->frame_count;
        if (scr->frame_end)
            scr->frame_end(scr->user_data);
    }
}

void sgl_set_flush(sgl_screen_t *scr,
                   void (*flush)(void *buffer, sgl_rect_t *refresh)) {
    scr->flush = flush;
}

int sgl_set_draw_pixel(sgl_screen_t *scr, uint32_t hor_res,
                       uint32_t buffer_size, uint32_t pixel_size,
                       void (*draw_pixel)(sgl_screen_t *scr, int32_t x,
                                          int32_t y, uint32_t color)) {
    uint32_t pixel_num;
    if (buffer_size == 0 || pixel_size == 0)
        return -1;
    pixel_num = buffer_size / pixel_size;
    if (pixel_num < hor_res)
        return -1;
    scr->draw_pixel = draw_pixel;
    scr->pixel_num = pixel_num;
    scr->pixel_size = pixel_size;
    scr->buffer_size = pixel_num * pixel_size;
    return 0;
}

static int sgl_set_color_format(sgl_screen_t *scr, uint32_t buffer_size,
                                sgl_color_format_t color_format) {
    int ret = 0;
    scr->pixel_index = 0;
    switch (color_format) {
        case SGL_COLOR_FORMAT_MONO:
            ret = sgl_set_draw_pixel(scr, scr->hor_res, buffer_size, 1,
                                     sgl_draw_pixel_mono);
            scr->pixel_index = 3;
            break;
        case SGL_COLOR_FORMAT_RGB332:
            ret = sgl_set_draw_pixel(scr, scr->hor_res, buffer_size, 1,
                                     sgl_draw_pixel_rgb332);
            break;
        case SGL_COLOR_FORMAT_RGB565:
            ret = sgl_set_draw_pixel(scr, scr->hor_res, buffer_size, 2,
                                     sgl_draw_pixel_rgb565);
            break;
        case SGL_COLOR_FORMAT_RGB565SWAP:
            ret = sgl_set_draw_pixel(scr, scr->hor_res, buffer_size, 2,
                                     sgl_draw_pixel_rgb565swap);
            break;
        case SGL_COLOR_FORMAT_BGR565:
            ret = sgl_set_draw_pixel(scr, scr->hor_res, buffer_size, 2,
                                     sgl_draw_pixel_bgr565);
            break;
        case SGL_COLOR_FORMAT_RGB888:
            ret = sgl_set_draw_pixel(scr, scr->hor_res, buffer_size, 3,
                                     sgl_draw_pixel_rgb888);
            break;
        case SGL_COLOR_FORMAT_BGR888:
            ret = sgl_set_draw_pixel(scr, scr->hor_res, buffer_size, 3,
                                     sgl_draw_pixel_bgr888);
            break;
        case SGL_COLOR_FORMAT_XRGB8888:
            ret = sgl_set_draw_pixel(scr, scr->hor_res, buffer_size, 4,
                                     sgl_draw_pixel_xrgb8888);
            break;
        case SGL_COLOR_FORMAT_XBGR8888:
            ret = sgl_set_draw_pixel(scr, scr->hor_res, buffer_size, 4,
                                     sgl_draw_pixel_xbgr8888);
            break;
        case SGL_COLOR_FORMAT_ARGB8888:
            ret = sgl_set_draw_pixel(scr, scr->hor_res, buffer_size, 4,
                                     sgl_draw_pixel_argb8888);
            break;
        case SGL_COLOR_FORMAT_ABGR8888:
            ret = sgl_set_draw_pixel(scr, scr->hor_res, buffer_size, 4,
                                     sgl_draw_pixel_abgr8888);
            break;
        case SGL_COLOR_FORMAT_RGBA8888:
            ret = sgl_set_draw_pixel(scr, scr->hor_res, buffer_size, 4,
                                     sgl_draw_pixel_rgba8888);
            break;
        case SGL_COLOR_FORMAT_BGRA8888:
            ret = sgl_set_draw_pixel(scr, scr->hor_res, buffer_size, 4,
                                     sgl_draw_pixel_bgra8888);
            break;
    }
    return ret;
}

void sgl_set_dirty_area(sgl_screen_t *scr, int32_t left, int32_t top,
                        int32_t right, int32_t bottom) {
    // sgl_set_area_within(&scr->dirty_area, &scr->root.area, left, top,
    //                     right, bottom);
}

void sgl_reset_dirty_area(sgl_screen_t *scr) {
    // scr->dirty_area = scr->root.area;
}

int sgl_set_drawable_area(sgl_screen_t *scr, int32_t left, int32_t top,
                          int32_t right, int32_t bottom) {
    return sgl_set_area_within(&scr->drawable_area, &scr->widget_bounds, left,
                               top, right, bottom);
}

void sgl_reset_drawable_area(sgl_screen_t *scr) {
    scr->drawable_area = scr->widget_bounds;
}

void sgl_set_screen_rotation(sgl_screen_t *scr, sgl_rotate_t rotate) {
    scr->rotate = rotate;
    switch (rotate) {
        case SGL_ROTATE_0:
        case SGL_ROTATE_180:
            scr->max_x = scr->hor_res - 1;
            scr->max_y = scr->ver_res - 1;
            break;
        case SGL_ROTATE_90:
        case SGL_ROTATE_270:
            scr->max_x = scr->ver_res - 1;
            scr->max_y = scr->hor_res - 1;
            break;
    }
    // sgl_set_area(&scr->root.area, 0, 0, scr->max_x, scr->max_y);
    sgl_set_rect(&scr->root.rect, 0, 0, scr->max_x + 1, scr->max_y + 1);
}

uint32_t sgl_get_frame_count(sgl_screen_t *scr) {
    return scr->frame_count;
}

void sgl_reset_frame_count(sgl_screen_t *scr) {
    scr->frame_count = 0;
}

void sgl_clear_buffer(sgl_screen_t *scr, uint8_t value) {
    memset(scr->buffer, value, scr->buffer_size);
}
