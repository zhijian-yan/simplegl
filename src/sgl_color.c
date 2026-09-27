// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#include "../inc/sgl_color.h"

void sgl_color_hsv2rgb(uint16_t h, uint8_t s, uint8_t v, uint8_t *r, uint8_t *g,
                       uint8_t *b) {
    uint8_t sv_r8, i, f, p, q, t;
    if (s == 0) {
        *r = *g = *b = v;
        return;
    }
    sv_r8 = (s * v) >> 8;
    h = (h * 1536) / 360;
    i = h >> 8;
    f = h & 255;
    p = v - sv_r8;
    q = v - ((sv_r8 * f) >> 8);
    t = v - ((sv_r8 * (255 - f)) >> 8);
    switch (i) {
        case 0:
            *r = v;
            *g = t;
            *b = p;
            break;
        case 1:
            *r = q;
            *g = v;
            *b = p;
            break;
        case 2:
            *r = p;
            *g = v;
            *b = t;
            break;
        case 3:
            *r = p;
            *g = q;
            *b = v;
            break;
        case 4:
            *r = t;
            *g = p;
            *b = v;
            break;
        default:
            *r = v;
            *g = p;
            *b = q;
            break;
    }
}

void sgl_color_rgb2hsv(uint8_t r, uint8_t g, uint8_t b, uint16_t *h, uint8_t *s,
                       uint8_t *v) {
    uint8_t max, min, delta;
    max = r > g ? (r > b ? r : b) : (g > b ? g : b);
    min = r < g ? (r < b ? r : b) : (g < b ? g : b);
    *v = max;
    delta = max - min;
    if (delta == 0) {
        *h = 0;
        *s = 0;
        return;
    }
    *s = (uint8_t)((uint16_t)delta * 255 / max);
    if (max == r) {
        *h = 60 * (int16_t)(g - b) / delta;
        if ((int16_t)*h < 0)
            *h += 360;
    } else if (max == g) {
        *h = 60 * (int16_t)(b - r) / delta + 120;
    } else {
        *h = 60 * (int16_t)(r - g) / delta + 240;
    }
}
