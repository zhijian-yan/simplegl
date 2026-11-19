// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#ifndef SGL_STRING_H
#define SGL_STRING_H

#include "display/sgl_display.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SGL_FORMAT_STRING_BUFFERSIZE (128)

static inline void sgl_align(sgl_int_t *x, sgl_int_t *y, sgl_int_t w,
                             sgl_int_t h, sgl_align_t align) {
    switch (align) {
        case SGL_ALIGN_UP_LEFT:
            break;
        case SGL_ALIGN_UP_RIGHT:
            *x -= w - 1;
            break;
        case SGL_ALIGN_DOWN_LEFT:
            *y -= h - 1;
            break;
        case SGL_ALIGN_DOWN_RIGHT:
            *x -= w - 1;
            *y -= h - 1;
            break;
        case SGL_ALIGN_CENTER:
            *x -= w / 2;
            *y -= h / 2;
            break;
        case SGL_ALIGN_UP_CENTER:
            *x -= w / 2;
            break;
        case SGL_ALIGN_DOWN_CENTER:
            *x -= w / 2;
            *y -= h - 1;
            break;
        case SGL_ALIGN_LEFT_CENTER:
            *y -= h / 2;
            break;
        case SGL_ALIGN_RIGHT_CENTER:
            *x -= w - 1;
            *y -= h / 2;
            break;
    }
}

void sgl_show_string(sgl_display_t *disp, sgl_int_t x, sgl_int_t y,
                     const char *str, sgl_int_t length, sgl_align_t align,
                     sgl_dir_t dir, sgl_color_t color);
int sgl_show_format(sgl_display_t *disp, sgl_int_t x, sgl_int_t y,
                    sgl_align_t align, sgl_dir_t dir, sgl_color_t color,
                    const char *format, ...);
void sgl_show_string_default(sgl_display_t *disp, sgl_int_t x, sgl_int_t y,
                             const char *str, sgl_int_t length,
                             sgl_color_t color);
int sgl_show_format_default(sgl_display_t *disp, sgl_int_t x, sgl_int_t y,
                            sgl_color_t color, const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif
