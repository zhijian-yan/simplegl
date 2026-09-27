// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#ifndef SGL_RECT_H
#define SGL_RECT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sgl_types.h"

void sgl_draw_rect(sgl_screen_t *scr, int32_t x, int32_t y, int32_t w,
                   int32_t h, int is_filled, uint32_t color);
void sgl_draw_round_rect(sgl_screen_t *scr, int32_t x, int32_t y, int32_t w,
                         int32_t h, int32_t r, int is_filled, uint32_t color);

#ifdef __cplusplus
}
#endif

#endif
