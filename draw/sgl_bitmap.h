// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#ifndef SGL_BITMAP_H
#define SGL_BITMAP_H

#include "display/sgl_display.h"

#ifdef __cplusplus
extern "C" {
#endif

void sgl_draw_bitmap_1bpp(sgl_display_t *disp, sgl_int_t x, sgl_int_t y,
                          sgl_int_t w, sgl_int_t h, const uint8_t *bitmap,
                          sgl_color_t color);
void sgl_draw_bitmap(sgl_display_t *disp, sgl_int_t x, sgl_int_t y, sgl_int_t w,
                     sgl_int_t h, const uint8_t *bitmap);

#ifdef __cplusplus
}
#endif

#endif
