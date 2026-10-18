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

#define SGL_COORD_MAX ((int32_t)1 << 29)

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

static inline int sgl_area_is_intersect(const sgl_area_t *bounds, int32_t left,
                                        int32_t top, int32_t right,
                                        int32_t bottom) {
    if (left > bounds->right || right < bounds->left || top > bounds->bottom ||
        bottom < bounds->top)
        return 0;
    return 1;
}

static inline int sgl_area_get_intersect(const sgl_area_t *a,
                                         const sgl_area_t *b, sgl_area_t *out) {
    int32_t left = (a->left > b->left) ? a->left : b->left;
    int32_t top = (a->top > b->top) ? a->top : b->top;
    int32_t right = (a->right < b->right) ? a->right : b->right;
    int32_t bottom = (a->bottom < b->bottom) ? a->bottom : b->bottom;
    if (left > right || top > bottom)
        return -1;
    out->left = left;
    out->top = top;
    out->right = right;
    out->bottom = bottom;
    return 0;
}

static inline int sgl_set_area_within(sgl_area_t *out, const sgl_area_t *bounds,
                                      int32_t left, int32_t top, int32_t right,
                                      int32_t bottom) {
    sgl_area_t in;
    sgl_set_area(&in, left, top, right, bottom);
    return sgl_area_get_intersect(&in, bounds, out);
}

static inline sgl_area_t sgl_area_get_union(const sgl_area_t *a,
                                            const sgl_area_t *b) {
    sgl_area_t union_area;
    union_area.left = (a->left < b->left) ? a->left : b->left;
    union_area.top = (a->top < b->top) ? a->top : b->top;
    union_area.right = (a->right > b->right) ? a->right : b->right;
    union_area.bottom = (a->bottom > b->bottom) ? a->bottom : b->bottom;
    return union_area;
}

static inline void sgl_write_hpixel(sgl_display_t *disp, int32_t x, int32_t y,
                                    int32_t len, uint32_t color) {
    disp->write_pixel_hline(&disp->fb, x, y, len, color);
}

static inline void sgl_write_vpixel(sgl_display_t *disp, int32_t x, int32_t y,
                                    int32_t len, uint32_t color) {
    int32_t y1 = y + len;
    sgl_write_pixel_t write_pixel = disp->write_pixel;
    sgl_framebuffer_t *fb = &disp->fb;
    for (len = (len > 0) ? 1 : -1; y != y1; y += len)
        write_pixel(fb, x, y, color);
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

static inline void sgl_rotate_point_ccw(const sgl_display_t *disp, int32_t *x,
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

static inline void sgl_rotate_point_cw(const sgl_display_t *disp, int32_t *x,
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

static inline void sgl_rotate_rect_ccw(const sgl_display_t *disp, int32_t *x,
                                       int32_t *y, int32_t *w, int32_t *h) {
    int32_t temp_x = *x;
    int32_t temp_y = *y;
    int32_t temp_w = *w;
    int32_t temp_h = *h;
    switch (disp->rotate) {
        case SGL_ROTATE_0:
            break;
        case SGL_ROTATE_90:
            *x = disp->max_y - temp_y - temp_h + 1;
            *y = temp_x;
            *w = temp_h;
            *h = temp_w;
            break;
        case SGL_ROTATE_180:
            *x = disp->max_x - temp_x - temp_w + 1;
            *y = disp->max_y - temp_y - temp_h + 1;
            break;
        case SGL_ROTATE_270:
            *x = temp_y;
            *y = disp->max_x - temp_x - temp_w + 1;
            *w = temp_h;
            *h = temp_w;
            break;
    }
}

static inline void sgl_rotate_rect_cw(const sgl_display_t *disp, int32_t *x,
                                      int32_t *y, int32_t *w, int32_t *h) {
    int32_t temp_x = *x;
    int32_t temp_y = *y;
    int32_t temp_w = *w;
    int32_t temp_h = *h;
    switch (disp->rotate) {
        case SGL_ROTATE_0:
            break;
        case SGL_ROTATE_90:
            *x = temp_y;
            *y = disp->max_y - temp_x - temp_w + 1;
            *w = temp_h;
            *h = temp_w;
            break;
        case SGL_ROTATE_180:
            *x = disp->max_x - temp_x - temp_w + 1;
            *y = disp->max_y - temp_y - temp_h + 1;
            break;
        case SGL_ROTATE_270:
            *x = disp->max_x - temp_y - temp_h + 1;
            *y = temp_x;
            *w = temp_h;
            *h = temp_w;
            break;
    }
}

static inline void sgl_rotate_area_ccw(const sgl_display_t *disp, int32_t *left,
                                       int32_t *top, int32_t *right,
                                       int32_t *bottom) {
    int32_t temp_left = *left;
    int32_t temp_top = *top;
    int32_t temp_right = *right;
    int32_t temp_bottom = *bottom;
    switch (disp->rotate) {
        case SGL_ROTATE_0:
            break;
        case SGL_ROTATE_90:
            *left = disp->max_y - temp_bottom;
            *top = temp_left;
            *right = disp->max_y - temp_top;
            *bottom = temp_right;
            break;
        case SGL_ROTATE_180:
            *left = disp->max_x - temp_right;
            *top = disp->max_y - temp_bottom;
            *right = disp->max_x - temp_left;
            *bottom = disp->max_y - temp_top;
            break;
        case SGL_ROTATE_270:
            *left = temp_top;
            *top = disp->max_x - temp_right;
            *right = temp_bottom;
            *bottom = disp->max_x - temp_left;
            break;
    }
}

static inline void sgl_rotate_area_cw(const sgl_display_t *disp, int32_t *left,
                                      int32_t *top, int32_t *right,
                                      int32_t *bottom) {
    int32_t temp_left = *left;
    int32_t temp_top = *top;
    int32_t temp_right = *right;
    int32_t temp_bottom = *bottom;
    switch (disp->rotate) {
        case SGL_ROTATE_0:
            break;
        case SGL_ROTATE_90:
            *left = temp_top;
            *top = disp->max_y - temp_right;
            *right = temp_bottom;
            *bottom = disp->max_y - temp_left;
            break;
        case SGL_ROTATE_180:
            *left = disp->max_x - temp_right;
            *top = disp->max_y - temp_bottom;
            *right = disp->max_x - temp_left;
            *bottom = disp->max_y - temp_top;
            break;
        case SGL_ROTATE_270:
            *left = disp->max_x - temp_bottom;
            *top = temp_left;
            *right = disp->max_x - temp_top;
            *bottom = temp_right;
            break;
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
