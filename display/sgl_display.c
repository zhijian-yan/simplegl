// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#include "sgl_display.h"
#include "core/sgl_core.h"
#include "draw/sgl_common.h"
#include "sgl_pixel.h"
#include <string.h>

int sgl_display_init(sgl_display_t *disp, const sgl_display_config_t *config) {
    if (!disp || !config || !config->buffer)
        return -1;
    memset(disp, 0, sizeof(sgl_display_t));
    disp->fb.buffer = config->buffer;
    disp->hor_res = config->hor_res;
    disp->ver_res = config->ver_res;
    disp->frame_start = config->frame_start;
    disp->frame_end = config->frame_end;
    if (sgl_set_pixel_format(disp, config->buffer_size, config->pixel_format))
        return -1;
    sgl_init_list_head(&disp->root.sibling);
    sgl_init_list_head(&disp->root.children);
    disp->root.vtable = &disp->background;
    sgl_set_display_rotation(disp, config->rotate);
    disp->dirty_rect = disp->root.rect;
    return 0;
}

int sgl_set_pixel_format(sgl_display_t *disp, uint32_t buffer_size,
                         sgl_pixel_format_t pixel_format) {
    uint32_t pixel_size = 1;
    uint32_t pixel_index = 0;
    uint32_t pixel_num;
    switch (pixel_format) {
        case SGL_PIXEL_FORMAT_1BIT:
            pixel_size = 1;
            pixel_index = 3;
            disp->write_pixel = sgl_write_pixel_1b;
            disp->write_pixel_hline = sgl_write_pixel_hline_1b;
            break;
        case SGL_PIXEL_FORMAT_8BIT:
            pixel_size = 1;
            pixel_index = 0;
            disp->write_pixel = sgl_write_pixel_8b;
            disp->write_pixel_hline = sgl_write_pixel_hline_8b;
            break;
        case SGL_PIXEL_FORMAT_16BIT:
            pixel_size = 2;
            pixel_index = 0;
            disp->write_pixel = sgl_write_pixel_16b;
            disp->write_pixel_hline = sgl_write_pixel_hline_16b;
            break;
        case SGL_PIXEL_FORMAT_24BIT:
            pixel_size = 3;
            pixel_index = 0;
            disp->write_pixel = sgl_write_pixel_24b;
            disp->write_pixel_hline = sgl_write_pixel_hline_24b;
            break;
        case SGL_PIXEL_FORMAT_32BIT:
            pixel_size = 4;
            pixel_index = 0;
            disp->write_pixel = sgl_write_pixel_32b;
            disp->write_pixel_hline = sgl_write_pixel_hline_32b;
            break;
    }
    pixel_num = buffer_size / pixel_size;
    if (buffer_size == 0 || pixel_num < disp->hor_res)
        return -1;
    disp->fb.pixel_size = pixel_size;
    disp->fb.pixel_index = pixel_index;
    disp->fb.pixel_num = pixel_num;
    disp->fb.buffer_size = pixel_num * pixel_size;
    return 0;
}

void sgl_clear_framebuffer(sgl_display_t *disp, uint8_t value) {
    memset(disp->fb.buffer, value, disp->fb.buffer_size);
}

void sgl_clear_rect(sgl_display_t *disp, int32_t x, int32_t y, int32_t w,
                    int32_t h, uint8_t value) {
    sgl_logical_offset(x, y);
    if (sgl_clip_rect(&disp->drawable_area, &x, &y, &w, &h))
        return;
    sgl_rotate_rect_ccw(disp, &x, &y, &w, &h);
    sgl_normalize_rect(&x, &y, &w, &h);
    sgl_buffer_offset(x, y);
    sgl_framebuffer_t *fb = &disp->fb;
    uint32_t rect_width_size = w * fb->pixel_size;
    uint32_t dirty_width_size = fb->dirty_width * fb->pixel_size;
    uint8_t *src = (uint8_t *)fb->buffer;
    src += x * fb->pixel_size + y * dirty_width_size;
    for (int i = 0; i < h; ++i) {
        memset(src, value, rect_width_size);
        src += dirty_width_size;
    }
}

void sgl_clear_widget(sgl_display_t *disp, const sgl_widget_t *widget,
                      uint8_t value) {
    sgl_clear_rect(disp, 0, 0, widget->rect.w, widget->rect.h, value);
}
