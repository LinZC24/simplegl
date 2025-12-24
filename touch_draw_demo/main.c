#include "../src/core/sgl_core.h"
#include "../src/core/sgl_obj.h"
#include "../src/draw/sgl_draw.h"
#include "../src/hal/sgl_hal.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

// 定义画布和按钮区域尺寸
#define SCREEN_W 800
#define SCREEN_H 480
#define BTN_H    80
#define CANVAS_H (SCREEN_H - BTN_H)

// 画布缓冲区 (RGB565格式，每个像素2字节)
static uint8_t *canvas_buf = NULL;

// --- 回调函数 ---

// 画布对象的绘制回调：将内存缓冲区的内容绘制到屏幕
void canvas_draw_cb(sgl_obj_t *obj, int x, int y) {
    if (!canvas_buf) return;
    
    // 构建图像描述符 (假设 sgl_draw.h 中定义了 sgl_img_dsc_t)
    // 如果编译报错提示未定义，请检查 sgl_draw.h
    sgl_img_dsc_t img;
    img.w = obj->w;
    img.h = obj->h;
    img.data = canvas_buf;
    
    // 使用 simplegl 的图片绘制功能将缓冲区显示出来
    sgl_draw_image(x, y, &img);
}

// 清除按钮的绘制回调：绘制红色背景和白色"X"
void btn_draw_cb(sgl_obj_t *obj, int x, int y) {
    // 1. 绘制红色背景
    sgl_draw_fill_rect(x, y, obj->w, obj->h, obj->color);
    
    // 2. 绘制一个白色的 "X" 表示清除
    sgl_color_t fg_color = 0xFFFF; // 白色
    int pad = 25;
    
    // 简单的加粗线绘制 (偏移1像素重绘)
    for(int i=-1; i<=1; i++) {
        sgl_draw_line(x + pad + i, y + pad, x + obj->w - pad + i, y + obj->h - pad, fg_color);
        sgl_draw_line(x + obj->w - pad + i, y + pad, x + pad + i, y + obj->h - pad, fg_color);
    }
    
    // 3. 绘制白色边框
    sgl_draw_rect(x, y, obj->w, obj->h, 0xFFFF);
}

// --- 绘图辅助函数 ---

// 在画布缓冲区上设置像素颜色 (RGB565 小端序)
void canvas_set_pixel(int x, int y, sgl_color_t color) {
    if (x < 0 || x >= SCREEN_W || y < 0 || y >= CANVAS_H) return;
    
    // 计算缓冲区索引 (每个像素2字节)
    int index = (y * SCREEN_W + x) * 2;
    
    canvas_buf[index] = color & 0xFF;         // 低字节
    canvas_buf[index + 1] = (color >> 8) & 0xFF; // 高字节
}

// 在画布缓冲区上画线 (Bresenham算法，保证快速移动时线条连续)
void canvas_draw_line(int x1, int y1, int x2, int y2, sgl_color_t color) {
    int dx = abs(x2 - x1);
    int dy = -abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx + dy;
    int e2;

    while (1) {
        canvas_set_pixel(x1, y1, color);
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

// --- 主函数 ---

int main() {
    // 1. 初始化 SimpleGL
    if (sgl_init() != 0) {
        printf("SGL Init failed!\n");
        return -1;
    }
    
    // 2. 分配画布内存并初始化为白色
    // 大小 = 宽 * 高 * 2字节(RGB565)
    canvas_buf = (uint8_t*)malloc(SCREEN_W * CANVAS_H * 2);
    if (!canvas_buf) {
        printf("Memory allocation failed!\n");
        sgl_deinit();
        return -1;
    }
    memset(canvas_buf, 0xFF, SCREEN_W * CANVAS_H * 2); // 0xFF = 白色
    
    // 3. 获取屏幕根对象
    sgl_obj_t *scr = sgl_get_scr_act();
    
    // 4. 创建画布对象 (占据屏幕上半部分)
    sgl_obj_t *canvas = sgl_obj_create(scr);
    sgl_obj_set_pos(canvas, 0, 0);
    sgl_obj_set_size(canvas, SCREEN_W, CANVAS_H);
    sgl_obj_set_draw_cb(canvas, canvas_draw_cb);
    
    // 5. 创建清除按钮对象 (占据屏幕底部)
    sgl_obj_t *btn = sgl_obj_create(scr);
    sgl_obj_set_pos(btn, 0, CANVAS_H);
    sgl_obj_set_size(btn, SCREEN_W, BTN_H);
    sgl_obj_set_color(btn, 0xF800); // 红色 (RGB565)
    sgl_obj_set_draw_cb(btn, btn_draw_cb);
    
    // 6. 主循环
    int x, y, pressed;
    int last_x = -1, last_y = -1;
    int was_pressed = 0;
    
    printf("Demo started. Touch screen to draw.\n");
    
    while(1) {
        // 获取输入状态
        sgl_get_indev(&x, &y, &pressed);
        
        if (pressed) {
            // --- 触摸按下 ---
            
            if (y >= CANVAS_H) {
                // 区域：按钮
                if (!was_pressed) { // 仅在按下的第一帧触发
                    memset(canvas_buf, 0xFF, SCREEN_W * CANVAS_H * 2); // 清空画布
                    printf("Canvas Cleared!\n");
                }
                last_x = -1; // 重置绘图笔触
            } else {
                // 区域：画布
                if (last_x != -1) {
                    // 如果有上一个点，画线连接，避免断点
                    canvas_draw_line(last_x, last_y, x, y, 0x0000); // 黑色笔迹
                } else {
                    // 第一个点
                    canvas_set_pixel(x, y, 0x0000);
                }
                // 记录当前点作为下一次的起点
                last_x = x;
                last_y = y;
            }
        } else {
            // --- 触摸松开 ---
            last_x = -1;
        }
        
        was_pressed = pressed;
        
        // 刷新屏幕 (会调用所有对象的 draw_cb)
        sgl_refresh();
        
        usleep(5000); // 5ms

      }   
      free(canvas_buf);
      sgl_deinit();
      return 0;
    }