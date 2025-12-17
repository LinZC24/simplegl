#include "sgl_obj.h"
#include <stdlib.h>
#include <stdio.h>

// 默认的绘制函数：画一个矩形
static void sgl_obj_draw_default(sgl_obj_t *obj) {
    // 计算绝对坐标 (简化版：假设只有一层父级，实际需要递归计算)
    int abs_x = obj->x;
    int abs_y = obj->y;
    
    if (obj->parent) {
        abs_x += obj->parent->x;
        abs_y += obj->parent->y;
    }

    // 调用底层 draw 模块画矩形
    sgl_draw_fill_rect(abs_x, abs_y, obj->w, obj->h, obj->color);
}

sgl_obj_t *sgl_obj_create(sgl_obj_t *parent) {
    sgl_obj_t *obj = (sgl_obj_t *)malloc(sizeof(sgl_obj_t));
    if (!obj) return NULL;

    // 初始化属性
    obj->parent = parent;
    obj->children = NULL;
    obj->child_cnt = 0;
    obj->x = 0;
    obj->y = 0;
    obj->w = 100; // 默认大小
    obj->h = 50;
    obj->color = 0xFFFFFFFF; // 默认白色
    obj->draw_cb = sgl_obj_draw_default; // 绑定默认绘制行为

    // 如果有父对象，将自己添加到父对象的子列表中
    if (parent) {
        // 简化实现：这里应该用 realloc 扩容 parent->children 数组
        // 为了演示，暂时略过复杂的链表/数组管理
        // sgl_obj_add_child(parent, obj); 
    }

    return obj;
}

void sgl_obj_set_pos(sgl_obj_t *obj, int x, int y) {
    if (!obj) return;
    obj->x = x;
    obj->y = y;
}

void sgl_obj_set_size(sgl_obj_t *obj, int w, int h) {
    if (!obj) return;
    obj->w = w;
    obj->h = h;
}

void sgl_obj_set_bg_color(sgl_obj_t *obj, sgl_color_t color) {
    if (!obj) return;
    obj->color = color;
}

void sgl_obj_set_draw_cb(sgl_obj_t *obj, sgl_event_cb_t draw_cb) {
    if (!obj) return;
    obj->draw_cb = draw_cb;
}