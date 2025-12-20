#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include "../src/core/sgl_core.h"
#include "../src/core/sgl_obj.h"
#include "../src/hal/sgl_hal.h"


// 状态定义
typedef enum {
    MOVE_DOWN,
    MOVE_RIGHT,
    MOVE_UP,
    MOVE_LEFT
} move_dir_t;

// 简单的命中测试
bool is_point_in_rect(int px, int py, sgl_obj_t *obj) {
    // 注意：这里简化了，假设 obj->x/y 就是绝对坐标（因为父对象是屏幕，屏幕坐标是0,0）
    return (px >= obj->x && px < obj->x + obj->w &&
            py >= obj->y && py < obj->y + obj->h);
}

int min_x = 10;
    int min_y = 10;
    int max_x = 740; // 800 - 50(box宽) - 10(边距)
    int max_y = 420; 

int main(int argc, char **argv) {
    if (sgl_init() != 0) return -1;

    sgl_obj_t *scr = sgl_get_scr_act();
    sgl_obj_set_color(scr, 0xFFFF); // 背景白色

    // 创建移动的矩形块
    sgl_obj_t *box = sgl_obj_create(scr);
    int cur_x = 10, cur_y = 10;
    sgl_obj_set_pos(box, cur_x, cur_y);
    sgl_obj_set_size(box, 50, 50);
    sgl_obj_set_color(box, 0x07E0); // 绿色

    move_dir_t dir = MOVE_DOWN;
    bool is_paused = false;
    bool last_pressed = false; // 用于检测点击边沿

    int speed = 3; // 移动速度

    while(1) {
        // 1. 处理输入
        int tx, ty, pressed;
        sgl_get_indev(&tx, &ty, &pressed);
        if (pressed) {
            printf("DEBUG: App Touch - State: PRESSED, X: %d, Y: %d (Box: %d,%d)\n", 
                   tx, ty, cur_x, cur_y);
        }
        // 检测点击事件 (上升沿：从没按变按下)
        if (pressed && !last_pressed) {
            if (is_paused) {
                // 暂停状态下，点击任意区域恢复
                is_paused = false;
                sgl_obj_set_color(box, 0x07E0); // 恢复绿色
                printf("Resumed!\n");
            } else {
                // 运行状态下，点击矩形块暂停
                if (is_point_in_rect(tx, ty, box)) {
                    is_paused = true;
                    sgl_obj_set_color(box, 0xF800); // 变红表示暂停
                    printf("Paused at (%d, %d)\n", cur_x, cur_y);
                }
            }
        }
        last_pressed = pressed;

        // 2. 处理移动逻辑
        if (!is_paused) {
            switch (dir) {
                case MOVE_DOWN:
                    cur_y += speed;
                    if (cur_y >= max_y) { cur_y = max_y; dir = MOVE_RIGHT; }
                    break;
                case MOVE_RIGHT:
                    cur_x += speed;
                    if (cur_x >= max_x) { cur_x = max_x; dir = MOVE_UP; }
                    break;
                case MOVE_UP:
                    cur_y -= speed;
                    if (cur_y <= min_y) { cur_y = min_y; dir = MOVE_LEFT; }
                    break;
                case MOVE_LEFT:
                    cur_x -= speed;
                    if (cur_x <= min_x) { cur_x = min_x; dir = MOVE_DOWN; }
                    break;
            }
            sgl_obj_set_pos(box, cur_x, cur_y);
        }

        // 3. 刷新屏幕
        sgl_refresh();
        
        // 控制帧率 (约 60FPS)
        usleep(16000); 
    }

    sgl_deinit();
    return 0;
}