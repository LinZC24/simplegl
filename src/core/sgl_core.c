#include "sgl_core.h"
#include "../hal/sgl_hal.h"
#include <stdlib.h>
#include <stddef.h>

// 全局变量：当前活动的屏幕对象
static sgl_obj_t *act_scr = NULL;

int sgl_init(void) {
    // 1. 初始化硬件层
    if (sgl_hal_init() != 0) {
        return -1;
    }

    // 2. 创建根对象 (屏幕)
    // 屏幕没有父对象
    act_scr = sgl_obj_create(NULL);
    if (!act_scr) {
        sgl_hal_deinit();
        return -1;
    }

    // 3. 设置屏幕默认属性
    sgl_obj_set_size(act_scr, sgl_hal_get_width(), sgl_hal_get_height());
    sgl_obj_set_pos(act_scr, 0, 0);
    sgl_obj_set_color(act_scr, 0xFF000000); // 默认黑色背景

    return 0;
}

// 辅助函数：递归释放对象及其所有子对象
static void _sgl_obj_del_recursive(sgl_obj_t *obj) {
    if (!obj) return;

    // 1. 先递归删除所有子对象
    sgl_obj_t *child = obj->child;
    while (child) {
        sgl_obj_t *next = child->next; // 先保存下一个兄弟，防止释放后找不到
        _sgl_obj_del_recursive(child);
        child = next;
    }

    // 2. 释放当前对象
    free(obj);
}

void sgl_deinit(void) {
    // 1. 释放对象树
    if (act_scr) {
        _sgl_obj_del_recursive(act_scr);
        act_scr = NULL;
    }

    // 2. 关闭硬件层
    sgl_hal_deinit();
}

sgl_obj_t *sgl_get_scr_act(void) {
    return act_scr;
}

// 辅助函数：递归渲染对象
static void _sgl_render_obj(sgl_obj_t *obj, int parent_abs_x, int parent_abs_y) {
    if (!obj) return;

    // 1. 计算绝对坐标
    int abs_x = parent_abs_x + obj->x;
    int abs_y = parent_abs_y + obj->y;

    // 2. 调用对象的绘制回调
    if (obj->draw_cb) {
        obj->draw_cb(obj, abs_x, abs_y);
    }

    // 3. 递归绘制子对象 (子对象覆盖在父对象之上)
    sgl_obj_t *child = obj->child;
    while (child) {
        _sgl_render_obj(child, abs_x, abs_y);
        child = child->next;
    }
}

void sgl_refresh(void) {
    if (!act_scr) return;

    // 从屏幕根节点开始渲染
    // 屏幕的父坐标视为 (0,0)
    _sgl_render_obj(act_scr, 0, 0);

    sgl_hal_flush();
}