// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#ifndef SGL_STRING_H
#define SGL_STRING_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sgl_types.h"

#define SGL_FORMAT_STRING_BUFFERSIZE (128)

void sgl_show_string(sgl_display_t *disp, int32_t x, int32_t y, const char *str,
                     int32_t length, sgl_align_t align, sgl_dir_t dir,
                     uint32_t color);
int sgl_show_format(sgl_display_t *disp, int32_t x, int32_t y,
                    sgl_align_t align, sgl_dir_t dir, uint32_t color,
                    const char *format, ...);
void sgl_show_string_default(sgl_display_t *disp, int32_t x, int32_t y,
                             const char *str, int32_t length, uint32_t color);
int sgl_show_format_default(sgl_display_t *disp, int32_t x, int32_t y,
                            uint32_t color, const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif
