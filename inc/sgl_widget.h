// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Zhijian Yan

#ifndef SGL_WIDGET_H
#define SGL_WIDGET_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sgl_types.h"

#define SGL_WIDGET_MAX_DRAW_INDEX 10

int sgl_widget_init(sgl_widget_t *widget, uint16_t draw_index, int32_t x,
                    int32_t y, int32_t w, int32_t h, void *user_data);
void sgl_widget_set_draw(sgl_widget_t *widget, uint16_t draw_index,
                         void *user_data);
void sgl_widget_set_rect(sgl_widget_t *widget, int32_t x, int32_t y, int32_t w,
                         int32_t h);
int sgl_widget_register_draw(sgl_widget_draw_t draw);
sgl_widget_draw_t sgl_widget_get_draw(uint32_t draw_index);
int sgl_widget_add(sgl_widget_t *widget, sgl_widget_t *parent);
int sgl_widget_remove(sgl_widget_t *widget);

static inline void sgl_widget_set_coords(sgl_widget_t *widget, int32_t x,
                                         int32_t y) {
    widget->rect.x = x;
    widget->rect.y = y;
}

static inline void sgl_widget_set_size(sgl_widget_t *widget, int32_t w,
                                       int32_t h) {
    widget->rect.w = w;
    widget->rect.h = h;
}

#ifdef __cplusplus
}
#endif

#endif
