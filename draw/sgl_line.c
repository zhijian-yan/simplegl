// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#include "sgl_line.h"
#include "sgl_common.h"

void sgl_draw_point(sgl_display_t *disp, int32_t x, int32_t y, uint32_t color) {
    sgl_logical_offset(x, y);
    if (!sgl_area_intersects(&disp->clip_area, x, y, x, y))
        return;
    sgl_buffer_offset(x, y);
    disp->write_pixel(&disp->fb, x, y, color);
}

void sgl_draw_hline(sgl_display_t *disp, int32_t x, int32_t y, int32_t len,
                    uint32_t color) {
    sgl_logical_offset(x, y);
    if (y < disp->clip_area.top || y > disp->clip_area.bottom)
        return;
    sgl_normalize_line(&x, &len);
    if (sgl_clip_line(&x, &len, disp->clip_area.left, disp->clip_area.right))
        return;
    sgl_buffer_offset(x, y);
    sgl_write_hpixel(disp, x, y, len, color);
}

void sgl_draw_vline(sgl_display_t *disp, int32_t x, int32_t y, int32_t len,
                    uint32_t color) {
    sgl_logical_offset(x, y);
    if (x < disp->clip_area.left || x > disp->clip_area.right)
        return;
    sgl_normalize_line(&y, &len);
    if (sgl_clip_line(&y, &len, disp->clip_area.top, disp->clip_area.bottom))
        return;
    sgl_buffer_offset(x, y);
    sgl_write_vpixel(disp, x, y, len, color);
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
