// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#ifndef SGL_PIXEL_H
#define SGL_PIXEL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sgl_types.h"

void sgl_draw_pixel_mono(sgl_display_t *disp, int32_t x, int32_t y,
                         uint32_t color);
void sgl_draw_pixel_rgb332(sgl_display_t *disp, int32_t x, int32_t y,
                           uint32_t color);
void sgl_draw_pixel_rgb565(sgl_display_t *disp, int32_t x, int32_t y,
                           uint32_t color);
void sgl_draw_pixel_rgb565swap(sgl_display_t *disp, int32_t x, int32_t y,
                               uint32_t color);
void sgl_draw_pixel_bgr565(sgl_display_t *disp, int32_t x, int32_t y,
                           uint32_t color);
void sgl_draw_pixel_rgb888(sgl_display_t *disp, int32_t x, int32_t y,
                           uint32_t color);
void sgl_draw_pixel_bgr888(sgl_display_t *disp, int32_t x, int32_t y,
                           uint32_t color);
void sgl_draw_pixel_xrgb8888(sgl_display_t *disp, int32_t x, int32_t y,
                             uint32_t color);
void sgl_draw_pixel_xbgr8888(sgl_display_t *disp, int32_t x, int32_t y,
                             uint32_t color);
void sgl_draw_pixel_argb8888(sgl_display_t *disp, int32_t x, int32_t y,
                             uint32_t color);
void sgl_draw_pixel_abgr8888(sgl_display_t *disp, int32_t x, int32_t y,
                             uint32_t color);
void sgl_draw_pixel_rgba8888(sgl_display_t *disp, int32_t x, int32_t y,
                             uint32_t color);
void sgl_draw_pixel_bgra8888(sgl_display_t *disp, int32_t x, int32_t y,
                             uint32_t color);

#ifdef __cplusplus
}
#endif

#endif
