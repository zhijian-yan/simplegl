// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#include "sgl_bitmap.h"
#include "sgl_common.h"
#include "sgl_line.h"

static inline uint32_t sgl_bitmap_24b_value(const uint8_t *pixel) {
    return (uint32_t)pixel[0] | ((uint32_t)pixel[1] << 8) |
           ((uint32_t)pixel[2] << 16);
}

static void sgl_rotate_bitmap(const sgl_display_t *disp, int32_t *x, int32_t *y,
                              int32_t *dx, int32_t *dy, int32_t w, int32_t h,
                              int32_t bmp_w, int32_t bmp_h, sgl_dir_t *dir) {
    int32_t temp;
    sgl_rotate_point_ccw(disp, x, y);
    temp = *dx;
    switch (disp->rotate) {
        case SGL_ROTATE_0:
            break;
        case SGL_ROTATE_90:
            if (*dir == SGL_DIR_UP || *dir == SGL_DIR_DOWN) {
                *x -= h - 1;
                *dx = bmp_h - *dy - h;
                *dy = temp;
            } else {
                *x -= w - 1;
                *dx = bmp_w - *dy - w;
                *dy = temp;
            }
            break;
        case SGL_ROTATE_180:
            if (*dir == SGL_DIR_UP || *dir == SGL_DIR_DOWN) {
                *x -= w - 1;
                *y -= h - 1;
                *dx = bmp_w - *dx - w;
                *dy = bmp_h - *dy - h;
            } else {
                *x -= h - 1;
                *y -= w - 1;
                *dx = bmp_h - *dx - h;
                *dy = bmp_w - *dy - w;
            }
            break;
        case SGL_ROTATE_270:
            if (*dir == SGL_DIR_UP || *dir == SGL_DIR_DOWN) {
                *y -= w - 1;
                *dx = *dy;
                *dy = bmp_w - temp - w;
            } else {
                *y -= h - 1;
                *dx = *dy;
                *dy = bmp_h - temp - h;
            }
            break;
    }
    *dir = (sgl_dir_t)((*dir + disp->rotate) & 3);
}

static int sgl_bitmap_prepare(const sgl_display_t *disp, sgl_dir_t dir,
                              int32_t *x, int32_t *y, int32_t *w, int32_t *h,
                              int32_t *row, int32_t *col, int32_t *row_dir,
                              int32_t *col_dir, int32_t *walk_x) {
    int32_t bmp_w = *w, bmp_h = *h;
    int32_t logical_x, logical_y, dx, dy;
    sgl_logical_offset(*x, *y);
    logical_x = *x;
    logical_y = *y;
    if (dir == SGL_DIR_UP || dir == SGL_DIR_DOWN) {
        if (sgl_clip_rect(&disp->drawable_area, x, y, w, h))
            return -1;
    } else {
        if (sgl_clip_rect(&disp->drawable_area, x, y, h, w))
            return -1;
    }
    dx = *x - logical_x;
    dy = *y - logical_y;
    sgl_rotate_bitmap(disp, x, y, &dx, &dy, *w, *h, bmp_w, bmp_h, &dir);
    sgl_buffer_offset(*x, *y);
    switch (dir) {
        case SGL_DIR_UP:
            *row = dy;
            *col = dx;
            *row_dir = 1;
            *col_dir = 1;
            *walk_x = 1;
            break;
        case SGL_DIR_RIGHT:
            *row = (bmp_h - 1) - dx;
            *col = dy;
            *row_dir = -1;
            *col_dir = 1;
            *walk_x = 0;
            break;
        case SGL_DIR_LEFT:
            *row = dx;
            *col = (bmp_w - 1) - dy;
            *row_dir = 1;
            *col_dir = -1;
            *walk_x = 0;
            break;
        case SGL_DIR_DOWN:
            *row = (bmp_h - 1) - dy;
            *col = (bmp_w - 1) - dx;
            *row_dir = -1;
            *col_dir = -1;
            *walk_x = 1;
            break;
    }
    return 0;
}

void sgl_draw_bitmap_1b(sgl_display_t *disp, int32_t x, int32_t y, int32_t w,
                        int32_t h, const uint8_t *bitmap, sgl_dir_t dir,
                        uint32_t color) {
    int32_t row, col, row_dir, col_dir, walk_x, index, i, j, bmp_w = w;
    uint32_t mask;
    sgl_write_pixel_t write_pixel = disp->write_pixel;
    sgl_framebuffer_t *fb = &disp->fb;
    if (sgl_bitmap_prepare(disp, dir, &x, &y, &w, &h, &row, &col, &row_dir,
                           &col_dir, &walk_x))
        return;
    if (walk_x) {
        for (i = 0; i < h; ++i) {
            index = (row >> 3) * bmp_w + col;
            mask = 1U << (row & 7);
            for (j = 0; j < w; ++j, index += col_dir) {
                if (bitmap[index] & mask)
                    write_pixel(fb, x + j, y, color);
            }
            row += row_dir;
            ++y;
        }
    } else {
        for (i = 0; i < h; ++i) {
            index = (row >> 3) * bmp_w + col;
            mask = 1U << (row & 7);
            for (j = 0; j < w; ++j, index += col_dir) {
                if (bitmap[index] & mask)
                    write_pixel(fb, x, y + j, color);
            }
            row += row_dir;
            ++x;
        }
    }
}

void sgl_draw_bitmap_8b(sgl_display_t *disp, int32_t x, int32_t y, int32_t w,
                        int32_t h, const uint8_t *bitmap, sgl_dir_t dir) {
    const uint8_t *src;
    int32_t row, col, row_dir, col_dir, walk_x, src_step, src_row, i, j;
    int32_t bmp_w = w;
    sgl_write_pixel_t write_pixel = disp->write_pixel;
    sgl_framebuffer_t *fb = &disp->fb;
    if (sgl_bitmap_prepare(disp, dir, &x, &y, &w, &h, &row, &col, &row_dir,
                           &col_dir, &walk_x))
        return;
    src = bitmap + row * bmp_w + col;
    src_step = col_dir;
    src_row = row_dir * bmp_w;
    if (walk_x) {
        for (i = 0; i < h; ++i) {
            const uint8_t *p = src;
            for (j = 0; j < w; ++j, p += src_step) {
                write_pixel(fb, x + j, y, *p);
            }
            src += src_row;
            ++y;
        }
    } else {
        for (i = 0; i < h; ++i) {
            const uint8_t *p = src;
            for (j = 0; j < w; ++j, p += src_step) {
                write_pixel(fb, x, y + j, *p);
            }
            src += src_row;
            ++x;
        }
    }
}

void sgl_draw_bitmap_16b(sgl_display_t *disp, int32_t x, int32_t y, int32_t w,
                         int32_t h, const uint16_t *bitmap, sgl_dir_t dir) {
    const uint8_t *src;
    int32_t row, col, row_dir, col_dir, walk_x, src_step, src_row, i, j;
    int32_t bmp_w = w;
    sgl_write_pixel_t write_pixel = disp->write_pixel;
    sgl_framebuffer_t *fb = &disp->fb;
    if (sgl_bitmap_prepare(disp, dir, &x, &y, &w, &h, &row, &col, &row_dir,
                           &col_dir, &walk_x))
        return;
    src = (const uint8_t *)(bitmap + row * bmp_w + col);
    src_step = col_dir * 2;
    src_row = row_dir * bmp_w * 2;
    if (walk_x) {
        for (i = 0; i < h; ++i) {
            const uint8_t *p = src;
            for (j = 0; j < w; ++j, p += src_step) {
                write_pixel(fb, x + j, y, *(const uint16_t *)p);
            }
            src += src_row;
            ++y;
        }
    } else {
        for (i = 0; i < h; ++i) {
            const uint8_t *p = src;
            for (j = 0; j < w; ++j, p += src_step) {
                write_pixel(fb, x, y + j, *(const uint16_t *)p);
            }
            src += src_row;
            ++x;
        }
    }
}

void sgl_draw_bitmap_24b(sgl_display_t *disp, int32_t x, int32_t y, int32_t w,
                         int32_t h, const uint8_t *bitmap, sgl_dir_t dir) {
    const uint8_t *src;
    int32_t row, col, row_dir, col_dir, walk_x, src_step, src_row, i, j;
    int32_t bmp_w = w;
    sgl_write_pixel_t write_pixel = disp->write_pixel;
    sgl_framebuffer_t *fb = &disp->fb;
    if (sgl_bitmap_prepare(disp, dir, &x, &y, &w, &h, &row, &col, &row_dir,
                           &col_dir, &walk_x))
        return;
    src = bitmap + (row * bmp_w + col) * 3;
    src_step = col_dir * 3;
    src_row = row_dir * bmp_w * 3;
    if (walk_x) {
        for (i = 0; i < h; ++i) {
            const uint8_t *p = src;
            for (j = 0; j < w; ++j, p += src_step) {
                write_pixel(fb, x + j, y, sgl_bitmap_24b_value(p));
            }
            src += src_row;
            ++y;
        }
    } else {
        for (i = 0; i < h; ++i) {
            const uint8_t *p = src;
            for (j = 0; j < w; ++j, p += src_step) {
                write_pixel(fb, x, y + j, sgl_bitmap_24b_value(p));
            }
            src += src_row;
            ++x;
        }
    }
}

void sgl_draw_bitmap_32b(sgl_display_t *disp, int32_t x, int32_t y, int32_t w,
                         int32_t h, const uint32_t *bitmap, sgl_dir_t dir) {
    const uint8_t *src;
    int32_t row, col, row_dir, col_dir, walk_x, src_step, src_row, i, j;
    int32_t bmp_w = w;
    sgl_write_pixel_t write_pixel = disp->write_pixel;
    sgl_framebuffer_t *fb = &disp->fb;
    if (sgl_bitmap_prepare(disp, dir, &x, &y, &w, &h, &row, &col, &row_dir,
                           &col_dir, &walk_x))
        return;
    src = (const uint8_t *)(bitmap + row * bmp_w + col);
    src_step = col_dir * 4;
    src_row = row_dir * bmp_w * 4;
    if (walk_x) {
        for (i = 0; i < h; ++i) {
            const uint8_t *p = src;
            for (j = 0; j < w; ++j, p += src_step) {
                write_pixel(fb, x + j, y, *(const uint32_t *)p);
            }
            src += src_row;
            ++y;
        }
    } else {
        for (i = 0; i < h; ++i) {
            const uint8_t *p = src;
            for (j = 0; j < w; ++j, p += src_step) {
                write_pixel(fb, x, y + j, *(const uint32_t *)p);
            }
            src += src_row;
            ++x;
        }
    }
}
