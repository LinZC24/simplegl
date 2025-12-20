#ifndef SGL_OBJ_H
#define SGL_OBJ_H

#include <stdint.h>
#include "../draw/sgl_draw.h"

typedef struct _sgl_obj_t sgl_obj_t;

// 绘制回调函数定义：传入对象指针和计算好的绝对坐标
typedef void (*sgl_draw_cb_t)(sgl_obj_t *obj, int abs_x, int abs_y);

struct _sgl_obj_t {
    sgl_obj_t *parent;  // 父对象
    sgl_obj_t *child;   // 子对象链表头
    sgl_obj_t *next;    // 下一个兄弟节点

    int x, y;           // 相对父对象的坐标
    int w, h;           // 尺寸
    sgl_color_t color;  // 基础颜色
    
    sgl_draw_cb_t draw_cb; // 绘制回调
    void *user_data; 
};

sgl_obj_t *sgl_obj_create(sgl_obj_t *parent);
void sgl_obj_set_pos(sgl_obj_t *obj, int x, int y);
void sgl_obj_set_size(sgl_obj_t *obj, int w, int h);
// 注意：这里统一命名为 set_color，之前可能是 set_bg_color
void sgl_obj_set_color(sgl_obj_t *obj, sgl_color_t color);
void sgl_obj_set_draw_cb(sgl_obj_t *obj, sgl_draw_cb_t cb);

void sgl_obj_invalidate(sgl_obj_t *obj);

#endif // SGL_OBJ_H