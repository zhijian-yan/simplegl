// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#ifndef SGL_PIXEL_H
#define SGL_PIXEL_H

#include "core/sgl_types.h"
#include "sgl_display.h"

#ifdef __cplusplus
extern "C" {
#endif

void sgl_write_pixel_mono(sgl_framebuffer_t *fb, int32_t x, int32_t y,
                          uint32_t color);
void sgl_write_pixel_rgb332(sgl_framebuffer_t *fb, int32_t x, int32_t y,
                            uint32_t color);
void sgl_write_pixel_rgb565(sgl_framebuffer_t *fb, int32_t x, int32_t y,
                            uint32_t color);
void sgl_write_pixel_rgb565swap(sgl_framebuffer_t *fb, int32_t x, int32_t y,
                                uint32_t color);
void sgl_write_pixel_bgr565(sgl_framebuffer_t *fb, int32_t x, int32_t y,
                            uint32_t color);
void sgl_write_pixel_rgb888(sgl_framebuffer_t *fb, int32_t x, int32_t y,
                            uint32_t color);
void sgl_write_pixel_bgr888(sgl_framebuffer_t *fb, int32_t x, int32_t y,
                            uint32_t color);
void sgl_write_pixel_xrgb8888(sgl_framebuffer_t *fb, int32_t x, int32_t y,
                              uint32_t color);
void sgl_write_pixel_xbgr8888(sgl_framebuffer_t *fb, int32_t x, int32_t y,
                              uint32_t color);
void sgl_write_pixel_argb8888(sgl_framebuffer_t *fb, int32_t x, int32_t y,
                              uint32_t color);
void sgl_write_pixel_abgr8888(sgl_framebuffer_t *fb, int32_t x, int32_t y,
                              uint32_t color);
void sgl_write_pixel_rgba8888(sgl_framebuffer_t *fb, int32_t x, int32_t y,
                              uint32_t color);
void sgl_write_pixel_bgra8888(sgl_framebuffer_t *fb, int32_t x, int32_t y,
                              uint32_t color);

#ifdef __cplusplus
}
#endif

#endif
