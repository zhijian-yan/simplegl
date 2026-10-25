// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#include "sgl_display.h"
#include "core/sgl_core.h"
#include "draw/sgl_common.h"
#include "sgl_pixel.h"

static int sgl_set_pixel_format(sgl_display_t *disp, int32_t buffer_size,
                                sgl_pixel_format_t pixel_format);

int sgl_display_init(sgl_display_t *disp, const sgl_display_config_t *config) {
    if (!disp || !config || !config->framebuffer || !config->dirty_buffer ||
        !config->dirty_pending || !config->dirty_num)
        return -1;
    memset(disp, 0, sizeof(sgl_display_t));
    disp->fb.buffer = config->framebuffer;
    disp->dirty.dirty_buffer = config->dirty_buffer;
    disp->dirty.dirty_pending = config->dirty_pending;
    disp->dirty.dirty_num = config->dirty_num;
    disp->frame_start = config->frame_start;
    disp->frame_end = config->frame_end;
    if (sgl_set_pixel_format(disp, config->buffer_size, config->pixel_format))
        return -1;
    sgl_widget_init(&disp->root, &disp->background, 0, 0, config->hor_res,
                    config->ver_res, NULL);
    disp->root.disp = disp;
    sgl_set_full_screen(disp, config->flag_full_screen);
    sgl_set_idle_send(disp, config->flag_idle_send);
    sgl_invalidate_area(disp, 0, 0, config->hor_res - 1, config->ver_res - 1);
    return 0;
}

static int sgl_set_pixel_format(sgl_display_t *disp, int32_t buffer_size,
                                sgl_pixel_format_t pixel_format) {
    int32_t pixel_size = 1;
    int32_t pixel_index = 0;
    int32_t pixel_num;
    switch (pixel_format) {
        case SGL_PIXEL_FORMAT_1BIT:
            pixel_size = 1;
            pixel_index = 3;
            disp->write_pixel = sgl_write_pixel_1bpp;
            disp->write_pixel_hline = sgl_write_pixel_hline_1bpp;
            break;
        case SGL_PIXEL_FORMAT_8BIT:
            pixel_size = 1;
            pixel_index = 0;
            disp->write_pixel = sgl_write_pixel_8bpp;
            disp->write_pixel_hline = sgl_write_pixel_hline_8bpp;
            break;
        case SGL_PIXEL_FORMAT_16BIT:
            pixel_size = 2;
            pixel_index = 0;
            disp->write_pixel = sgl_write_pixel_16bpp;
            disp->write_pixel_hline = sgl_write_pixel_hline_16bpp;
            break;
        case SGL_PIXEL_FORMAT_24BIT:
            pixel_size = 3;
            pixel_index = 0;
            disp->write_pixel = sgl_write_pixel_24bpp;
            disp->write_pixel_hline = sgl_write_pixel_hline_24bpp;
            break;
        case SGL_PIXEL_FORMAT_32BIT:
            pixel_size = 4;
            pixel_index = 0;
            disp->write_pixel = sgl_write_pixel_32bpp;
            disp->write_pixel_hline = sgl_write_pixel_hline_32bpp;
            break;
    }
    pixel_num = buffer_size / pixel_size;
    if (buffer_size == 0 || pixel_num < disp->root.rect.w)
        return -1;
    disp->fb.pixel_size = pixel_size;
    disp->fb.pixel_index = pixel_index;
    disp->fb.pixel_num = pixel_num;
    disp->fb.buffer_size = pixel_num * pixel_size;
    return 0;
}

void sgl_clear_framebuffer(sgl_display_t *disp, uint8_t value) {
    memset(disp->fb.buffer, value, (size_t)disp->fb.buffer_size);
}

void sgl_clear_rect(sgl_display_t *disp, int32_t x, int32_t y, int32_t w,
                    int32_t h, uint8_t value) {
    sgl_normalize_rect(&x, &y, &w, &h);
    sgl_logical_offset(x, y);
    if (sgl_clip_rect(&disp->clip_area, &x, &y, &w, &h))
        return;
    sgl_buffer_offset(x, y);
    sgl_framebuffer_t *fb = &disp->fb;
    int32_t rect_width_size = w * fb->pixel_size;
    int32_t dirty_width_size = fb->dirty_width * fb->pixel_size;
    uint8_t *src = (uint8_t *)fb->buffer;
    src += x * fb->pixel_size + y * dirty_width_size;
    for (int32_t i = 0; i < h; ++i) {
        memset(src, value, (size_t)rect_width_size);
        src += dirty_width_size;
    }
}

void sgl_clear_widget(const sgl_widget_t *widget, uint8_t value) {
    sgl_clear_rect(widget->disp, 0, 0, widget->rect.w, widget->rect.h, value);
}

void sgl_set_framebuffer(sgl_display_t *disp, void *buffer) {
    disp->fb.buffer = buffer;
}

uint32_t sgl_get_frame_count(sgl_display_t *disp) {
    return disp->frame_count;
}

void sgl_reset_frame_count(sgl_display_t *disp) {
    disp->frame_count = 0;
}
