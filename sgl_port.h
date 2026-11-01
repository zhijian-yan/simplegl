// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#ifndef SGL_PORT_H
#define SGL_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

static inline int sgl_lock(void) {
    return 0;
}

static inline void sgl_unlock(int sgl_lock_state) {
    (void)sgl_lock_state;
}

#ifdef __cplusplus
}
#endif

#endif
