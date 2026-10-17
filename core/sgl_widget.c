// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#include "sgl_widget.h"
#include <string.h>

int sgl_widget_init(sgl_widget_t *widget, sgl_widget_vtable_t *vtable,
                    int32_t x, int32_t y, int32_t w, int32_t h,
                    void *user_data) {
    if (!widget || !vtable)
        return -1;
    memset(widget, 0, sizeof(sgl_widget_t));
    sgl_init_list_head(&widget->sibling);
    sgl_init_list_head(&widget->children);
    widget->rect.x = x;
    widget->rect.y = y;
    widget->rect.w = w;
    widget->rect.h = h;
    widget->vtable = vtable;
    widget->user_data = user_data;
    return 0;
}

void sgl_widget_vtable_config(sgl_widget_vtable_t *vtable,
                              sgl_widget_draw_t draw) {
    vtable->draw = draw;
}

static void sgl_widget_set_display(sgl_widget_t *widget, sgl_display_t *disp) {
    sgl_widget_t *child;
    widget->disp = disp;
    sgl_list_for_each_entry(child, &widget->children, sibling) {
        sgl_widget_set_display(child, disp);
    }
}

int sgl_widget_add(sgl_widget_t *widget, sgl_widget_t *parent) {
    if (!widget || !parent || widget == parent || widget->parent)
        return -1;
    widget->parent = parent;
    sgl_list_add_tail(&widget->sibling, &parent->children);
    sgl_widget_set_display(widget, parent->disp);
    return 0;
}

int sgl_widget_remove(sgl_widget_t *widget) {
    if (!widget || !widget->parent)
        return -1;
    sgl_list_del(&widget->sibling);
    sgl_widget_set_display(widget, NULL);
    return 0;
}
