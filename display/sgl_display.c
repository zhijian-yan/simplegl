// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#include "sgl_display.h"
#include "core/sgl_core.h"
#include "sgl_pixel.h"
#include <string.h>

static int sgl_set_color_format(sgl_display_t *disp, uint32_t buffer_size,
                                sgl_color_format_t color_format);

int sgl_display_init(sgl_display_t *disp, const sgl_display_config_t *config) {
    if (!disp || !config || !config->buffer)
        return -1;
    memset(disp, 0, sizeof(sgl_display_t));
    disp->fb.buffer = config->buffer;
    disp->hor_res = config->hor_res;
    disp->ver_res = config->ver_res;
    disp->frame_start = config->frame_start;
    disp->frame_end = config->frame_end;
    if (sgl_set_color_format(disp, config->buffer_size, config->color_format))
        return -1;
    sgl_init_list_head(&disp->root.sibling);
    sgl_init_list_head(&disp->root.children);
    disp->root.vtable = &disp->background;
    sgl_set_display_rotation(disp, config->rotate);
    disp->dirty_rect = disp->root.rect;
    return 0;
}

int sgl_set_write_pixel(sgl_display_t *disp, uint32_t hor_res,
                        uint32_t buffer_size, uint32_t pixel_size,
                        sgl_write_pixel_t write_pixel) {
    uint32_t pixel_num;
    if (buffer_size == 0 || pixel_size == 0)
        return -1;
    pixel_num = buffer_size / pixel_size;
    if (pixel_num < hor_res)
        return -1;
    disp->write_pixel = write_pixel;
    disp->fb.pixel_num = pixel_num;
    disp->fb.pixel_size = pixel_size;
    disp->fb.buffer_size = pixel_num * pixel_size;
    return 0;
}

static int sgl_set_color_format(sgl_display_t *disp, uint32_t buffer_size,
                                sgl_color_format_t color_format) {
    int ret = 0;
    disp->fb.pixel_index = 0;
    switch (color_format) {
        case SGL_COLOR_FORMAT_MONO:
            ret = sgl_set_write_pixel(disp, disp->hor_res, buffer_size, 1,
                                      sgl_write_pixel_mono);
            disp->fb.pixel_index = 3;
            break;
        case SGL_COLOR_FORMAT_RGB332:
            ret = sgl_set_write_pixel(disp, disp->hor_res, buffer_size, 1,
                                      sgl_write_pixel_rgb332);
            break;
        case SGL_COLOR_FORMAT_RGB565:
            ret = sgl_set_write_pixel(disp, disp->hor_res, buffer_size, 2,
                                      sgl_write_pixel_rgb565);
            break;
        case SGL_COLOR_FORMAT_RGB565SWAP:
            ret = sgl_set_write_pixel(disp, disp->hor_res, buffer_size, 2,
                                      sgl_write_pixel_rgb565swap);
            break;
        case SGL_COLOR_FORMAT_BGR565:
            ret = sgl_set_write_pixel(disp, disp->hor_res, buffer_size, 2,
                                      sgl_write_pixel_bgr565);
            break;
        case SGL_COLOR_FORMAT_RGB888:
            ret = sgl_set_write_pixel(disp, disp->hor_res, buffer_size, 3,
                                      sgl_write_pixel_rgb888);
            break;
        case SGL_COLOR_FORMAT_BGR888:
            ret = sgl_set_write_pixel(disp, disp->hor_res, buffer_size, 3,
                                      sgl_write_pixel_bgr888);
            break;
        case SGL_COLOR_FORMAT_XRGB8888:
            ret = sgl_set_write_pixel(disp, disp->hor_res, buffer_size, 4,
                                      sgl_write_pixel_xrgb8888);
            break;
        case SGL_COLOR_FORMAT_XBGR8888:
            ret = sgl_set_write_pixel(disp, disp->hor_res, buffer_size, 4,
                                      sgl_write_pixel_xbgr8888);
            break;
        case SGL_COLOR_FORMAT_ARGB8888:
            ret = sgl_set_write_pixel(disp, disp->hor_res, buffer_size, 4,
                                      sgl_write_pixel_argb8888);
            break;
        case SGL_COLOR_FORMAT_ABGR8888:
            ret = sgl_set_write_pixel(disp, disp->hor_res, buffer_size, 4,
                                      sgl_write_pixel_abgr8888);
            break;
        case SGL_COLOR_FORMAT_RGBA8888:
            ret = sgl_set_write_pixel(disp, disp->hor_res, buffer_size, 4,
                                      sgl_write_pixel_rgba8888);
            break;
        case SGL_COLOR_FORMAT_BGRA8888:
            ret = sgl_set_write_pixel(disp, disp->hor_res, buffer_size, 4,
                                      sgl_write_pixel_bgra8888);
            break;
    }
    return ret;
}

void sgl_set_flush(sgl_display_t *disp,
                   void (*flush)(void *buffer, sgl_rect_t *refresh)) {
    disp->flush = flush;
}

uint32_t sgl_get_frame_count(const sgl_display_t *disp) {
    return disp->frame_count;
}

void sgl_reset_frame_count(sgl_display_t *disp) {
    disp->frame_count = 0;
}

void sgl_clear_buffer(sgl_display_t *disp, uint8_t value) {
    memset(disp->fb.buffer, value, disp->fb.buffer_size);
}
