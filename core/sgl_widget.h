// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#ifndef SGL_WIDGET_H
#define SGL_WIDGET_H

#include "sgl_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*sgl_widget_draw_t)(sgl_display_t *disp, sgl_widget_t *widget);

typedef struct {
    sgl_widget_draw_t draw;
} sgl_widget_class_t;

struct sgl_widget {
    sgl_widget_class_t *cls;
    sgl_widget_t *parent;
    struct sgl_list_head sibling;
    struct sgl_list_head children;
    sgl_rect_t rect;
    sgl_display_t *display;
    void *user_data;
    uint16_t flags;
};

void sgl_widget_init(sgl_widget_t *widget, sgl_widget_class_t *cls, sgl_int_t x,
                     sgl_int_t y, sgl_int_t w, sgl_int_t h, void *user_data);
void sgl_widget_class_config(sgl_widget_class_t *cls, sgl_widget_draw_t draw);
int sgl_widget_add(sgl_widget_t *widget, sgl_widget_t *parent);
int sgl_widget_remove(sgl_widget_t *widget);
void sgl_widget_get_origin_coords(sgl_widget_t *widget, sgl_int_t *ox,
                                  sgl_int_t *oy);
void sgl_widget_set_coords(sgl_widget_t *widget, sgl_int_t x, sgl_int_t y);
void sgl_widget_set_size(sgl_widget_t *widget, sgl_int_t w, sgl_int_t h);
void sgl_widget_set_rect(sgl_widget_t *widget, sgl_int_t x, sgl_int_t y,
                         sgl_int_t w, sgl_int_t h);
sgl_area_t sgl_widget_get_bounds(sgl_widget_t *widget);

#ifdef __cplusplus
}
#endif

#endif
