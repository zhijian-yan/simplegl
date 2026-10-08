// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#ifndef SGL_DISPLAY_H
#define SGL_DISPLAY_H

#include "core/sgl_types.h"
#include "core/sgl_widget.h"

#ifdef __cplusplus
extern "C" {
#endif

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
} sgl_display_config_t;

typedef struct {
    void *buffer;
    uint32_t buffer_size;
    uint32_t buffer_width;
    uint32_t pixel_num;
    uint32_t pixel_size;
    uint32_t pixel_index;
} sgl_framebuffer_t;

typedef void (*sgl_write_pixel_t)(sgl_framebuffer_t *fb, int32_t x, int32_t y,
                                  uint32_t color);

typedef struct sgl_display {
    sgl_framebuffer_t fb;
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
    uint8_t slice_state;
    sgl_rotate_t rotate;
    sgl_widget_t root;
    sgl_widget_vtable_t background;
    void *user_data;
    sgl_write_pixel_t write_pixel;
    void (*flush)(void *buffer, sgl_rect_t *refresh);
    void (*frame_start)(void *user_data);
    void (*frame_end)(void *user_data);
} sgl_display_t;

int sgl_display_init(sgl_display_t *disp, const sgl_display_config_t *config);
int sgl_set_write_pixel(sgl_display_t *disp, uint32_t hor_res,
                        uint32_t buffer_size, uint32_t pixel_size,
                        sgl_write_pixel_t write_pixel);
void sgl_set_flush(sgl_display_t *disp,
                   void (*flush)(void *buffer, sgl_rect_t *refresh));
uint32_t sgl_get_frame_count(const sgl_display_t *disp);
void sgl_reset_frame_count(sgl_display_t *disp);
void sgl_clear_buffer(sgl_display_t *disp, uint8_t value);

#ifdef __cplusplus
}
#endif

#endif
