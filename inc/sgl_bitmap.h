// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#ifndef SGL_BITMAP_H
#define SGL_BITMAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sgl_types.h"

void sgl_show_bitmap_1b(sgl_display_t *disp, int32_t x, int32_t y, int32_t w,
                        int32_t h, const uint8_t *bitmap, sgl_dir_t dir,
                        uint32_t color);
void sgl_show_bitmap_8b(sgl_display_t *disp, int32_t x, int32_t y, int32_t w,
                        int32_t h, const uint8_t *bitmap, sgl_dir_t dir);
void sgl_show_bitmap_16b(sgl_display_t *disp, int32_t x, int32_t y, int32_t w,
                         int32_t h, const uint16_t *bitmap, sgl_dir_t dir);
void sgl_show_bitmap_24b(sgl_display_t *disp, int32_t x, int32_t y, int32_t w,
                         int32_t h, const uint8_t *bitmap, sgl_dir_t dir);
void sgl_show_bitmap_32b(sgl_display_t *disp, int32_t x, int32_t y, int32_t w,
                         int32_t h, const uint32_t *bitmap, sgl_dir_t dir);

#ifdef __cplusplus
}
#endif

#endif
