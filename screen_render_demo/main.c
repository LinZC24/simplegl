#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include "../src/core/sgl_core.h"
#include "../src/core/sgl_obj.h"
#include "../src/hal/sgl_hal.h"

// 状态定义：左上-左下-右上-右下-左上
typedef enum {
    PHASE_LEFT_DOWN,   // 1. 左上 -> 左下 (垂直)
    PHASE_DIAG_UP,     // 2. 左下 -> 右上 (斜线)
    PHASE_RIGHT_DOWN,  // 3. 右上 -> 右下 (垂直)
    PHASE_DIAG_DOWN    // 4. 右下 -> 左上 (斜线)
} move_phase_t;

// 简单的命中测试
bool is_point_in_rect(int px, int py, sgl_obj_t *obj) {
    return (px >= obj->x && px < obj->x + obj->w &&
            py >= obj->y && py < obj->y + obj->h);
}

// 定义活动区域边界
int min_x = 10;
int min_y = 10;
int max_x = 740; // 800 - 50(box宽) - 10(边距)
int max_y = 420; // 480 - 50(box高) - 10(边距)

int main(int argc, char **argv) {
    if (sgl_init() != 0) return -1;

    sgl_obj_t *scr = sgl_get_scr_act();
    sgl_obj_set_color(scr, 0xFFFF); // 背景白色

    // 创建移动的矩形块
    sgl_obj_t *box = sgl_obj_create(scr);
    
    // 使用 float 记录精确位置，以便处理斜线移动
    float pos_x = (float)min_x;
    float pos_y = (float)min_y;
    
    sgl_obj_set_pos(box, (int)pos_x, (int)pos_y);
    sgl_obj_set_size(box, 50, 50);
    sgl_obj_set_color(box, 0x07E0); // 绿色

    move_phase_t phase = PHASE_LEFT_DOWN;
    bool is_paused = false;
    bool last_pressed = false;

    float speed = 3.0f; 

    // 预计算斜线移动的步长向量
    // 矩形活动区域宽 W = 730, 高 H = 410
    // 对角线长度 D = sqrt(730^2 + 410^2) ≈ 837.25
    // 单位向量 X分量 = 730 / 837.25 ≈ 0.872
    // 单位向量 Y分量 = 410 / 837.25 ≈ 0.490
    float step_diag_x = 0.872f * speed;
    float step_diag_y = 0.490f * speed;

    while(1) {
        // 1. 处理输入
        int tx, ty, pressed;
        sgl_get_indev(&tx, &ty, &pressed);
        
        // 当前整数坐标用于碰撞检测
        int cur_x = (int)pos_x;
        int cur_y = (int)pos_y;

        if (pressed) {
            // printf("DEBUG: Touch at %d, %d\n", tx, ty);
        }

        // 检测点击事件
        if (pressed && !last_pressed) {
            if (is_paused) {
                is_paused = false;
                sgl_obj_set_color(box, 0x07E0); // 恢复绿色
                printf("Resumed!\n");
            } else {
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
            switch (phase) {
                case PHASE_LEFT_DOWN: // 左上 -> 左下
                    pos_y += speed;
                    if (pos_y >= max_y) {
                        pos_y = max_y;
                        phase = PHASE_DIAG_UP;
                    }
                    break;

                case PHASE_DIAG_UP: // 左下 -> 右上
                    pos_x += step_diag_x;
                    pos_y -= step_diag_y;
                    // 简单的到达判断
                    if (pos_x >= max_x || pos_y <= min_y) {
                        pos_x = max_x;
                        pos_y = min_y;
                        phase = PHASE_RIGHT_DOWN;
                    }
                    break;

                case PHASE_RIGHT_DOWN: // 右上 -> 右下
                    pos_y += speed;
                    if (pos_y >= max_y) {
                        pos_y = max_y;
                        phase = PHASE_DIAG_DOWN;
                    }
                    break;

                case PHASE_DIAG_DOWN: // 右下 -> 左上
                    pos_x -= step_diag_x;
                    pos_y -= step_diag_y;
                    if (pos_x <= min_x || pos_y <= min_y) {
                        pos_x = min_x;
                        pos_y = min_y;
                        phase = PHASE_LEFT_DOWN;
                    }
                    break;
            }
            sgl_obj_set_pos(box, (int)pos_x, (int)pos_y);
        }

        // 3. 刷新屏幕
        sgl_refresh();
        
        // 控制帧率
        usleep(16000); 
    }

    sgl_deinit();
    return 0;
}
    