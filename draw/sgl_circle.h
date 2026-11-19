// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#ifndef SGL_CIRCLE_H
#define SGL_CIRCLE_H

#include "display/sgl_display.h"

#ifdef __cplusplus
extern "C" {
#endif

void sgl_draw_circle(sgl_display_t *disp, sgl_int_t xc, sgl_int_t yc,
                     sgl_int_t r, sgl_int_t is_filled, sgl_color_t color);
void sgl_draw_circle_with_diameter(sgl_display_t *disp, sgl_int_t x,
                                   sgl_int_t y, sgl_int_t d,
                                   sgl_int_t is_filled, sgl_color_t color);
void sgl_draw_ellipse(sgl_display_t *disp, sgl_int_t xc, sgl_int_t yc,
                      sgl_int_t rx, sgl_int_t ry, sgl_int_t is_filled,
                      sgl_color_t color);

#ifdef __cplusplus
}
#endif

#endif
