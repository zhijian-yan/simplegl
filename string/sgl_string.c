// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#include "sgl_string.h"
#include "draw/sgl_bitmap.h"
#include "sgl_builtin_font.h"
#include <stdarg.h>
#include <stdio.h>

void sgl_show_string(sgl_display_t *disp, int32_t x, int32_t y, const char *str,
                     int32_t length, sgl_align_t align, sgl_dir_t dir,
                     uint32_t color) {
    int32_t i;
    int32_t fontwidth = 8;
    int32_t fontheight = 16;
    int32_t fontsize = (fontheight + 7) / 8 * fontwidth;
    switch (dir) {
        case SGL_DIR_UP:
            sgl_align(&x, &y, fontwidth * length, fontheight, align);
            for (i = 0; i < length; ++i)
                sgl_draw_bitmap_1b(
                    disp, x + i * fontwidth, y, fontwidth, fontheight,
                    &sgl_builtin_fixedsys_8x16[(str[i] - ' ') * fontsize], dir,
                    color);
            break;
        case SGL_DIR_RIGHT:
            sgl_align(&x, &y, fontheight, fontwidth * length, align);
            for (i = 0; i < length; ++i)
                sgl_draw_bitmap_1b(
                    disp, x, y + i * fontwidth, fontwidth, fontheight,
                    &sgl_builtin_fixedsys_8x16[(str[i] - ' ') * fontsize], dir,
                    color);
            break;
        case SGL_DIR_LEFT:
            sgl_align(&x, &y, fontheight, fontwidth * length, align);
            y += (length - 1) * fontwidth;
            for (i = 0; i < length; ++i)
                sgl_draw_bitmap_1b(
                    disp, x, y - i * fontwidth, fontwidth, fontheight,
                    &sgl_builtin_fixedsys_8x16[(str[i] - ' ') * fontsize], dir,
                    color);
            break;
        case SGL_DIR_DOWN:
            sgl_align(&x, &y, fontwidth * length, fontheight, align);
            x += (length - 1) * fontwidth;
            for (i = 0; i < length; ++i)
                sgl_draw_bitmap_1b(
                    disp, x - i * fontwidth, y, fontwidth, fontheight,
                    &sgl_builtin_fixedsys_8x16[(str[i] - ' ') * fontsize], dir,
                    color);
            break;
    }
}

int sgl_show_format(sgl_display_t *disp, int32_t x, int32_t y,
                    sgl_align_t align, sgl_dir_t dir, uint32_t color,
                    const char *format, ...) {
    char buffer[SGL_FORMAT_STRING_BUFFERSIZE];
    va_list args;
    va_start(args, format);
    int length = vsnprintf(buffer, SGL_FORMAT_STRING_BUFFERSIZE, format, args);
    va_end(args);
    sgl_show_string(disp, x, y, buffer, length, align, dir, color);
    return length;
}

void sgl_show_string_default(sgl_display_t *disp, int32_t x, int32_t y,
                             const char *str, int32_t length, uint32_t color) {
    sgl_show_string(disp, x, y, str, length, SGL_ALIGN_DEFAULT, SGL_DIR_DEFAULT,
                    color);
}

int sgl_show_format_default(sgl_display_t *disp, int32_t x, int32_t y,
                            uint32_t color, const char *format, ...) {
    char buffer[SGL_FORMAT_STRING_BUFFERSIZE];
    va_list args;
    va_start(args, format);
    int length = vsnprintf(buffer, SGL_FORMAT_STRING_BUFFERSIZE, format, args);
    va_end(args);
    sgl_show_string(disp, x, y, buffer, length, SGL_ALIGN_DEFAULT,
                    SGL_DIR_DEFAULT, color);
    return length;
}
