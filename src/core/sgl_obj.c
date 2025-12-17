#include "sgl_obj.h"
#include <stdlib.h>
#include <stddef.h>

// 默认绘制行为
static void sgl_obj_draw_rect(sgl_obj_t *obj, int abs_x, int abs_y) {
    sgl_draw_fill_rect(abs_x, abs_y, obj->w, obj->h, obj->color);
}

sgl_obj_t *sgl_obj_create(sgl_obj_t *parent) {
    sgl_obj_t *obj = (sgl_obj_t*)malloc(sizeof(sgl_obj_t));
    if (!obj) return NULL;

    obj->parent = parent;
    obj->child = NULL;
    obj->next = NULL;
    
    obj->x = 0; obj->y = 0;
    obj->w = 100; obj->h = 50;
    obj->color = 0xFFFFFFFF;
    obj->draw_cb = sgl_obj_draw_rect;

    if (parent) {
        // 头插法插入子链表
        obj->next = parent->child;
        parent->child = obj;
    }
    return obj;
}

void sgl_obj_set_pos(sgl_obj_t *obj, int x, int y) {
    if(obj) { obj->x = x; obj->y = y; }
}

void sgl_obj_set_size(sgl_obj_t *obj, int w, int h) {
    if(obj) { obj->w = w; obj->h = h; }
}

void sgl_obj_set_color(sgl_obj_t *obj, sgl_color_t color) {
    if(obj) { obj->color = color; }
}

void sgl_obj_set_draw_cb(sgl_obj_t *obj, sgl_draw_cb_t cb) {
    if(obj) { obj->draw_cb = cb; }
}