// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#include "sgl_bitmap.h"
#include "sgl_common.h"

static void sgl_rotate_bitmap(const sgl_display_t *disp, int32_t *x, int32_t *y,
                              int32_t *dx, int32_t *dy, int32_t w, int32_t h,
                              int32_t bmp_w, int32_t bmp_h, sgl_dir_t *dir) {
    int32_t temp;
    // sgl_rotate_point_ccw(disp, x, y);
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
        if (sgl_clip_rect(&disp->clip_area, x, y, w, h))
            return -1;
    } else {
        if (sgl_clip_rect(&disp->clip_area, x, y, h, w))
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

void sgl_draw_bitmap_1bpp(sgl_display_t *disp, int32_t x, int32_t y, int32_t w,
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

void sgl_draw_bitmap(sgl_display_t *disp, int32_t x, int32_t y, int32_t w,
                     int32_t h, const uint8_t *bitmap) {
    sgl_logical_offset(x, y);
    sgl_rect_t bmp = {x, y, w, h};
    if (sgl_clip_rect(&disp->clip_area, &x, &y, &w, &h))
        return;
    sgl_framebuffer_t *fb = &disp->fb;
    int32_t pixel_size = fb->pixel_size;
    int32_t copy_size = w * pixel_size;
    int32_t src_stride = bmp.w * pixel_size;
    int32_t des_stride = fb->dirty_width * pixel_size;
    const uint8_t *src =
        bitmap + ((x - bmp.x) + (y - bmp.y) * bmp.w) * pixel_size;
    sgl_buffer_offset(x, y);
    uint8_t *des = (uint8_t *)disp->fb.buffer +
                   (x + (y >> fb->pixel_index) * fb->dirty_width) * pixel_size;
    for (int32_t i = 0; i < h; ++i) {
        memcpy(des, src, (size_t)copy_size);
        src += src_stride;
        des += des_stride;
    }
}
