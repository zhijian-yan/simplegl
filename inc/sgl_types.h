// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Zhijian Yan

#ifndef SGL_TYPES_H
#define SGL_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sgl_list.h"
#include <stdint.h>

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
    SGL_SLICE_STATE_IDLE,
    SGL_SLICE_STATE_START,
    SGL_SLICE_STATE_RUNNING,
} sgl_slice_state_t;

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

typedef struct sgl_screen sgl_screen_t;

typedef struct sgl_widget sgl_widget_t;

typedef void (*sgl_widget_draw_t)(sgl_screen_t *scr, sgl_widget_t *widget);

struct sgl_widget {
    sgl_widget_t *parent;
    struct list_head sibling;
    struct list_head children;
    sgl_rect_t rect;
    uint16_t flags;
    uint16_t draw_index;
    void *user_data;
};

typedef struct {
    void *buffer;
    uint32_t buffer_size;
    sgl_color_format_t color_format;
    sgl_rotate_t rotate;
    uint32_t hor_res;
    uint32_t ver_res;
    void *user_data;
    void (*frame_start)(void *user_data);
    void (*frame_end)(void *user_data);
} sgl_config_t;

struct sgl_screen {
    void *buffer;
    uint32_t buffer_size;
    uint32_t buffer_width;
    uint32_t pixel_num;
    uint32_t pixel_index;
    uint32_t hor_res;
    uint32_t ver_res;
    uint32_t max_x;
    uint32_t max_y;
    uint32_t buffer_offset_x;
    uint32_t buffer_offset_y;
    int32_t logical_offset_x;
    int32_t logical_offset_y;
    sgl_area_t drawable_area;
    sgl_area_t widget_bounds;
    sgl_area_t dirty_area;
    sgl_area_t slice_area;
    sgl_rect_t frame_rect;
    sgl_rect_t dirty_rect;
    sgl_rect_t slice_rect;
    uint32_t slice_count;
    uint32_t frame_count;
    sgl_slice_state_t slice_state;
    sgl_rotate_t rotate;
    sgl_widget_t root;
    void *user_data;
    void (*flush)(void *buffer, sgl_rect_t *refresh);
    void (*draw_pixel)(sgl_screen_t *scr, int32_t x, int32_t y, uint32_t color);
    void (*frame_start)(void *user_data);
    void (*frame_end)(void *user_data);
};

#ifdef __cplusplus
}
#endif

#endif
