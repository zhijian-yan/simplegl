// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#ifndef SGL_COMMON_H
#define SGL_COMMON_H

#include "display/sgl_display.h"

#ifdef __cplusplus
extern "C" {
#endif

#define sgl_logical_offset(x, y) \
    {                            \
        (x) += disp->logical_x;  \
        (y) += disp->logical_y;  \
    }

#define sgl_buffer_offset(x, y) \
    {                           \
        (x) -= disp->buffer_x;  \
        (y) -= disp->buffer_y;  \
    }

#define SGL_COORD_MAX ((sgl_int_t)1 << 29)

static inline void sgl_set_rect(sgl_rect_t *rect, sgl_int_t x, sgl_int_t y,
                                sgl_int_t w, sgl_int_t h) {
    rect->x = x;
    rect->y = y;
    rect->w = w;
    rect->h = h;
}

static inline void sgl_set_area(sgl_area_t *area, sgl_int_t left, sgl_int_t top,
                                sgl_int_t right, sgl_int_t bottom) {
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
    if (rect->w >= 0) {
        area->left = rect->x;
        area->right = rect->x + rect->w - 1;
    } else {
        area->left = rect->x + rect->w + 1;
        area->right = rect->x;
    }
    if (rect->h >= 0) {
        area->top = rect->y;
        area->bottom = rect->y + rect->h - 1;
    } else {
        area->top = rect->y + rect->h + 1;
        area->bottom = rect->y;
    }
}

static inline int sgl_area_intersects(const sgl_area_t *bounds, sgl_int_t left,
                                      sgl_int_t top, sgl_int_t right,
                                      sgl_int_t bottom) {
    if (left > bounds->right || top > bounds->bottom || right < bounds->left ||
        bottom < bounds->top)
        return 0;
    return 1;
}

static inline int sgl_area_contains(const sgl_area_t *bounds, sgl_int_t left,
                                    sgl_int_t top, sgl_int_t right,
                                    sgl_int_t bottom) {
    if (left > bounds->left && top > bounds->top && right < bounds->right &&
        bottom < bounds->bottom)
        return 0;
    return 1;
}

static inline void sgl_normalize_line(sgl_int_t *posi, sgl_int_t *len) {
    if (*len < 0) {
        *posi += *len + 1;
        *len = -*len;
    }
}

static inline void sgl_normalize_rect(sgl_int_t *x, sgl_int_t *y, sgl_int_t *w,
                                      sgl_int_t *h) {
    sgl_normalize_line(x, w);
    sgl_normalize_line(y, h);
}

static inline int sgl_clip_line(sgl_int_t *start, sgl_int_t *len, sgl_int_t min,
                                sgl_int_t max) {
    if (*len <= 0)
        return -1;
    if (*start > max)
        return -1;
    sgl_int_t end = *start + *len - 1;
    if (end < min)
        return -1;
    if (*start < min)
        *start = min;
    if (end > max)
        end = max;
    *len = end - *start + 1;
    return 0;
}

static inline int sgl_clip_rect(const sgl_area_t *bounds, sgl_int_t *x,
                                sgl_int_t *y, sgl_int_t *w, sgl_int_t *h) {
    if (sgl_clip_line(x, w, bounds->left, bounds->right))
        return -1;
    if (sgl_clip_line(y, h, bounds->top, bounds->bottom))
        return -1;
    return 0;
}

static inline void sgl_write_hpixel(sgl_display_t *disp, sgl_int_t x,
                                    sgl_int_t y, sgl_int_t len,
                                    sgl_color_t color) {
    disp->write_pixel_hline(&disp->fb, x, y, len, color);
}

static inline void sgl_write_vpixel(sgl_display_t *disp, sgl_int_t x,
                                    sgl_int_t y, sgl_int_t len,
                                    sgl_color_t color) {
    sgl_int_t y1 = y + len;
    sgl_write_pixel_t write_pixel = disp->write_pixel;
    sgl_framebuffer_t *fb = &disp->fb;
    for (len = (len > 0) ? 1 : -1; y != y1; y += len)
        write_pixel(fb, x, y, color);
}

static inline void sgl_write_rect(sgl_display_t *disp, sgl_int_t x, sgl_int_t y,
                                  sgl_int_t w, sgl_int_t h, sgl_color_t color) {
    sgl_framebuffer_t *fb = &disp->fb;
    if (fb->pixel_shift) {
        for (sgl_int_t i = 0; i < h; ++i)
            sgl_write_hpixel(disp, x, y + i, w, color);
        return;
    }
    sgl_write_hpixel(disp, x, y, w, color);
    uint8_t *src =
        (uint8_t *)fb->buffer +
        (x + (y >> fb->pixel_shift) * fb->slice_width) * fb->pixel_size;
    uint8_t *des = src;
    size_t line_size = w * fb->pixel_size;
    size_t line_stride = fb->slice_width * fb->pixel_size;
    for (sgl_int_t i = 1; i < h; ++i) {
        des += line_stride;
        memcpy(des, src, line_size);
    }
}

#ifdef __cplusplus
}
#endif

#endif
