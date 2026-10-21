// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#ifndef SGL_WIDGET_H
#define SGL_WIDGET_H

#include "sgl_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sgl_display sgl_display_t;

typedef struct sgl_widget sgl_widget_t;

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
    sgl_display_t *disp;
    void *user_data;
    uint16_t flags;
};

int sgl_widget_init(sgl_widget_t *widget, sgl_widget_class_t *cls, int32_t x,
                    int32_t y, int32_t w, int32_t h, void *user_data);
void sgl_widget_class_config(sgl_widget_class_t *cls, sgl_widget_draw_t draw);
int sgl_widget_add(sgl_widget_t *widget, sgl_widget_t *parent);
int sgl_widget_remove(sgl_widget_t *widget);
int sgl_widget_get_origin_coords(sgl_widget_t *widget, int32_t *ox,
                                 int32_t *oy);
void sgl_widget_set_coords(sgl_widget_t *widget, int32_t x, int32_t y);
void sgl_widget_set_size(sgl_widget_t *widget, int32_t w, int32_t h);
void sgl_widget_set_rect(sgl_widget_t *widget, int32_t x, int32_t y, int32_t w,
                         int32_t h);
sgl_area_t sgl_widget_get_bounds(sgl_widget_t *widget);

#ifdef __cplusplus
}
#endif

#endif
