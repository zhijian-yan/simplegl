// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Zhijian Yan

#include "../inc/sgl_widget.h"
#include "../inc/sgl_common.h"
#include <string.h>

void sgl_widget_init(sgl_widget_t *widget, int32_t x, int32_t y, int32_t w,
                     int32_t h, sgl_widget_vtable_t *vtable, void *user_data) {
    memset(widget, 0, sizeof(sgl_widget_t));
    INIT_LIST_HEAD(&widget->sibling);
    INIT_LIST_HEAD(&widget->children);
    sgl_set_rect(&widget->rect, x, y, w, h);
    widget->vtable = vtable;
    widget->user_data = user_data;
}

void sgl_widget_vtable_config(sgl_widget_vtable_t *vtable, sgl_draw_t draw) {
    vtable->draw = draw;
}

void sgl_widget_set_rect(sgl_widget_t *widget, int32_t x, int32_t y, int32_t w,
                         int32_t h) {
    sgl_set_rect(&widget->rect, x, y, w, h);
}

int sgl_widget_add(sgl_widget_t *widget, sgl_widget_t *parent) {
    if (widget == parent)
        return -1;
    if (widget->parent)
        return -1;
    widget->parent = parent;
    list_add_tail(&widget->sibling, &parent->children);
    return 0;
}

int sgl_widget_remove(sgl_widget_t *widget) {
    if (!widget->parent)
        return -1;
    list_del(&widget->sibling);
    widget->parent = NULL;
    return 0;
}
