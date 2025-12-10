#ifndef SGL_DRAW_H
#define SGL_DRAW_H

#include "../hal/sgl_hal.h"

/*
@brief 清空屏幕
*/
void sgl_draw_clear(sgl_color_t color);

/*
@brief 绘制一个点
*/
void sgl_draw_point(int x, int y, sgl_color_t color);

/*
@brief 绘制一条直线
@param x1, y1 起始点
@param x2, y2 终止点
*/
void sgl_draw_line(int x1, int y1, int x2, int y2, sgl_color_t color);

/*
@brief 绘制空心矩形
@param x, y 左上角坐标
@param w, h 矩形宽度和高度
*/
void sgl_draw_rect(int x, int y, int w, int h, sgl_color_t color);

/*
@brief 绘制填充矩形
*/
void sgl_draw_fill_rect(int x, int y, int w, int h, sgl_color_t color);

/*
@brief 绘制空心圆形
@param x, y 圆心坐标
@param r 半径
*/
void sgl_draw_circle(int x, int y, int r, sgl_color_t color);

/*
@brief 绘制填充圆形
*/
void sgl_draw_fill_circle(int x, int y, int r, sgl_color_t color);