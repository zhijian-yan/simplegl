// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#ifndef SGL_COMMON_H
#define SGL_COMMON_H

#include "core/sgl_types.h"
#include "display/sgl_display.h"

#ifdef __cplusplus
extern "C" {
#endif

#define sgl_logical_offset(x, y)       \
    {                                  \
        (x) += disp->logical_offset_x; \
        (y) += disp->logical_offset_y; \
    }

#define sgl_buffer_offset(x, y)       \
    {                                 \
        (x) -= disp->buffer_offset_x; \
        (y) -= disp->buffer_offset_y; \
    }

static inline void sgl_write_hpixel(sgl_display_t *disp, int32_t x, int32_t y,
                                    int32_t len, uint32_t color) {
    int32_t x1 = x + len;
    for (len = (len > 0) ? 1 : -1; x != x1; x += len)
        disp->write_pixel(&disp->fb, x, y, color);
}

static inline void sgl_write_vpixel(sgl_display_t *disp, int32_t x, int32_t y,
                                    int32_t len, uint32_t color) {
    int32_t y1 = y + len;
    for (len = (len > 0) ? 1 : -1; y != y1; y += len)
        disp->write_pixel(&disp->fb, x, y, color);
}

static inline int sgl_check_area(const sgl_area_t *bounds, int32_t left,
                                 int32_t top, int32_t right, int32_t bottom) {
    if (left > bounds->right || right < bounds->left || top > bounds->bottom ||
        bottom < bounds->top)
        return -1;
    return 0;
}

static inline void sgl_normalize_line(int32_t *posi, int32_t *len) {
    if (*len < 0) {
        *posi += *len + 1;
        *len = -*len;
    }
}

static inline void sgl_normalize_rect(int32_t *x, int32_t *y, int32_t *w,
                                      int32_t *h) {
    sgl_normalize_line(x, w);
    sgl_normalize_line(y, h);
}

static inline int sgl_clip_line(int32_t *start, int32_t *len, int32_t min,
                                int32_t max) {
    int32_t end;
    if (*len > 0) {
        if (*start > max)
            return -1;
        end = *start + *len - 1;
        if (end < min)
            return -1;
        if (*start < min)
            *start = min;
        if (end > max)
            end = max;
        *len = end - *start + 1;
    } else if (*len < 0) {
        if (*start < min)
            return -1;
        end = *start + *len + 1;
        if (end > max)
            return -1;
        if (end < min)
            end = min;
        if (*start > max)
            *start = max;
        *len = end - *start - 1;
    }
    return 0;
}

static inline int sgl_clip_rect(const sgl_area_t *bounds, int32_t *x,
                                int32_t *y, int32_t *w, int32_t *h) {
    if (sgl_clip_line(x, w, bounds->left, bounds->right))
        return -1;
    if (sgl_clip_line(y, h, bounds->top, bounds->bottom))
        return -1;
    return 0;
}

static inline void sgl_rotate_point_ccw(sgl_display_t *disp, int32_t *x,
                                        int32_t *y) {
    int32_t temp = *x;
    switch (disp->rotate) {
        case SGL_ROTATE_0:
            break;
        case SGL_ROTATE_90:
            *x = disp->max_y - *y;
            *y = temp;
            break;
        case SGL_ROTATE_180:
            *x = disp->max_x - *x;
            *y = disp->max_y - *y;
            break;
        case SGL_ROTATE_270:
            *x = *y;
            *y = disp->max_x - temp;
            break;
    }
}

static inline void sgl_rotate_point_cw(sgl_display_t *disp, int32_t *x,
                                       int32_t *y) {
    int32_t temp = *x;
    switch (disp->rotate) {
        case SGL_ROTATE_0:
            break;
        case SGL_ROTATE_90:
            *x = *y;
            *y = disp->max_y - temp;
            break;
        case SGL_ROTATE_180:
            *x = disp->max_x - *x;
            *y = disp->max_y - *y;
            break;
        case SGL_ROTATE_270:
            *x = disp->max_x - *y;
            *y = temp;
            break;
    }
}

static inline void sgl_rotate_rect_ccw(sgl_display_t *disp, int32_t *x,
                                       int32_t *y, int32_t *w, int32_t *h) {
    int32_t temp1 = *x;
    int32_t temp2 = *w;
    switch (disp->rotate) {
        case SGL_ROTATE_0:
            break;
        case SGL_ROTATE_90:
            *x = disp->max_y - *y;
            *y = temp1;
            *w = -*h;
            *h = temp2;
            break;
        case SGL_ROTATE_180:
            *x = disp->max_x - *x;
            *y = disp->max_y - *y;
            *w = -*w;
            *h = -*h;
            break;
        case SGL_ROTATE_270:
            *x = *y;
            *y = disp->max_x - temp1;
            *w = *h;
            *h = -temp2;
            break;
    }
}

static inline void sgl_rotate_rect_cw(sgl_display_t *disp, int32_t *x,
                                      int32_t *y, int32_t *w, int32_t *h) {
    int32_t temp1 = *x;
    int32_t temp2 = *w;
    switch (disp->rotate) {
        case SGL_ROTATE_0:
            break;
        case SGL_ROTATE_90:
            *x = *y;
            *y = disp->max_y - temp1;
            *w = *h;
            *h = -temp2;
            break;
        case SGL_ROTATE_180:
            *x = disp->max_x - *x;
            *y = disp->max_y - *y;
            *w = -*w;
            *h = -*h;
            break;
        case SGL_ROTATE_270:
            *x = disp->max_x - *y;
            *y = temp1;
            *w = -*h;
            *h = temp2;
            break;
    }
}

static inline void sgl_rotate_area_ccw(sgl_display_t *disp, int32_t *left,
                                       int32_t *top, int32_t *right,
                                       int32_t *bottom) {
    int32_t temp;
    sgl_rotate_point_ccw(disp, left, top);
    sgl_rotate_point_ccw(disp, right, bottom);
    if (*left > *right) {
        temp = *left;
        *left = *right;
        *right = temp;
    }
    if (*top > *bottom) {
        temp = *top;
        *top = *bottom;
        *bottom = temp;
    }
}

static inline void sgl_rotate_area_cw(sgl_display_t *disp, int32_t *left,
                                      int32_t *top, int32_t *right,
                                      int32_t *bottom) {
    int32_t temp;
    sgl_rotate_point_cw(disp, left, top);
    sgl_rotate_point_cw(disp, right, bottom);
    if (*left > *right) {
        temp = *left;
        *left = *right;
        *right = temp;
    }
    if (*top > *bottom) {
        temp = *top;
        *top = *bottom;
        *bottom = temp;
    }
}

void sgl_draw_circle_section(sgl_display_t *disp, int32_t xc, int32_t yc,
                             int32_t r, int32_t offset_x, int32_t offset_y,
                             uint32_t color);
void sgl_draw_filled_circle_section(sgl_display_t *disp, int32_t xc, int32_t yc,
                                    int32_t r, int32_t offset_x,
                                    int32_t offset_y, uint32_t color);
void sgl_draw_ellipse_section(sgl_display_t *disp, int32_t xc, int32_t yc,
                              int32_t rx, int32_t ry, uint32_t color);
void sgl_draw_filled_ellipse_section(sgl_display_t *disp, int32_t xc,
                                     int32_t yc, int32_t rx, int32_t ry,
                                     uint32_t color);

#ifdef __cplusplus
}
#endif

#endif
