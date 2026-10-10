// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#ifndef SGL_DISPLAY_H
#define SGL_DISPLAY_H

#include "core/sgl_types.h"
#include "core/sgl_widget.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SGL_PIXEL_FORMAT_1BIT,
    SGL_PIXEL_FORMAT_8BIT,
    SGL_PIXEL_FORMAT_16BIT,
    SGL_PIXEL_FORMAT_24BIT,
    SGL_PIXEL_FORMAT_32BIT,
} sgl_pixel_format_t;

typedef struct {
    void *buffer;
    uint32_t buffer_size;
    sgl_rotate_t rotate;
    uint32_t hor_res;
    uint32_t ver_res;
    void *user_data;
    sgl_pixel_format_t pixel_format;
    void (*frame_start)(void *user_data);
    void (*frame_end)(void *user_data);
} sgl_display_config_t;

typedef struct {
    void *buffer;
    uint32_t buffer_size;
    uint32_t dirty_width;
    uint32_t pixel_num;
    uint32_t pixel_size;
    uint32_t pixel_index;
} sgl_framebuffer_t;

typedef void (*sgl_write_pixel_t)(sgl_framebuffer_t *fb, int32_t x, int32_t y,
                                  uint32_t format_color);

typedef void (*sgl_write_pixel_hline_t)(sgl_framebuffer_t *fb, int32_t x,
                                        int32_t y, int32_t len,
                                        uint32_t format_color);

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
    sgl_write_pixel_hline_t write_pixel_hline;
    void (*frame_start)(void *user_data);
    void (*frame_end)(void *user_data);
} sgl_display_t;

int sgl_display_init(sgl_display_t *disp, const sgl_display_config_t *config);
int sgl_set_pixel_format(sgl_display_t *disp, uint32_t buffer_size,
                         sgl_pixel_format_t pixel_format);
void sgl_clear_framebuffer(sgl_display_t *disp, uint8_t value);
void sgl_clear_rect(sgl_display_t *disp, int32_t x, int32_t y, int32_t w,
                    int32_t h, uint8_t value);
void sgl_clear_widget(sgl_display_t *disp, const sgl_widget_t *widget,
                      uint8_t value);

static inline void sgl_set_framebuffer(sgl_display_t *disp, void *buffer) {
    disp->fb.buffer = buffer;
}

static inline uint32_t sgl_get_frame_count(const sgl_display_t *disp) {
    return disp->frame_count;
}

static inline void sgl_reset_frame_count(sgl_display_t *disp) {
    disp->frame_count = 0;
}

#ifdef __cplusplus
}
#endif

#endif
