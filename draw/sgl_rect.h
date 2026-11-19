// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#ifndef SGL_RECT_H
#define SGL_RECT_H

#include "display/sgl_display.h"

#ifdef __cplusplus
extern "C" {
#endif

void sgl_draw_rect(sgl_display_t *disp, sgl_int_t x, sgl_int_t y, sgl_int_t w,
                   sgl_int_t h, int is_filled, sgl_color_t color);
void sgl_draw_round_rect(sgl_display_t *disp, sgl_int_t x, sgl_int_t y,
                         sgl_int_t w, sgl_int_t h, sgl_int_t r, int is_filled,
                         sgl_color_t color);

#ifdef __cplusplus
}
#endif

#endif
