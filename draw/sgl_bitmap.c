// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#include "sgl_bitmap.h"
#include "sgl_common.h"

void sgl_draw_bitmap_1bpp(sgl_display_t *disp, sgl_int_t x, sgl_int_t y,
                          sgl_int_t w, sgl_int_t h, const uint8_t *bitmap,
                          sgl_color_t color) {
    sgl_check_param(disp);
    sgl_check_param(bitmap);
    sgl_normalize_rect(&x, &y, &w, &h);
    sgl_logical_offset(x, y);
    sgl_rect_t bmp = {x, y, w, h};
    if (sgl_clip_rect(&disp->clip_area, &x, &y, &w, &h))
        return;
    sgl_int_t dx = x - bmp.x;
    sgl_int_t dy = y - bmp.y;
    sgl_buffer_offset(x, y);
    for (sgl_int_t i = 0; i < h; ++i) {
        size_t index = dx + ((dy + i) >> 3) * (size_t)bmp.w;
        sgl_int_t mask = 1U << ((dy + i) & 7);
        sgl_int_t temp = y + i;
        for (sgl_int_t j = 0; j < w; ++j) {
            if (bitmap[index + j] & mask)
                disp->write_pixel(&disp->fb, x + j, temp, color);
        }
    }
}

void sgl_draw_bitmap(sgl_display_t *disp, sgl_int_t x, sgl_int_t y, sgl_int_t w,
                     sgl_int_t h, const uint8_t *bitmap) {
    sgl_check_param(disp);
    sgl_check_param(bitmap);
    sgl_normalize_rect(&x, &y, &w, &h);
    sgl_logical_offset(x, y);
    sgl_rect_t bmp = {x, y, w, h};
    if (sgl_clip_rect(&disp->clip_area, &x, &y, &w, &h))
        return;
    sgl_framebuffer_t *fb = &disp->fb;
    size_t pixel_size = fb->pixel_size;
    size_t copy_size = w * pixel_size;
    size_t src_stride = bmp.w * pixel_size;
    size_t des_stride = fb->slice_width * pixel_size;
    const uint8_t *src =
        bitmap + ((x - bmp.x) + (y - bmp.y) * (size_t)bmp.w) * pixel_size;
    sgl_buffer_offset(x, y);
    uint8_t *des = (uint8_t *)disp->fb.buffer +
                   (x + (y >> fb->pixel_shift) * fb->slice_width) * pixel_size;
    for (sgl_int_t i = 0; i < h; ++i) {
        memcpy(des, src, copy_size);
        src += src_stride;
        des += des_stride;
    }
}
