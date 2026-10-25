// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#include "sgl_rect.h"
#include "sgl_common.h"
#include "sgl_line.h"

void sgl_draw_circle_section(sgl_display_t *disp, int32_t xc, int32_t yc,
                             int32_t r, int32_t offset_x, int32_t offset_y,
                             uint32_t color);
void sgl_draw_filled_circle_section(sgl_display_t *disp, int32_t xc, int32_t yc,
                                    int32_t r, int32_t offset_x,
                                    int32_t offset_y, uint32_t color);

void sgl_draw_rect(sgl_display_t *disp, int32_t x, int32_t y, int32_t w,
                   int32_t h, int is_filled, uint32_t color) {
    sgl_check_param(disp);
    ;
    sgl_normalize_rect(&x, &y, &w, &h);
    if (is_filled == 1 || h < 2 || w < 2) {
        sgl_logical_offset(x, y);
        if (sgl_clip_rect(&disp->clip_area, &x, &y, &w, &h))
            return;
        sgl_buffer_offset(x, y);
        sgl_write_rect(disp, x, y, w, h, color);
    } else {
        sgl_draw_hline(disp, x, y, w - 1, color);
        sgl_draw_vline(disp, x, y + 1, h - 1, color);
        sgl_draw_vline(disp, x + w - 1, y, h - 1, color);
        sgl_draw_hline(disp, x + 1, y + h - 1, w - 1, color);
    }
}

void sgl_draw_round_rect(sgl_display_t *disp, int32_t x, int32_t y, int32_t w,
                         int32_t h, int32_t r, int is_filled, uint32_t color) {
    sgl_check_param(disp);
    sgl_normalize_rect(&x, &y, &w, &h);
    int32_t iw, ih;
    if (w <= 2 || h <= 2) {
        if (w > h) {
            for (h += y; y < h; ++y)
                sgl_draw_hline(disp, x, y, w, color);
        } else {
            for (w += x; x < w; ++x)
                sgl_draw_vline(disp, x, y, h, color);
        }
        return;
    }
    iw = w >> 1;
    ih = h >> 1;
    if (r < 0)
        r = -r;
    if (r > iw)
        r = iw;
    if (r > ih)
        r = ih;
    iw = w - (r << 1);
    ih = h - (r << 1);
    if (is_filled == 0) {
        sgl_draw_hline(disp, x + r, y, iw, color);
        sgl_draw_hline(disp, x + r, y + h - 1, iw, color);
        sgl_draw_vline(disp, x, y + r, ih, color);
        sgl_draw_vline(disp, x + w - 1, y + r, ih, color);
        sgl_draw_circle_section(disp, x + r, y + r, r, iw - 1, ih - 1, color);
    } else {
        sgl_draw_rect(disp, x + r, y, iw, h, 1, color);
        sgl_draw_rect(disp, x, y + r, r, ih, 1, color);
        sgl_draw_rect(disp, x + w - r, y + r, r, ih, 1, color);
        sgl_draw_filled_circle_section(disp, x + r, y + r, r, iw - 1, ih - 1,
                                       color);
    }
}
