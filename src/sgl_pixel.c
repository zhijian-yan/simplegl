// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#include "../inc/sgl_pixel.h"
#include "../inc/sgl_common.h"

void sgl_draw_pixel_mono(sgl_screen_t *scr, int32_t x, int32_t y,
                         uint32_t color) {
    uint8_t mask = 1U << (y & 7);
    switch (color) {
        case SGL_MONO_BLACK:
            ((uint8_t *)scr->buffer)[(y >> 3) * scr->buffer_width + x] &= ~mask;
            break;
        case SGL_MONO_WHITE:
            ((uint8_t *)scr->buffer)[(y >> 3) * scr->buffer_width + x] |= mask;
            break;
        case SGL_MONO_INVERT:
            ((uint8_t *)scr->buffer)[(y >> 3) * scr->buffer_width + x] ^= mask;
            break;
    }
}

void sgl_draw_pixel_rgb332(sgl_screen_t *scr, int32_t x, int32_t y,
                           uint32_t color) {
    uint8_t r, g, b;
    r = (color >> 16) & 0xFF;
    g = (color >> 8) & 0xFF;
    b = color & 0xFF;
    ((uint8_t *)scr->buffer)[x + y * scr->buffer_width] =
        (r & 0xE0) | ((g >> 3) & 0x1C) | (b >> 6);
}

void sgl_draw_pixel_rgb565(sgl_screen_t *scr, int32_t x, int32_t y,
                           uint32_t color) {
    ((uint16_t *)scr->buffer)[x + y * scr->buffer_width] = (uint16_t)color;
}

void sgl_draw_pixel_rgb565swap(sgl_screen_t *scr, int32_t x, int32_t y,
                               uint32_t color) {
    uint16_t c = (uint16_t)color;
    c = (uint16_t)((c << 8) | (c >> 8));
    ((uint16_t *)scr->buffer)[x + y * scr->buffer_width] = c;
}

void sgl_draw_pixel_bgr565(sgl_screen_t *scr, int32_t x, int32_t y,
                           uint32_t color) {
    uint8_t r, g, b;
    uint16_t c;
    r = (color >> 16) & 0xFF;
    g = (color >> 8) & 0xFF;
    b = color & 0xFF;
    c = ((uint16_t)(b >> 3) << 11) | ((uint16_t)(g >> 2) << 5) |
        (uint16_t)(r >> 3);
    ((uint16_t *)scr->buffer)[x + y * scr->buffer_width] = c;
}

void sgl_draw_pixel_rgb888(sgl_screen_t *scr, int32_t x, int32_t y,
                           uint32_t color) {
    uint8_t *pixel = (uint8_t *)scr->buffer + (x + y * scr->buffer_width) * 3;
    pixel[0] = (color >> 16) & 0xFF;
    pixel[1] = (color >> 8) & 0xFF;
    pixel[2] = color & 0xFF;
}

void sgl_draw_pixel_bgr888(sgl_screen_t *scr, int32_t x, int32_t y,
                           uint32_t color) {
    uint8_t *pixel = (uint8_t *)scr->buffer + (x + y * scr->buffer_width) * 3;
    pixel[0] = color & 0xFF;
    pixel[1] = (color >> 8) & 0xFF;
    pixel[2] = (color >> 16) & 0xFF;
}

void sgl_draw_pixel_xrgb8888(sgl_screen_t *scr, int32_t x, int32_t y,
                             uint32_t color) {
    ((uint32_t *)scr->buffer)[x + y * scr->buffer_width] =
        (color & 0x00FFFFFF) | 0xFF000000;
}

void sgl_draw_pixel_xbgr8888(sgl_screen_t *scr, int32_t x, int32_t y,
                             uint32_t color) {
    uint8_t *pixel = (uint8_t *)scr->buffer + (x + y * scr->buffer_width) * 4;
    pixel[0] = 0xFF;
    pixel[1] = color & 0xFF;
    pixel[2] = (color >> 8) & 0xFF;
    pixel[3] = (color >> 16) & 0xFF;
}

void sgl_draw_pixel_argb8888(sgl_screen_t *scr, int32_t x, int32_t y,
                             uint32_t color) {
    ((uint32_t *)scr->buffer)[x + y * scr->buffer_width] = color;
}

void sgl_draw_pixel_abgr8888(sgl_screen_t *scr, int32_t x, int32_t y,
                             uint32_t color) {
    uint8_t *pixel = (uint8_t *)scr->buffer + (x + y * scr->buffer_width) * 4;
    pixel[0] = (color >> 24) & 0xFF;
    pixel[1] = color & 0xFF;
    pixel[2] = (color >> 8) & 0xFF;
    pixel[3] = (color >> 16) & 0xFF;
}

void sgl_draw_pixel_rgba8888(sgl_screen_t *scr, int32_t x, int32_t y,
                             uint32_t color) {
    uint8_t *pixel = (uint8_t *)scr->buffer + (x + y * scr->buffer_width) * 4;
    pixel[0] = (color >> 16) & 0xFF;
    pixel[1] = (color >> 8) & 0xFF;
    pixel[2] = color & 0xFF;
    pixel[3] = (color >> 24) & 0xFF;
}

void sgl_draw_pixel_bgra8888(sgl_screen_t *scr, int32_t x, int32_t y,
                             uint32_t color) {
    uint8_t *pixel = (uint8_t *)scr->buffer + (x + y * scr->buffer_width) * 4;
    pixel[0] = color & 0xFF;
    pixel[1] = (color >> 8) & 0xFF;
    pixel[2] = (color >> 16) & 0xFF;
    pixel[3] = (color >> 24) & 0xFF;
}
