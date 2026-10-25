// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#ifndef SGL_CORE_H
#define SGL_CORE_H

#include "display/sgl_display.h"

#ifdef __cplusplus
extern "C" {
#endif

int sgl_draw(sgl_display_t *disp, sgl_area_t *flush_area);
int sgl_set_clip_area(sgl_display_t *disp, int32_t left, int32_t top,
                      int32_t right, int32_t bottom);
void sgl_restore_clip_area(sgl_display_t *disp);
void sgl_invalidate_area(sgl_display_t *disp, int32_t left, int32_t top,
                         int32_t right, int32_t bottom);
void sgl_set_full_screen(sgl_display_t *disp, uint8_t enable);
void sgl_set_idle_redraw(sgl_display_t *disp, uint8_t enable);

#ifdef __cplusplus
}
#endif

#endif
