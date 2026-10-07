// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#ifndef SGL_COMMON_H
#define SGL_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sgl_types.h"

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

static inline void sgl_set_rect(sgl_rect_t *rect, int32_t x, int32_t y,
                                int32_t w, int32_t h) {
    rect->x = x;
    rect->y = y;
    rect->w = w;
    rect->h = h;
}

static inline void sgl_set_area(sgl_area_t *area, int32_t left, int32_t top,
                                int32_t right, int32_t bottom) {
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
    area->left = rect->x;
    area->top = rect->y;
    if (rect->w > 0)
        area->right = rect->x + rect->w - 1;
    else
        area->right = rect->x - rect->w - 1;
    if (rect->h > 0)
        area->bottom = rect->y + rect->h - 1;
    else
        area->bottom = rect->y - rect->h - 1;
}

static inline int sgl_check_area(const sgl_area_t *bounds, int32_t left,
                                 int32_t top, int32_t right, int32_t bottom) {
    if (left > bounds->right || right < bounds->left || top > bounds->bottom ||
        bottom < bounds->top)
        return -1;
    return 0;
}

static inline int sgl_set_area_within(sgl_area_t *area,
                                      const sgl_area_t *bounds, int32_t left,
                                      int32_t top, int32_t right,
                                      int32_t bottom) {
    if (sgl_check_area(bounds, left, top, right, bottom))
        return -1;
    if (left < bounds->left)
        left = bounds->left;
    if (top < bounds->top)
        top = bounds->top;
    if (right > bounds->right)
        right = bounds->right;
    if (bottom > bounds->bottom)
        bottom = bounds->bottom;
    sgl_set_area(area, left, top, right, bottom);
    return 0;
}

static inline void sgl_align(int32_t *x, int32_t *y, int32_t w, int32_t h,
                             sgl_align_t align) {
    switch (align) {
        case SGL_ALIGN_UP_LEFT:
            break;
        case SGL_ALIGN_UP_RIGHT:
            *x -= w - 1;
            break;
        case SGL_ALIGN_DOWN_LEFT:
            *y -= h - 1;
            break;
        case SGL_ALIGN_DOWN_RIGHT:
            *x -= w - 1;
            *y -= h - 1;
            break;
        case SGL_ALIGN_CENTER:
            *x -= w / 2;
            *y -= h / 2;
            break;
        case SGL_ALIGN_UP_CENTER:
            *x -= w / 2;
            break;
        case SGL_ALIGN_DOWN_CENTER:
            *x -= w / 2;
            *y -= h - 1;
            break;
        case SGL_ALIGN_LEFT_CENTER:
            *y -= h / 2;
            break;
        case SGL_ALIGN_RIGHT_CENTER:
            *x -= w - 1;
            *y -= h / 2;
            break;
    }
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

static inline void sgl_draw_hpixel(sgl_display_t *disp, int32_t x, int32_t y,
                                   int32_t len, uint32_t color) {
    int32_t x1 = x + len;
    for (len = (len > 0) ? 1 : -1; x != x1; x += len)
        disp->draw_pixel(disp, x, y, color);
}

static inline void sgl_draw_vpixel(sgl_display_t *disp, int32_t x, int32_t y,
                                   int32_t len, uint32_t color) {
    int32_t y1 = y + len;
    for (len = (len > 0) ? 1 : -1; y != y1; y += len)
        disp->draw_pixel(disp, x, y, color);
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
