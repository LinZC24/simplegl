#ifndef SGL_OBJ_H
#define SGL_OBJ_H

#include <stdint.h>
#include "../draw/sgl_draw.h" // 需要颜色定义

// 前向声明，允许结构体内部引用自己
typedef struct _sgl_obj_t sgl_obj_t;

// 定义绘制回调函数的类型
// 参数: obj - 当前要绘制的对象
typedef void (*sgl_event_cb_t)(sgl_obj_t *obj);

struct _sgl_obj_t {
    // 1. 层级关系
    sgl_obj_t *parent;          // 父对象
    sgl_obj_t **children;       // 子对象数组 (简化版，也可以用链表)
    uint16_t child_cnt;         // 子对象数量

    // 2. 几何属性 (相对于父对象的坐标)
    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;

    // 3. 视觉属性
    sgl_color_t color;          // 背景色

    // 4. 行为/多态
    sgl_event_cb_t draw_cb;     // 绘制回调：核心层调用它来画出具体的样子
};

/**
 * @brief 创建一个基础对象
 * @param parent 父对象 (如果是屏幕根对象，则为 NULL)
 * @return 新创建的对象指针
 */
sgl_obj_t *sgl_obj_create(sgl_obj_t *parent);

/**
 * @brief 设置位置
 */
void sgl_obj_set_pos(sgl_obj_t *obj, int x, int y);

/**
 * @brief 设置大小
 */
void sgl_obj_set_size(sgl_obj_t *obj, int w, int h);

/**
 * @brief 设置背景色
 */
void sgl_obj_set_bg_color(sgl_obj_t *obj, sgl_color_t color);

/**
 * @brief 设置绘制回调 (用于自定义控件外观)
 */
void sgl_obj_set_draw_cb(sgl_obj_t *obj, sgl_event_cb_t draw_cb);

#endif // SGL_OBJ_H