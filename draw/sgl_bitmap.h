// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#ifndef SGL_BITMAP_H
#define SGL_BITMAP_H

#include "display/sgl_display.h"

#ifdef __cplusplus
extern "C" {
#endif

void sgl_draw_bitmap_1bpp(sgl_display_t *disp, int32_t x, int32_t y, int32_t w,
                          int32_t h, const uint8_t *bitmap, uint32_t color);
void sgl_draw_bitmap(sgl_display_t *disp, int32_t x, int32_t y, int32_t w,
                     int32_t h, const uint8_t *bitmap);

#ifdef __cplusplus
}
#endif

#endif
