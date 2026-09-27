// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#include "../inc/sgl_rect.h"
#include "../inc/sgl_common.h"
#include "../inc/sgl_line.h"

void sgl_draw_rect(sgl_screen_t *scr, int32_t x, int32_t y, int32_t w,
                   int32_t h, int is_filled, uint32_t color) {
    if (is_filled == 1 || (h > -2 && h < 2) || (w > -2 && w < 2)) {
        sgl_logical_offset(x, y);
        if (sgl_clip_rect(&scr->drawable_area, &x, &y, &w, &h))
            return;
        sgl_rotate_rect_ccw(scr, &x, &y, &w, &h);
        sgl_normalize_rect(&x, &y, &w, &h);
        sgl_buffer_offset(x, y);
        if (w > h) {
            for (h += y; y < h; ++y) {
                sgl_draw_hpixel(scr, x, y, w, color);
            }
        } else {
            for (w += x; x < w; ++x) {
                sgl_draw_vpixel(scr, x, y, h, color);
            }
        }
    } else {
        sgl_normalize_rect(&x, &y, &w, &h);
        sgl_draw_hline(scr, x, y, w - 1, color);
        sgl_draw_vline(scr, x, y + 1, h - 1, color);
        sgl_draw_vline(scr, x + w - 1, y, h - 1, color);
        sgl_draw_hline(scr, x + 1, y + h - 1, w - 1, color);
    }
}

void sgl_draw_round_rect(sgl_screen_t *scr, int32_t x, int32_t y, int32_t w,
                         int32_t h, int32_t r, int is_filled, uint32_t color) {
    int32_t mw, mh;
    sgl_normalize_rect(&x, &y, &w, &h);
    if (w <= 2 || h <= 2) {
        if (w > h) {
            for (h += y; y < h; ++y)
                sgl_draw_hline(scr, x, y, w, color);
        } else {
            for (w += x; x < w; ++x)
                sgl_draw_vline(scr, x, y, h, color);
        }
        return;
    }
    mw = w >> 1;
    mh = h >> 1;
    if (r < 0)
        r = -r;
    if (r > mw)
        r = mw;
    if (r > mh)
        r = mh;
    mw = w - (r << 1);
    mh = h - (r << 1);
    if (is_filled == 0) {
        sgl_draw_hline(scr, x + r, y, mw, color);
        sgl_draw_hline(scr, x + r, y + h - 1, mw, color);
        sgl_draw_vline(scr, x, y + r, mh, color);
        sgl_draw_vline(scr, x + w - 1, y + r, mh, color);
        sgl_draw_circle_section(scr, x + r, y + r, r, mw - 1, mh - 1, color);
    } else {
        sgl_draw_rect(scr, x + r, y, mw, h, 1, color);
        sgl_draw_rect(scr, x, y + r, r, mh, 1, color);
        sgl_draw_rect(scr, x + w - r, y + r, r, mh, 1, color);
        sgl_draw_filled_circle_section(scr, x + r, y + r, r, mw - 1, mh - 1,
                                       color);
    }
}
