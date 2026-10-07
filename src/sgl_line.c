// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#include "../inc/sgl_line.h"
#include "../inc/sgl_common.h"

void sgl_draw_point(sgl_display_t *disp, int32_t x, int32_t y, uint32_t color) {
    sgl_logical_offset(x, y);
    if (sgl_check_area(&disp->drawable_area, x, y, x, y))
        return;
    sgl_rotate_point_ccw(disp, &x, &y);
    sgl_buffer_offset(x, y);
    disp->draw_pixel(disp, x, y, color);
}

void sgl_draw_hline(sgl_display_t *disp, int32_t x, int32_t y, int32_t len,
                    uint32_t color) {
    sgl_logical_offset(x, y);
    if (y < disp->drawable_area.top || y > disp->drawable_area.bottom)
        return;
    if (sgl_clip_line(&x, &len, disp->drawable_area.left,
                      disp->drawable_area.right))
        return;
    sgl_rotate_point_ccw(disp, &x, &y);
    sgl_buffer_offset(x, y);
    switch (disp->rotate) {
        case SGL_ROTATE_0:
            sgl_draw_hpixel(disp, x, y, len, color);
            break;
        case SGL_ROTATE_90:
            sgl_draw_vpixel(disp, x, y, len, color);
            break;
        case SGL_ROTATE_180:
            sgl_draw_hpixel(disp, x, y, -len, color);
            break;
        case SGL_ROTATE_270:
            sgl_draw_vpixel(disp, x, y, -len, color);
            break;
    }
}

void sgl_draw_vline(sgl_display_t *disp, int32_t x, int32_t y, int32_t len,
                    uint32_t color) {
    sgl_logical_offset(x, y);
    if (x < disp->drawable_area.left || x > disp->drawable_area.right)
        return;
    if (sgl_clip_line(&y, &len, disp->drawable_area.top,
                      disp->drawable_area.bottom))
        return;
    sgl_rotate_point_ccw(disp, &x, &y);
    sgl_buffer_offset(x, y);
    switch (disp->rotate) {
        case SGL_ROTATE_0:
            sgl_draw_vpixel(disp, x, y, len, color);
            break;
        case SGL_ROTATE_90:
            sgl_draw_hpixel(disp, x, y, -len, color);
            break;
        case SGL_ROTATE_180:
            sgl_draw_vpixel(disp, x, y, -len, color);
            break;
        case SGL_ROTATE_270:
            sgl_draw_hpixel(disp, x, y, len, color);
            break;
    }
}

void sgl_draw_line(sgl_display_t *disp, int32_t x0, int32_t y0, int32_t x1,
                   int32_t y1, uint32_t color) {
    int32_t dx, dy, sx, sy, err;
    dx = x1 - x0;
    dy = y1 - y0;
    sx = 1, sy = 1;
    if (dx < 0) {
        dx = -dx;
        sx = -1;
    }
    if (dy < 0) {
        dy = -dy;
        sy = -1;
    }
    if (dx > dy) {
        for (err = dx >> 1; x0 != x1; x0 += sx) {
            sgl_draw_point(disp, x0, y0, color);
            err -= dy;
            if (err < 0) {
                y0 += sy;
                err += dx;
            }
        }
    } else {
        for (err = dy >> 1; y0 != y1; y0 += sy) {
            sgl_draw_point(disp, x0, y0, color);
            err -= dx;
            if (err < 0) {
                x0 += sx;
                err += dy;
            }
        }
    }
    sgl_draw_point(disp, x1, y1, color);
}
