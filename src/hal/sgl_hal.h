#ifndef SGL_HAL_H
#define SGL_HAL_H

#include <stdint.h>
#include "../sgl_conf_template.h"

#if SGL_COLOR_DEPTH == 32
    typedef uint32_t sgl_color_t;
#elif SGL_COLOR_DEPTH == 16
    typedef uint16_t sgl_color_t;
#elif SGL_COLOR_DEPTH == 8
    typedef uint8_t sgl_color_t;
#else
    typedef uint8_t sgl_color_t;
#endif

/*
@brief 初始化硬件抽象层
@return 0 成功
*/
int sgl_hal_init(void);

/*
@brief 释放硬件资源
*/
void sgl_hal_deinit(void);

/*
@brief 在指定坐标绘制像素
@param x 横坐标
@param y 纵坐标
@param color 颜色
*/
void sgl_hal_draw_pixel(int x, int y, sgl_color_t color);

/*
@brief 刷新显示缓冲区到屏幕（双缓冲模式）
若非双缓冲模式，此函数不被调用
*/
void sgl_hal_flush(void);

/*
@brief 获取屏幕宽度
*/
int sgl_hal_get_width(void);

/*
@brief 获取屏幕高度
*/
int sgl_hal_get_height(void);

#endif