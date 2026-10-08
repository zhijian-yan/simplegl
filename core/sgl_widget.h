// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#ifndef SGL_WIDGET_H
#define SGL_WIDGET_H

#include "sgl_list.h"
#include "sgl_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sgl_display sgl_display_t;

typedef struct sgl_widget sgl_widget_t;

typedef void (*sgl_draw_t)(sgl_display_t *disp, sgl_widget_t *widget);

typedef struct {
    sgl_draw_t draw;
} sgl_widget_vtable_t;

struct sgl_widget {
    sgl_widget_t *parent;
    struct sgl_list_head sibling;
    struct sgl_list_head children;
    sgl_rect_t rect;
    sgl_widget_vtable_t *vtable;
    void *user_data;
    uint16_t flags;
};

void sgl_widget_init(sgl_widget_t *widget, int32_t x, int32_t y, int32_t w,
                     int32_t h, sgl_widget_vtable_t *vtable, void *user_data);
void sgl_widget_set_rect(sgl_widget_t *widget, int32_t x, int32_t y, int32_t w,
                         int32_t h);
void sgl_widget_vtable_config(sgl_widget_vtable_t *vtable, sgl_draw_t draw);
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
