#include "sgl_draw.h"
#include "../hal/sgl_hal.h"
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

void sgl_draw_char(int x, int y, char letter, const sgl_font_t *font, sgl_color_t color) {
    sgl_glyph_dsc_t dsc;
    if (!font->get_glyph_dsc(font, &dsc, letter)) return;

    const uint8_t *bitmap = dsc.bitmap;
    for (int row = 0; row < dsc.box_h; row++) {
        uint8_t row_data = bitmap[row];
        for (int col = 0; col < dsc.box_w; col++) {
            // 假设最高位对应最左边的像素 (0x80)
            if (row_data & (0x80 >> col)) {
                sgl_hal_draw_pixel(x + dsc.ofs_x + col, y + dsc.ofs_y + row, color);
            }
        }
    }
}

void sgl_draw_string(int x, int y, const char *str, const sgl_font_t *font, sgl_color_t color) {
    while (*str) {
        sgl_draw_char(x, y, *str, font, color);
        
        sgl_glyph_dsc_t dsc;
        font->get_glyph_dsc(font, &dsc, *str);
        x += dsc.adv_w;
        
        str++;
    }
}

void sgl_draw_image(int x, int y, const sgl_img_dsc_t *img) {
    if (!img || !img->data) return;

    // 将 uint8_t* 转换为 uint16_t* 以便按像素读取 (假设系统是小端序)
    // 注意：如果数据流是字节流且未对齐，直接强转可能在某些架构上有风险，
    // 但在 ARM/x86 Linux 上通常没问题。更安全的做法是按字节组合。
    
    const uint8_t *data_u8 = img->data;

    for (int row = 0; row < img->h; row++) {
        for (int col = 0; col < img->w; col++) {
            // 计算当前像素在数组中的索引 (每个像素2字节)
            int index = (row * img->w + col) * 2;
            
            // 组合两个字节为一个 16位 颜色值 (RGB565)
            // 假设数据存储为 Little Endian: [Low Byte] [High Byte]
            uint16_t color = data_u8[index] | (data_u8[index + 1] << 8);
            
            // 绘制像素
            sgl_hal_draw_pixel(x + col, y + row, color);
        }
    }
}