// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#ifndef SGL_CIRCLE_H
#define SGL_CIRCLE_H

#include "core/sgl_types.h"
#include "display/sgl_display.h"

#ifdef __cplusplus
extern "C" {
#endif

void sgl_draw_circle(sgl_display_t *disp, int32_t x, int32_t y, int32_t d,
                     int32_t is_filled, uint32_t color);
void sgl_draw_circle_center(sgl_display_t *disp, int32_t xc, int32_t yc,
                            int32_t r, int32_t is_filled, uint32_t color);
void sgl_draw_ellipse(sgl_display_t *disp, int32_t xc, int32_t yc, int32_t rx,
                      int32_t ry, int32_t is_filled, uint32_t color);

#ifdef __cplusplus
}
#endif

#endif
