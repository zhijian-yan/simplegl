// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#include "sgl_pixel.h"

static inline uint8_t *sgl_get_pixel(sgl_framebuffer_t *fb, int32_t x,
                                     int32_t y, int32_t pixel_size,
                                     int32_t shift) {
    return (uint8_t *)fb->buffer +
           (x + (y >> shift) * fb->dirty_width) * pixel_size;
}

void sgl_write_pixel_1b(sgl_framebuffer_t *fb, int32_t x, int32_t y,
                        uint32_t format_color) {
    uint8_t *pixel = sgl_get_pixel(fb, x, y, 1, 3);
    uint8_t mask = 1U << (y & 7);
    switch (format_color) {
        case SGL_MONO_BLACK:
            *(uint8_t *)pixel &= ~mask;
            break;
        case SGL_MONO_WHITE:
            *(uint8_t *)pixel |= mask;
            break;
        case SGL_MONO_INVERT:
            *(uint8_t *)pixel ^= mask;
            break;
    }
}

void sgl_write_pixel_8b(sgl_framebuffer_t *fb, int32_t x, int32_t y,
                        uint32_t format_color) {
    uint8_t *pixel = sgl_get_pixel(fb, x, y, 1, 0);
    *(uint8_t *)pixel = (uint8_t)format_color;
}

void sgl_write_pixel_16b(sgl_framebuffer_t *fb, int32_t x, int32_t y,
                         uint32_t format_color) {
    uint8_t *pixel = sgl_get_pixel(fb, x, y, 2, 0);
    *(uint16_t *)pixel = (uint16_t)format_color;
}

void sgl_write_pixel_24b(sgl_framebuffer_t *fb, int32_t x, int32_t y,
                         uint32_t format_color) {
    uint8_t *pixel = sgl_get_pixel(fb, x, y, 3, 0);
    *(uint8_t *)pixel = (uint8_t)format_color;
    *((uint8_t *)pixel + 1) = (uint8_t)(format_color >> 8);
    *((uint8_t *)pixel + 2) = (uint8_t)(format_color >> 16);
}

void sgl_write_pixel_32b(sgl_framebuffer_t *fb, int32_t x, int32_t y,
                         uint32_t format_color) {
    uint8_t *pixel = sgl_get_pixel(fb, x, y, 4, 0);
    *(uint32_t *)pixel = (uint32_t)format_color;
}

void sgl_write_pixel_hline_1b(sgl_framebuffer_t *fb, int32_t x, int32_t y,
                              int32_t len, uint32_t format_color) {
    uint8_t *pixel = sgl_get_pixel(fb, x, y, 1, 3);
    int32_t step = (len > 0) ? 1 : -1;
    if (len < 0) {
        len = -len;
    }
    for (int32_t i = 0; i < len; ++i) {
        uint8_t mask = 1U << (y & 7);
        switch (format_color) {
            case SGL_MONO_BLACK:
                *(uint8_t *)pixel &= ~mask;
                break;
            case SGL_MONO_WHITE:
                *(uint8_t *)pixel |= mask;
                break;
            case SGL_MONO_INVERT:
                *(uint8_t *)pixel ^= mask;
                break;
        }
        pixel += step;
    }
}

void sgl_write_pixel_hline_8b(sgl_framebuffer_t *fb, int32_t x, int32_t y,
                              int32_t len, uint32_t format_color) {
    uint8_t *pixel = sgl_get_pixel(fb, x, y, 1, 0);
    int32_t step = (len > 0) ? 1 : -1;
    if (len < 0) {
        len = -len;
    }
    for (int32_t i = 0; i < len; ++i) {
        *(uint8_t *)pixel = (uint8_t)format_color;
        pixel += step;
    }
}

void sgl_write_pixel_hline_16b(sgl_framebuffer_t *fb, int32_t x, int32_t y,
                               int32_t len, uint32_t format_color) {
    uint8_t *pixel = sgl_get_pixel(fb, x, y, 2, 0);
    int32_t step = (len > 0) ? 2 : -2;
    if (len < 0) {
        len = -len;
    }
    for (int32_t i = 0; i < len; ++i) {
        *(uint16_t *)pixel = (uint16_t)format_color;
        pixel += step;
    }
}

void sgl_write_pixel_hline_24b(sgl_framebuffer_t *fb, int32_t x, int32_t y,
                               int32_t len, uint32_t format_color) {
    uint8_t *pixel = sgl_get_pixel(fb, x, y, 3, 0);
    int32_t step = (len > 0) ? 3 : -3;
    if (len < 0) {
        len = -len;
    }
    for (int32_t i = 0; i < len; ++i) {
        *(uint8_t *)pixel = (uint8_t)format_color;
        *((uint8_t *)pixel + 1) = (uint8_t)(format_color >> 8);
        *((uint8_t *)pixel + 2) = (uint8_t)(format_color >> 16);
        pixel += step;
    }
}

void sgl_write_pixel_hline_32b(sgl_framebuffer_t *fb, int32_t x, int32_t y,
                               int32_t len, uint32_t format_color) {
    uint8_t *pixel = sgl_get_pixel(fb, x, y, 4, 0);
    int32_t step = (len > 0) ? 4 : -4;
    if (len < 0) {
        len = -len;
    }
    for (int32_t i = 0; i < len; ++i) {
        *(uint32_t *)pixel = (uint32_t)format_color;
        pixel += step;
    }
}
