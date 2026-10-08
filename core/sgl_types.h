// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#ifndef SGL_TYPES_H
#define SGL_TYPES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SGL_MONO_BLACK = 0,
    SGL_MONO_WHITE = 1,
    SGL_MONO_INVERT = 2,
} sgl_mono_color_t;

typedef enum {
    SGL_DIR_UP = 0,
    SGL_DIR_RIGHT,
    SGL_DIR_DOWN,
    SGL_DIR_LEFT,
} sgl_dir_t;

#define SGL_DIR_DEFAULT SGL_DIR_UP

/**
 * alignment:
 *
 *     up_left           up_center           up_right
 *             +-------------+-------------+
 *             |                           |
 *             |                           |
 * left_center +        center(x,y)        + right_center
 *             |                           |
 *             |                           |
 *             +-------------+-------------+
 *   down_left          down_center          down_right
 */

typedef enum {
    SGL_ALIGN_UP_LEFT = 0,
    SGL_ALIGN_UP_RIGHT,
    SGL_ALIGN_DOWN_LEFT,
    SGL_ALIGN_DOWN_RIGHT,
    SGL_ALIGN_CENTER,
    SGL_ALIGN_UP_CENTER,
    SGL_ALIGN_DOWN_CENTER,
    SGL_ALIGN_LEFT_CENTER,
    SGL_ALIGN_RIGHT_CENTER,
} sgl_align_t;

#define SGL_ALIGN_DEFAULT SGL_ALIGN_UP_LEFT

typedef enum {
    SGL_ROTATE_0 = 0,
    SGL_ROTATE_90,
    SGL_ROTATE_180,
    SGL_ROTATE_270,
} sgl_rotate_t;

#define SGL_ROTATE_DEFAULT SGL_ROTATE_0

typedef enum {
    SGL_COLOR_FORMAT_MONO,
    SGL_COLOR_FORMAT_RGB332,
    SGL_COLOR_FORMAT_RGB565,
    SGL_COLOR_FORMAT_RGB565SWAP,
    SGL_COLOR_FORMAT_BGR565,
    SGL_COLOR_FORMAT_RGB888,
    SGL_COLOR_FORMAT_BGR888,
    SGL_COLOR_FORMAT_XRGB8888,
    SGL_COLOR_FORMAT_XBGR8888,
    SGL_COLOR_FORMAT_ARGB8888,
    SGL_COLOR_FORMAT_ABGR8888,
    SGL_COLOR_FORMAT_RGBA8888,
    SGL_COLOR_FORMAT_BGRA8888,
} sgl_color_format_t;

typedef struct {
    int32_t left;
    int32_t top;
    int32_t right;
    int32_t bottom;
} sgl_area_t;

typedef struct {
    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;
} sgl_rect_t;

#ifdef __cplusplus
}
#endif

#endif
