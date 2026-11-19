// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#ifndef SGL_TYPES_H
#define SGL_TYPES_H

#include <assert.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int16_t sgl_int_t;
typedef uint32_t sgl_color_t;

#define sgl_check_param(param) assert((param) != 0)

#define sgl_flag_get(type, param, shift, flag) \
    ((param) & (type)((type)1U << ((flag) + (shift))))
#define sgl_flag_set(type, param, shift, flag) \
    ((param) |= (type)((type)1U << ((flag) + (shift))))
#define sgl_flag_reset(type, param, shift, flag) \
    ((param) &= (type) ~(type)((type)1U << ((flag) + (shift))))

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

typedef struct {
    sgl_int_t left;
    sgl_int_t top;
    sgl_int_t right;
    sgl_int_t bottom;
} sgl_area_t;

typedef struct {
    sgl_int_t x;
    sgl_int_t y;
    sgl_int_t w;
    sgl_int_t h;
} sgl_rect_t;

struct sgl_list_head {
    struct sgl_list_head *next;
    struct sgl_list_head *prev;
};

typedef struct sgl_display sgl_display_t;
typedef struct sgl_widget sgl_widget_t;

#ifdef __cplusplus
}
#endif

#endif
