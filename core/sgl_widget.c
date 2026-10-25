// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#include "sgl_widget.h"
#include "core/sgl_core.h"
#include "draw/sgl_common.h"
#include "sgl_list.h"

#define sgl_widget_walk_to_root(widget, parent) \
    for (parent = widget->parent; parent; parent = parent->parent)

void sgl_widget_init(sgl_widget_t *widget, sgl_widget_class_t *cls, int32_t x,
                     int32_t y, int32_t w, int32_t h, void *user_data) {
    sgl_check_param(widget);
    sgl_check_param(cls);
    memset(widget, 0, sizeof(sgl_widget_t));
    sgl_init_list_head(&widget->sibling);
    sgl_init_list_head(&widget->children);
    sgl_normalize_rect(&x, &y, &w, &h);
    widget->rect.x = x;
    widget->rect.y = y;
    widget->rect.w = w;
    widget->rect.h = h;
    widget->cls = cls;
    widget->user_data = user_data;
}

void sgl_widget_class_config(sgl_widget_class_t *cls, sgl_widget_draw_t draw) {
    sgl_check_param(cls);
    cls->draw = draw;
}

static void sgl_widget_set_display(sgl_widget_t *widget, sgl_display_t *disp) {
    sgl_widget_t *child;
    widget->disp = disp;
    sgl_list_for_each_entry(child, &widget->children, sibling) {
        sgl_widget_set_display(child, disp);
    }
}

int sgl_widget_add(sgl_widget_t *widget, sgl_widget_t *parent) {
    sgl_check_param(widget);
    sgl_check_param(parent);
    if (widget == parent || widget->parent)
        return -1;
    widget->parent = parent;
    sgl_list_add_tail(&widget->sibling, &parent->children);
    sgl_widget_set_display(widget, parent->disp);
    return 0;
}

int sgl_widget_remove(sgl_widget_t *widget) {
    sgl_check_param(widget);
    if (!widget->parent)
        return -1;
    sgl_list_del(&widget->sibling);
    sgl_widget_set_display(widget, NULL);
    return 0;
}

void sgl_widget_get_origin_coords(sgl_widget_t *widget, int32_t *ox,
                                  int32_t *oy) {
    sgl_check_param(widget);
    sgl_check_param(ox);
    sgl_check_param(oy);
    sgl_widget_t *parent;
    int32_t x = widget->rect.x;
    int32_t y = widget->rect.y;
    sgl_widget_walk_to_root(widget, parent) {
        x += parent->rect.x;
        y += parent->rect.y;
    }
    *ox = x;
    *oy = y;
}

sgl_area_t sgl_widget_get_bounds(sgl_widget_t *widget) {
    sgl_check_param(widget);
    sgl_area_t bounds = {0};
    sgl_widget_get_origin_coords(widget, &bounds.left, &bounds.top);
    bounds.right = bounds.left + widget->rect.w - 1;
    bounds.bottom = bounds.top + widget->rect.h - 1;
    return bounds;
}

void sgl_invalidate_widget(sgl_widget_t *widget) {
    sgl_check_param(widget);
    if (!widget->disp)
        return;
    sgl_area_t area = sgl_widget_get_bounds(widget);
    sgl_invalidate_area(widget->disp, area.left, area.top, area.right,
                        area.bottom);
}

void sgl_widget_set_coords(sgl_widget_t *widget, int32_t x, int32_t y) {
    sgl_check_param(widget);
    widget->rect.x = x;
    widget->rect.y = y;
}

void sgl_widget_set_size(sgl_widget_t *widget, int32_t w, int32_t h) {
    sgl_check_param(widget);
    widget->rect.w = w;
    widget->rect.h = h;
}

void sgl_widget_set_rect(sgl_widget_t *widget, int32_t x, int32_t y, int32_t w,
                         int32_t h) {
    sgl_check_param(widget);
    sgl_normalize_rect(&x, &y, &w, &h);
    widget->rect.x = x;
    widget->rect.y = y;
    widget->rect.w = w;
    widget->rect.h = h;
}
