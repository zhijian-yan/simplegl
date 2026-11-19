// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#ifndef SGL_PIXEL_H
#define SGL_PIXEL_H

#include "sgl_display.h"

#ifdef __cplusplus
extern "C" {
#endif

void sgl_write_pixel_1bpp(sgl_framebuffer_t *fb, sgl_int_t x, sgl_int_t y,
                          sgl_color_t format_color);
void sgl_write_pixel_8bpp(sgl_framebuffer_t *fb, sgl_int_t x, sgl_int_t y,
                          sgl_color_t format_color);
void sgl_write_pixel_16bpp(sgl_framebuffer_t *fb, sgl_int_t x, sgl_int_t y,
                           sgl_color_t format_color);
void sgl_write_pixel_24bpp(sgl_framebuffer_t *fb, sgl_int_t x, sgl_int_t y,
                           sgl_color_t format_color);
void sgl_write_pixel_32bpp(sgl_framebuffer_t *fb, sgl_int_t x, sgl_int_t y,
                           sgl_color_t format_color);
void sgl_write_pixel_hline_1bpp(sgl_framebuffer_t *fb, sgl_int_t x, sgl_int_t y,
                                sgl_int_t len, sgl_color_t format_color);
void sgl_write_pixel_hline_8bpp(sgl_framebuffer_t *fb, sgl_int_t x, sgl_int_t y,
                                sgl_int_t len, sgl_color_t format_color);
void sgl_write_pixel_hline_16bpp(sgl_framebuffer_t *fb, sgl_int_t x,
                                 sgl_int_t y, sgl_int_t len,
                                 sgl_color_t format_color);
void sgl_write_pixel_hline_24bpp(sgl_framebuffer_t *fb, sgl_int_t x,
                                 sgl_int_t y, sgl_int_t len,
                                 sgl_color_t format_color);
void sgl_write_pixel_hline_32bpp(sgl_framebuffer_t *fb, sgl_int_t x,
                                 sgl_int_t y, sgl_int_t len,
                                 sgl_color_t format_color);

#ifdef __cplusplus
}
#endif

#endif
