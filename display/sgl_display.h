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
    void *framebuffer;
    int32_t buffer_size;
    void *dirty_buffer;
    uint8_t dirty_num;
    int32_t hor_res;
    int32_t ver_res;
    sgl_pixel_format_t pixel_format;
    void (*frame_start)(void *user_data);
    void (*frame_end)(void *user_data);
    void *user_data;
} sgl_display_config_t;

typedef struct {
    void *buffer;
    int32_t buffer_size;
    int32_t dirty_width;
    int32_t pixel_num;
    int32_t pixel_size;
    int32_t pixel_index;
} sgl_framebuffer_t;

typedef struct {
    sgl_area_t slice_area;
    sgl_rect_t frame_rect;
    uint16_t slice_count;
    uint8_t slice_state;
} sgl_slice_t;

typedef struct {
    sgl_area_t *dirty_buffer;
    uint8_t dirty_count;
    uint8_t dirty_num;
    uint8_t flag_full_screen;
} sgl_dirty_t;

typedef void (*sgl_write_pixel_t)(sgl_framebuffer_t *fb, int32_t x, int32_t y,
                                  uint32_t format_color);

typedef void (*sgl_write_pixel_hline_t)(sgl_framebuffer_t *fb, int32_t x,
                                        int32_t y, int32_t len,
                                        uint32_t format_color);

typedef struct sgl_display {
    sgl_framebuffer_t fb;
    sgl_slice_t slice;
    sgl_dirty_t dirty;
    int32_t logical_ox;
    int32_t logical_oy;
    int32_t buffer_ox;
    int32_t buffer_oy;
    sgl_area_t clip_area;
    sgl_area_t widget_bounds;
    int32_t frame_count;
    sgl_widget_t root;
    sgl_widget_class_t background;
    sgl_write_pixel_t write_pixel;
    sgl_write_pixel_hline_t write_pixel_hline;
    void (*frame_start)(void *user_data);
    void (*frame_end)(void *user_data);
    void *user_data;
} sgl_display_t;

int sgl_display_init(sgl_display_t *disp, const sgl_display_config_t *config);
void sgl_clear_framebuffer(sgl_display_t *disp, uint8_t value);
void sgl_clear_rect(sgl_display_t *disp, int32_t x, int32_t y, int32_t w,
                    int32_t h, uint8_t value);
void sgl_clear_widget(const sgl_widget_t *widget, uint8_t value);

static inline void sgl_set_framebuffer(sgl_display_t *disp, void *buffer) {
    disp->fb.buffer = buffer;
}

static inline int32_t sgl_get_frame_count(const sgl_display_t *disp) {
    return disp->frame_count;
}

static inline void sgl_reset_frame_count(sgl_display_t *disp) {
    disp->frame_count = 0;
}

#ifdef __cplusplus
}
#endif

#endif
