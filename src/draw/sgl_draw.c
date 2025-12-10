#include "sgl_draw.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void sgl_draw_clear(sgl_color_t color) {
    int width = sgl_hal_get_width();
    int height = sgl_hal_get_height();
    for(int y = 0; y < height; y++) {
      for(int x = 0; x < width; x++) {
        sgl_hal_draw_pixel(x, y, color);
      }
    }
}

void sgl_draw_point(int x, int y, sgl_color_t color) {
    sgl_hal_draw_pixel(x, y, color);
}

// 使用 Bresenham 算法绘制直线
void sgl_draw_line(int x1, int y1, int x2, int y2, sgl_color_t color) {
    int dx = abs(x2 - x1);
    int dy = -abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx + dy;
    int e2;

    while (1) {
        sgl_hal_draw_pixel(x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 > dy) {
            err += dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void sgl_draw_rect(int x, int y, int w, int h, sgl_color_t color) {
    sgl_draw_line(x, y, x + w - 1, y, color);
    sgl_draw_line(x, y, x, y + h - 1, color);
    sgl_draw_line(x + w - 1, y, x + w - 1, y + h - 1, color);
    sgl_draw_line(x, y + h - 1, x + w - 1, y + h - 1, color);
}

void sgl_draw_fill_rect(int x, int y, int w, int h, sgl_color_t color) {
    for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) {
            sgl_hal_draw_pixel(x + i, y + j, color);
        }
    }
}

static void plot_circle_points(int cx, int cy, int x, int y, sgl_color_t color) {
    sgl_hal_draw_pixel(cx + x, cy + y, color);
    sgl_hal_draw_pixel(cx - x, cy + y, color);
    sgl_hal_draw_pixel(cx + x, cy - y, color);
    sgl_hal_draw_pixel(cx - x, cy - y, color);
    sgl_hal_draw_pixel(cx + y, cy + x, color);
    sgl_hal_draw_pixel(cx - y, cy + x, color);
    sgl_hal_draw_pixel(cx + y, cy - x, color);
    sgl_hal_draw_pixel(cx - y, cy - x, color);
} 

void sgl_draw_circle(int x, int y, int r, sgl_color_t color) {
    int dx = 0;
    int dy = r;
    int d = 1 - r;

    plot_circle_points(x, y, dx, dy, color);

    while (dx < dy) {
        if (d < 0) {
            d += 2 * dx + 3;
        } else {
            d += 2 * (dx - dy) + 5;
            dy--;
        }
        dx++;
        plot_circle_points(x, y, dx, dy, color);
    }
}

static void draw_circle_line(int x1, int x2, int y, sgl_color_t color) {
    sgl_draw_line(x1, y, x2, y, color);
}

void sgl_draw_fill_circle(int x, int y, int r, sgl_color_t color) {
    int dx = 0;
    int dy = r;
    int d = 1 - r;

    draw_circle_line(x - r, x + r, y, color);

    while (dx < dy) {
        if (d < 0) {
            d += 2 * dx + 3;
        } else {
            d += 2 * (dx - dy) + 5;
            dy--;
        }
        dx++;
        draw_circle_line(x - dx, x + dx, y + dy, color);
        draw_circle_line(x - dx, x + dx, y - dy, color);
        draw_circle_line(x - dy, x + dy, y + dx, color);
        draw_circle_line(x - dy, x + dy, y - dx, color);
    }
}