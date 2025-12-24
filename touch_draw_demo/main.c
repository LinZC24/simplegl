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
#define BTN_W    100
#define CANVAS_H (SCREEN_H - BTN_H)

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

// --- 主函数 ---

int main() {
    // 1. 初始化 SimpleGL
    if (sgl_init() != 0) {
        printf("SGL Init failed!\n");
        return -1;
    }
    
    // 2. 获取屏幕根对象
    sgl_obj_t *scr = sgl_get_scr_act();
    // 设置屏幕背景为白色，作为默认画布背景
    sgl_obj_set_color(scr, 0xFFFF); 
    
    // 3. 创建清除按钮对象 (占据屏幕底部)
    sgl_obj_t *btn = sgl_obj_create(scr);
    sgl_obj_set_pos(btn, 0, CANVAS_H);
    sgl_obj_set_size(btn, BTN_W, BTN_H);
    sgl_obj_set_color(btn, 0xF800); // 红色 (RGB565)
    sgl_obj_set_draw_cb(btn, btn_draw_cb);
    
    // 4. 初始绘制界面
    // 这会将背景(白色)和按钮绘制到 back_buffer，然后 flush 到屏幕
    sgl_refresh();
    
    // 5. 主循环
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
                    // 清除画布区域：直接调用库函数画一个白色矩形覆盖
                    sgl_draw_fill_rect(0, 0, SCREEN_W, CANVAS_H, 0xFFFF);
                    sgl_hal_flush(); // 立即刷新显示
                    printf("Canvas Cleared!\n");
                }
                last_x = -1; // 重置绘图笔触
            } else {
                // 区域：画布
                if (last_x != -1) {
                    // 使用 sgl_draw 提供的画线功能直接绘制到 back_buffer
                    // sgl_draw_line 内部会调用 sgl_hal_draw_pixel
                    sgl_draw_line(last_x, last_y, x, y, 0x0000); // 黑色笔迹
                } else {
                    // 第一个点
                    sgl_draw_point(x, y, 0x0000);
                }
                
                // 立即刷新显存，让线条可见
                sgl_hal_flush();
                
                // 记录当前点作为下一次的起点
                last_x = x;
                last_y = y;
            }
        } else {
            // --- 触摸松开 ---
            last_x = -1;
        }
        
        was_pressed = pressed;
        
        usleep(5000); // 5ms
    }

    // 6. 退出前清理
    sgl_deinit();
    return 0;
}