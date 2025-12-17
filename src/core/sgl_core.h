#ifndef SGL_CORE_H
#define SGL_CORE_H

#include <stdint.h>
#include <stdbool.h>
#include "../math/sgl_math.h"

// 颜色缓冲区格式 (RGBA8888)
typedef uint32_t sgl_color_t;

// 深度缓冲区格式
typedef float sgl_depth_t;

// 渲染上下文结构体
typedef struct {
    int width;
    int height;
    sgl_color_t* color_buffer;
    sgl_depth_t* depth_buffer;
    
    // 视口变换矩阵
    mat4x4 viewport_matrix;
    
    // 当前使用的着色器程序 (函数指针或其他抽象)
    // void (*vertex_shader)(...);
    // void (*fragment_shader)(...);
} sgl_context_t;

// 全局上下文指针
extern sgl_context_t* current_context;

/**
 * @brief 初始化 SimpleGL 上下文
 * 
 * @param width 帧缓冲区宽度
 * @param height 帧缓冲区高度
 * @return sgl_context_t* 创建的上下文指针，失败返回 NULL
 */
sgl_context_t* sgl_create_context(int width, int height);

/**
 * @brief 销毁上下文并释放资源
 * 
 * @param ctx 要销毁的上下文
 */
void sgl_destroy_context(sgl_context_t* ctx);

/**
 * @brief 设置当前活动的上下文
 * 
 * @param ctx 上下文指针
 */
void sgl_set_current_context(sgl_context_t* ctx);

/**
 * @brief 清除颜色缓冲区
 * 
 * @param color 清除使用的颜色
 */
void sgl_clear_color(sgl_color_t color);

/**
 * @brief 清除深度缓冲区
 * 
 * @param depth 清除使用的深度值 (通常为 1.0)
 */
void sgl_clear_depth(float depth);

/**
 * @brief 同时清除颜色和深度缓冲区
 * 
 * @param color 清除颜色
 * @param depth 清除深度
 */
void sgl_clear(sgl_color_t color, float depth);

/**
 * @brief 设置视口大小并更新视口矩阵
 * 
 * @param x 左下角 x 坐标
 * @param y 左下角 y 坐标
 * @param width 宽度
 * @param height 高度
 */
void sgl_viewport(int x, int y, int width, int height);

/**
 * @brief 获取当前颜色缓冲区的原始指针 (用于输出到文件或屏幕)
 * 
 * @return sgl_color_t* 
 */
sgl_// filepath: /home/student/finalProj/simplegl/include/core/sgl_core.h
#ifndef SGL_CORE_H
#define SGL_CORE_H

#include <stdint.h>
#include <stdbool.h>
#include "../math/sgl_math.h"

// 颜色缓冲区格式 (RGBA8888)
typedef uint32_t sgl_color_t;

// 深度缓冲区格式
typedef float sgl_depth_t;

// 渲染上下文结构体
typedef struct {
    int width;
    int height;
    sgl_color_t* color_buffer;
    sgl_depth_t* depth_buffer;
    
    // 视口变换矩阵
    mat4x4 viewport_matrix;
    
    // 当前使用的着色器程序 (函数指针或其他抽象)
    // void (*vertex_shader)(...);
    // void (*fragment_shader)(...);
} sgl_context_t;

// 全局上下文指针
extern sgl_context_t* current_context;

/**
 * @brief 初始化 SimpleGL 上下文
 * 
 * @param width 帧缓冲区宽度
 * @param height 帧缓冲区高度
 * @return sgl_context_t* 创建的上下文指针，失败返回 NULL
 */
sgl_context_t* sgl_create_context(int width, int height);

/**
 * @brief 销毁上下文并释放资源
 * 
 * @param ctx 要销毁的上下文
 */
void sgl_destroy_context(sgl_context_t* ctx);

/**
 * @brief 设置当前活动的上下文
 * 
 * @param ctx 上下文指针
 */
void sgl_set_current_context(sgl_context_t* ctx);

/**
 * @brief 清除颜色缓冲区
 * 
 * @param color 清除使用的颜色
 */
void sgl_clear_color(sgl_color_t color);

/**
 * @brief 清除深度缓冲区
 * 
 * @param depth 清除使用的深度值 (通常为 1.0)
 */
void sgl_clear_depth(float depth);

/**
 * @brief 同时清除颜色和深度缓冲区
 * 
 * @param color 清除颜色
 * @param depth 清除深度
 */
void sgl_clear(sgl_color_t color, float depth);

/**
 * @brief 设置视口大小并更新视口矩阵
 * 
 * @param x 左下角 x 坐标
 * @param y 左下角 y 坐标
 * @param width 宽度
 * @param height 高度
 */
void sgl_viewport(int x, int y, int width, int height);

/**
 * @brief 获取当前颜色缓冲区的原始指针 (用于输出到文件或屏幕)
 * 
 * @return sgl_color_t* 
 */
sgl_color_t* sgl_get_color_buffer();

#endif 