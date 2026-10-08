// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#include "sgl_circle.h"
#include "sgl_common.h"
#include "sgl_line.h"

/**
 * +-----------> X
 * |    6 7
 * | 5       8
 * | 4       1
 * |    3 2
 * v
 * Y
 */

void sgl_draw_circle_section(sgl_display_t *disp, int32_t xc, int32_t yc,
                             int32_t r, int32_t offset_x, int32_t offset_y,
                             uint32_t color) {
    int32_t x, y, f, ddF_x, ddF_y;
    x = 0;
    y = r;
    f = 1 - r;
    ddF_x = 1;
    ddF_y = -2 * r;
    while (1) {
        if (f >= 0) {
            --y;
            ddF_y += 2;
            f += ddF_y;
        }
        ++x;
        ddF_x += 2;
        f += ddF_x;
        if (x >= y)
            break;
        sgl_draw_point(disp, xc + y + offset_x, yc + x + offset_y, color); // 1
        sgl_draw_point(disp, xc + x + offset_x, yc + y + offset_y, color); // 2
        sgl_draw_point(disp, xc - x, yc + y + offset_y, color);            // 3
        sgl_draw_point(disp, xc - y, yc + x + offset_y, color);            // 4
        sgl_draw_point(disp, xc - y, yc - x, color);                       // 5
        sgl_draw_point(disp, xc - x, yc - y, color);                       // 6
        sgl_draw_point(disp, xc + x + offset_x, yc - y, color);            // 7
        sgl_draw_point(disp, xc + y + offset_x, yc - x, color);            // 8
    }
    if (x == y) {
        sgl_draw_point(disp, xc + y + offset_x, yc + x + offset_y,
                       color);                                  // 1, 2
        sgl_draw_point(disp, xc - x, yc + y + offset_y, color); // 3, 4
        sgl_draw_point(disp, xc - y, yc - x, color);            // 5, 6
        sgl_draw_point(disp, xc + x + offset_x, yc - y, color); // 7, 8
    }
}

void sgl_draw_filled_circle_section(sgl_display_t *disp, int32_t xc, int32_t yc,
                                    int32_t r, int32_t offset_x,
                                    int32_t offset_y, uint32_t color) {
    int32_t x, y, f, ddF_x, ddF_y, len;
    x = 0;
    y = r;
    f = 1 - r;
    ddF_x = 1;
    ddF_y = -2 * r;
    while (1) {
        if (f >= 0) {
            --y;
            ddF_y += 2;
            f += ddF_y;
        }
        ++x;
        ddF_x += 2;
        f += ddF_x;
        if (x >= y)
            break;
        len = y - x;
        sgl_draw_hline(disp, xc + x + 1 + offset_x, yc + x + offset_y, len,
                       color); // 1
        sgl_draw_vline(disp, xc + x + offset_x, yc + x + 1 + offset_y, len,
                       color);                                           // 2
        sgl_draw_vline(disp, xc - x, yc + x + 1 + offset_y, len, color); // 3
        sgl_draw_hline(disp, xc - y, yc + x + offset_y, len, color);     // 4
        sgl_draw_hline(disp, xc - y, yc - x, len, color);                // 5
        sgl_draw_vline(disp, xc - x, yc - y, len, color);                // 6
        sgl_draw_vline(disp, xc + x + offset_x, yc - y, len, color);     // 7
        sgl_draw_hline(disp, xc + x + 1 + offset_x, yc - x, len, color); // 8
    }
    if (x == y)
        ++x;
    while (x > 1) {
        --x;
        sgl_draw_point(disp, xc + x + offset_x, yc + x + offset_y,
                       color);                                  // 1, 2
        sgl_draw_point(disp, xc - x, yc + x + offset_y, color); // 3, 4
        sgl_draw_point(disp, xc - x, yc - x, color);            // 5, 6
        sgl_draw_point(disp, xc + x + offset_x, yc - x, color); // 7, 8
    }
}

void __sgl_draw_circle(sgl_display_t *disp, int32_t xc, int32_t yc, int32_t r,
                       int32_t offset, int32_t is_filled, uint32_t color) {
    if (is_filled == 0 && r > 0) {
        if (offset == 0) {
            sgl_draw_point(disp, xc + r, yc, color); // 8, 1
            sgl_draw_point(disp, xc, yc + r, color); // 3, 2
            sgl_draw_point(disp, xc - r, yc, color); // 5, 4
            sgl_draw_point(disp, xc, yc - r, color); // 6, 7
        }
        sgl_draw_circle_section(disp, xc, yc, r, offset, offset, color);
    } else {
        if (offset == 0) {
            sgl_draw_point(disp, xc, yc, color);         // xc, yc
            sgl_draw_hline(disp, xc + 1, yc, r, color);  // 8, 1
            sgl_draw_vline(disp, xc, yc + 1, r, color);  // 3, 2
            sgl_draw_hline(disp, xc - 1, yc, -r, color); // 5, 4
            sgl_draw_vline(disp, xc, yc - 1, -r, color); // 6, 7
        }
        sgl_draw_filled_circle_section(disp, xc, yc, r, offset, offset, color);
    }
}

void sgl_draw_circle(sgl_display_t *disp, int32_t x, int32_t y, int32_t d,
                     int32_t is_filled, uint32_t color) {
    int32_t r;
    if (d < 0)
        d = -d;
    if (sgl_check_area(&disp->drawable_area, x, y, x + d, y + d))
        return;
    if (d == 2) {
        sgl_draw_hline(disp, x, y, 2, color);
        sgl_draw_hline(disp, x, y + 1, 2, color);
        return;
    }
    r = d >> 1;
    if (d & 0x1)
        __sgl_draw_circle(disp, x + r, y + r, r, 0, is_filled, color);
    else
        __sgl_draw_circle(disp, x + r, y + r, r, -1, is_filled, color);
}

void sgl_draw_circle_center(sgl_display_t *disp, int32_t xc, int32_t yc,
                            int32_t r, int32_t is_filled, uint32_t color) {
    if (r < 0)
        r = -r;
    if (sgl_check_area(&disp->drawable_area, xc - r, yc - r, xc + r, yc + r))
        return;
    __sgl_draw_circle(disp, xc, yc, r, 0, is_filled, color);
}

void sgl_draw_ellipse_section(sgl_display_t *disp, int32_t xc, int32_t yc,
                              int32_t rx, int32_t ry, uint32_t color) {
    int32_t x, y, err, rxrx2, ryry2, xchg, ychg, stopx, stopy;
    rxrx2 = (rx * rx) << 1;
    ryry2 = (ry * ry) << 1;
    x = rx;
    y = 0;
    xchg = (1 - rx - rx) * (ryry2 >> 1);
    ychg = rxrx2 >> 1;
    err = 0;
    stopx = ryry2 * rx;
    stopy = 0;
    while (1) {
        ++y;
        stopy += rxrx2;
        err += ychg;
        ychg += rxrx2;
        if (2 * err + xchg > 0) {
            --x;
            stopx -= ryry2;
            err += xchg;
            xchg += ryry2;
        }
        if (stopx <= stopy)
            break;
        sgl_draw_point(disp, xc + x, yc + y, color); // 1
        sgl_draw_point(disp, xc - x, yc + y, color); // 4
        sgl_draw_point(disp, xc - x, yc - y, color); // 5
        sgl_draw_point(disp, xc + x, yc - y, color); // 8
    }
    x = 0;
    y = ry;
    ychg = (1 - ry - ry) * (rxrx2 >> 1);
    xchg = ryry2 >> 1;
    err = 0;
    stopy = rxrx2 * ry;
    stopx = 0;
    while (1) {
        ++x;
        stopx += ryry2;
        err += xchg;
        xchg += ryry2;
        if (2 * err + ychg > 0) {
            --y;
            stopy -= rxrx2;
            err += ychg;
            ychg += rxrx2;
        }
        if (stopx >= stopy)
            break;
        sgl_draw_point(disp, xc + x, yc + y, color); // 2
        sgl_draw_point(disp, xc - x, yc + y, color); // 3
        sgl_draw_point(disp, xc - x, yc - y, color); // 6
        sgl_draw_point(disp, xc + x, yc - y, color); // 7
    }
    if (stopx == stopy) {
        sgl_draw_point(disp, xc + x, yc + y, color); // 1, 2
        sgl_draw_point(disp, xc - x, yc + y, color); // 3, 4
        sgl_draw_point(disp, xc - x, yc - y, color); // 5, 6
        sgl_draw_point(disp, xc + x, yc - y, color); // 7, 8
    }
}

void sgl_draw_filled_ellipse_section(sgl_display_t *disp, int32_t xc,
                                     int32_t yc, int32_t rx, int32_t ry,
                                     uint32_t color) {
    int32_t x = 0, y = ry, d1, d2;
    int32_t rx_2 = rx * rx;
    int32_t ry_2 = ry * ry;
    d1 = 4 * ry_2 + rx_2 * (1 - 4 * ry);
    while (2 * ry_2 * (x + 1) < rx_2 * (2 * y - 1)) {
        if (d1 <= 0) {
            d1 += 4 * ry_2 * (2 * x + 3);
            ++x;
            sgl_draw_point(disp, xc + x, yc + y, color);
            sgl_draw_point(disp, xc + x, yc - y, color);
            sgl_draw_point(disp, xc - x, yc + y, color);
            sgl_draw_point(disp, xc - x, yc - y, color);
        } else {
            d1 += 4 * (ry_2 * (2 * x + 3) + rx_2 * (-2 * y + 2));
            ++x;
            --y;
            sgl_draw_hline(disp, xc + 1, yc + y, x, color);
            sgl_draw_hline(disp, xc + 1, yc - y, x, color);
            sgl_draw_hline(disp, xc - x, yc + y, x, color);
            sgl_draw_hline(disp, xc - x, yc - y, x, color);
        }
    }
    d2 = ry_2 * (2 * x + 1) * (2 * x + 1) +
         4 * (rx_2 * (y - 1) * (y - 1) - rx_2 * ry_2);
    while (y > 0) {
        if (d2 <= 0) {
            d2 += 4 * (ry_2 * (2 * x + 2) + rx_2 * (-2 * y + 3));
            ++x;
            --y;
        } else {
            d2 += 4 * (rx_2 * (-2 * y + 3));
            --y;
        }
        sgl_draw_hline(disp, xc + 1, yc + y, x, color);
        sgl_draw_hline(disp, xc + 1, yc - y, x, color);
        sgl_draw_hline(disp, xc - x, yc + y, x, color);
        sgl_draw_hline(disp, xc - x, yc - y, x, color);
    }
}

void sgl_draw_ellipse(sgl_display_t *disp, int32_t xc, int32_t yc, int32_t rx,
                      int32_t ry, int32_t is_filled, uint32_t color) {
    if (rx < 0)
        rx = -rx;
    if (ry < 0)
        ry = -ry;
    if (sgl_check_area(&disp->drawable_area, xc - rx, yc - ry, xc + rx,
                       yc + ry))
        return;
    if (is_filled == 0 && rx > 0 && ry > 0) {
        sgl_draw_point(disp, xc + rx, yc, color); // 1, 2
        sgl_draw_point(disp, xc, yc + ry, color); // 3, 4
        sgl_draw_point(disp, xc - rx, yc, color); // 5, 6
        sgl_draw_point(disp, xc, yc - ry, color); // 7, 8
        sgl_draw_ellipse_section(disp, xc, yc, rx, ry, color);
    } else {
        sgl_draw_point(disp, xc, yc, color);          // (xc, yc)
        sgl_draw_hline(disp, xc + 1, yc, rx, color);  // 8, 1
        sgl_draw_vline(disp, xc, yc + 1, ry, color);  // 2, 3
        sgl_draw_hline(disp, xc - 1, yc, -rx, color); // 4, 5
        sgl_draw_vline(disp, xc, yc - 1, -ry, color); // 6, 7
        sgl_draw_filled_ellipse_section(disp, xc, yc, rx, ry, color);
    }
}
