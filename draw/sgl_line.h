// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#ifndef SGL_LINE_H
#define SGL_LINE_H

#include "display/sgl_display.h"

#ifdef __cplusplus
extern "C" {
#endif

void sgl_draw_point(sgl_display_t *disp, sgl_int_t x, sgl_int_t y,
                    sgl_color_t color);
void sgl_draw_hline(sgl_display_t *disp, sgl_int_t x, sgl_int_t y,
                    sgl_int_t len, sgl_color_t color);
void sgl_draw_vline(sgl_display_t *disp, sgl_int_t x, sgl_int_t y,
                    sgl_int_t len, sgl_color_t color);
void sgl_draw_line(sgl_display_t *disp, sgl_int_t x0, sgl_int_t y0,
                   sgl_int_t x1, sgl_int_t y1, sgl_color_t color);

#ifdef __cplusplus
}
#endif

#endif
