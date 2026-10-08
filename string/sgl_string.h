// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#ifndef SGL_STRING_H
#define SGL_STRING_H

#include "core/sgl_types.h"
#include "display/sgl_display.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SGL_FORMAT_STRING_BUFFERSIZE (128)

static inline void sgl_align(int32_t *x, int32_t *y, int32_t w, int32_t h,
                             sgl_align_t align) {
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
