#ifndef SGL_CORE_H
#define SGL_CORE_H

#include "sgl_obj.h"
#include "../hal/sgl_hal.h" // 引入 sgl_color_t 定义

/**
 * @brief 初始化 SimpleGL 核心 (包括 HAL)
 * @return 0 成功, 非0 失败
 */
int sgl_init(void);

/**
 * @brief 释放资源
 */
void sgl_deinit(void);

/**
 * @brief 获取当前活动屏幕对象
 */
sgl_obj_t *sgl_get_scr_act(void);

/**
 * @brief 刷新屏幕 (遍历对象树并绘制)
 */
void sgl_refresh(void);

#endif // SGL_CORE_H