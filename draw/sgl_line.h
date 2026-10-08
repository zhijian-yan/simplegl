// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#ifndef SGL_LINE_H
#define SGL_LINE_H

#include "core/sgl_types.h"
#include "display/sgl_display.h"

#ifdef __cplusplus
extern "C" {
#endif

void sgl_draw_point(sgl_display_t *disp, int32_t x, int32_t y, uint32_t color);
void sgl_draw_hline(sgl_display_t *disp, int32_t x, int32_t y, int32_t len,
                    uint32_t color);
void sgl_draw_vline(sgl_display_t *disp, int32_t x, int32_t y, int32_t len,
                    uint32_t color);
void sgl_draw_line(sgl_display_t *disp, int32_t x0, int32_t y0, int32_t x1,
                   int32_t y1, uint32_t color);

#ifdef __cplusplus
}
#endif

#endif
