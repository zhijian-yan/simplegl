// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#ifndef SGL_PIXEL_H
#define SGL_PIXEL_H

#include "core/sgl_types.h"
#include "sgl_display.h"

#ifdef __cplusplus
extern "C" {
#endif

void sgl_write_pixel_1b(sgl_framebuffer_t *fb, int32_t x, int32_t y,
                        uint32_t format_color);
void sgl_write_pixel_8b(sgl_framebuffer_t *fb, int32_t x, int32_t y,
                        uint32_t format_color);
void sgl_write_pixel_16b(sgl_framebuffer_t *fb, int32_t x, int32_t y,
                         uint32_t format_color);
void sgl_write_pixel_24b(sgl_framebuffer_t *fb, int32_t x, int32_t y,
                         uint32_t format_color);
void sgl_write_pixel_32b(sgl_framebuffer_t *fb, int32_t x, int32_t y,
                         uint32_t format_color);
void sgl_write_pixel_hline_1b(sgl_framebuffer_t *fb, int32_t x, int32_t y,
                              int32_t len, uint32_t format_color);
void sgl_write_pixel_hline_8b(sgl_framebuffer_t *fb, int32_t x, int32_t y,
                              int32_t len, uint32_t format_color);
void sgl_write_pixel_hline_16b(sgl_framebuffer_t *fb, int32_t x, int32_t y,
                               int32_t len, uint32_t format_color);
void sgl_write_pixel_hline_24b(sgl_framebuffer_t *fb, int32_t x, int32_t y,
                               int32_t len, uint32_t format_color);
void sgl_write_pixel_hline_32b(sgl_framebuffer_t *fb, int32_t x, int32_t y,
                               int32_t len, uint32_t format_color);

#ifdef __cplusplus
}
#endif

#endif
