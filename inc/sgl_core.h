// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#ifndef SGL_CORE_H
#define SGL_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sgl_types.h"

int sgl_init(sgl_screen_t *scr, sgl_config_t *config);
void sgl_handler(sgl_screen_t *scr);
void sgl_set_flush(sgl_screen_t *scr,
                   void (*flush)(void *buffer, sgl_rect_t *refresh));
int sgl_set_draw_pixel(sgl_screen_t *scr, uint32_t hor_res,
                       uint32_t buffer_size, uint32_t pixel_size,
                       void (*draw_pixel)(sgl_screen_t *scr, int32_t x,
                                          int32_t y, uint32_t color));
void sgl_set_dirty_area(sgl_screen_t *scr, int32_t left, int32_t top,
                        int32_t right, int32_t bottom);
int sgl_set_drawable_area(sgl_screen_t *scr, int32_t left, int32_t top,
                          int32_t right, int32_t bottom);
void sgl_reset_dirty_area(sgl_screen_t *scr);
void sgl_reset_drawable_area(sgl_screen_t *scr);
void sgl_set_screen_rotation(sgl_screen_t *scr, sgl_rotate_t rotate);
uint32_t sgl_get_frame_count(sgl_screen_t *scr);
void sgl_reset_frame_count(sgl_screen_t *scr);
void sgl_clear_buffer(sgl_screen_t *scr, uint8_t value);

#ifdef __cplusplus
}
#endif

#endif
