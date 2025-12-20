#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "../src/core/sgl_core.h"
#include "../src/core/sgl_obj.h"
#include "../src/hal/sgl_hal.h"
#include "../src/draw/sgl_draw.h"
#include "../src/font/sgl_font.h"

// 简单的自定义对象绘制回调，用于绘制文本
// 在 SimpleGL 中，通常对象有一个 draw_cb，这里我们模拟这个机制
// 或者直接在主循环中调用 sgl_draw_string

int main(int argc, char **argv) {
    // 1. 初始化
    if (sgl_init() != 0) {
        printf("SimpleGL init failed!\n");
        return -1;
    }

    sgl_obj_t *scr = sgl_get_scr_act();
    sgl_obj_set_color(scr, 0xFFFF); // 背景白色

    // 动画变量
    int anim_x = 10;
    int anim_y = 300;
    int anim_dir = 1;

    printf("Starting Font Render Demo...\n");

    while(1) {
        // --- 渲染帧开始 ---
        
        // 强制重绘背景
        sgl_obj_invalidate(scr); 
        sgl_refresh(); // 这会绘制背景色

        // --- 直接在显存/缓冲区上绘制文本 ---
        
        // 标题
        sgl_draw_string(20, 20, "SimpleGL Font Demo", &sgl_font_ascii_16, 0x0000); // 黑色
        
        // 数字测试
        sgl_draw_string(20, 50, "Numbers: 0123456789", &sgl_font_ascii_16, 0x0000); // 红色
        
        // 大写字母
        sgl_draw_string(20, 80, "Upper: ABCDEFGHIJKLMNOPQRSTUVWXYZ", &sgl_font_ascii_16, 0x0000); // 绿色
        
        // 小写字母
        sgl_draw_string(20, 110, "Lower: abcdefghijklmnopqrstuvwxyz", &sgl_font_ascii_16, 0x0000); // 蓝色
        
        // 符号
        sgl_draw_string(20, 140, "Symbols: !@#$%^&*()_+-=[]{};':\",./<>?", &sgl_font_ascii_16, 0x0000);
        
        // 移动的文本
        sgl_draw_string(anim_x, anim_y, "Moving Text >>> rendering", &sgl_font_ascii_16, 0x0000); // 紫色

        // 更新动画位置
        anim_x += 2 * anim_dir;
        if (anim_x > 600 || anim_x < 10) anim_dir *= -1;

        // --- 提交显存 ---
        sgl_hal_flush();

        // 控制帧率
        usleep(20000); // 20ms
    }

    sgl_deinit();
    return 0;
}