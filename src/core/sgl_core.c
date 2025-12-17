#include "sgl_core.h"
#include <stdlib.h>
#include <string.h>

// 全局上下文实例
sgl_context_t* current_context = NULL;

sgl_context_t* sgl_create_context(int width, int height) {
    sgl_context_t* ctx = (sgl_context_t*)malloc(sizeof(sgl_context_t));
    if (!ctx) return NULL;

    ctx->width = width;
    ctx->height = height;

    // 分配颜色缓冲区 (RGBA)
    ctx->color_buffer = (sgl_color_t*)malloc(width * height * sizeof(sgl_color_t));
    // 分配深度缓冲区 (Float)
    ctx->depth_buffer = (sgl_depth_t*)malloc(width * height * sizeof(sgl_depth_t));

    if (!ctx->color_buffer || !ctx->depth_buffer) {
        sgl_destroy_context(ctx);
        return NULL;
    }

    // 初始化视口矩阵
    // 临时切换上下文以复用 sgl_viewport 逻辑
    sgl_context_t* old_ctx = current_context;
    current_context = ctx;
    sgl_viewport(0, 0, width, height);
    current_context = old_ctx;

    return ctx;
}

void sgl_destroy_context(sgl_context_t* ctx) {
    if (!ctx) return;

    if (ctx->color_buffer) free(ctx->color_buffer);
    if (ctx->depth_buffer) free(ctx->depth_buffer);

    if (current_context == ctx) {
        current_context = NULL;
    }

    free(ctx);
}

void sgl_set_current_context(sgl_context_t* ctx) {
    current_context = ctx;
}

void sgl_clear_color(sgl_color_t color) {
    if (!current_context || !current_context->color_buffer) return;

    size_t pixel_count = current_context->width * current_context->height;
    
    // 简单的循环赋值
    for (size_t i = 0; i < pixel_count; ++i) {
        current_context->color_buffer[i] = color;
    }
}

void sgl_clear_depth(float depth) {
    if (!current_context || !current_context->depth_buffer) return;

    size_t pixel_count = current_context->width * current_context->height;
    
    for (size_t i = 0; i < pixel_count; ++i) {
        current_context->depth_buffer[i] = depth;
    }
}

void sgl_clear(sgl_color_t color, float depth) {
    sgl_clear_color(color);
    sgl_clear_depth(depth);
}

void sgl_viewport(int x, int y, int width, int height) {
    if (!current_context) return;

    float w = (float)width / 2.0f;
    float h = (float)height / 2.0f;

    // 构建视口变换矩阵
    // 映射 NDC [-1, 1] -> Screen [x, x+w], [y, y+h]
    // 映射 Z [-1, 1] -> [0, 1]
    
    // 假设 mat4x4 结构体有 m[4][4] 成员
    // 初始化为 0
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            current_context->viewport_matrix.m[i][j] = 0.0f;
        }
    }

    // 对角线缩放
    current_context->viewport_matrix.m[0][0] = w;
    current_context->viewport_matrix.m[1][1] = h;
    current_context->viewport_matrix.m[2][2] = 0.5f;
    current_context->viewport_matrix.m[3][3] = 1.0f;

    // 平移分量 (第4列)
    current_context->viewport_matrix.m[0][3] = (float)x + w;
    current_context->viewport_matrix.m[1][3] = (float)y + h;
    current_context->viewport_matrix.m[2][3] = 0.5f;
}

sgl_color_t* sgl_get// filepath: /home/student/finalProj/simplegl/src/core/sgl_core.c
#include "sgl_core.h"
#include <stdlib.h>
#include <string.h>

// 全局上下文实例
sgl_context_t* current_context = NULL;

sgl_context_t* sgl_create_context(int width, int height) {
    sgl_context_t* ctx = (sgl_context_t*)malloc(sizeof(sgl_context_t));
    if (!ctx) return NULL;

    ctx->width = width;
    ctx->height = height;

    // 分配颜色缓冲区 (RGBA)
    ctx->color_buffer = (sgl_color_t*)malloc(width * height * sizeof(sgl_color_t));
    // 分配深度缓冲区 (Float)
    ctx->depth_buffer = (sgl_depth_t*)malloc(width * height * sizeof(sgl_depth_t));

    if (!ctx->color_buffer || !ctx->depth_buffer) {
        sgl_destroy_context(ctx);
        return NULL;
    }

    // 初始化视口矩阵
    // 临时切换上下文以复用 sgl_viewport 逻辑
    sgl_context_t* old_ctx = current_context;
    current_context = ctx;
    sgl_viewport(0, 0, width, height);
    current_context = old_ctx;

    return ctx;
}

void sgl_destroy_context(sgl_context_t* ctx) {
    if (!ctx) return;

    if (ctx->color_buffer) free(ctx->color_buffer);
    if (ctx->depth_buffer) free(ctx->depth_buffer);

    if (current_context == ctx) {
        current_context = NULL;
    }

    free(ctx);
}

void sgl_set_current_context(sgl_context_t* ctx) {
    current_context = ctx;
}

void sgl_clear_color(sgl_color_t color) {
    if (!current_context || !current_context->color_buffer) return;

    size_t pixel_count = current_context->width * current_context->height;
    
    // 简单的循环赋值
    for (size_t i = 0; i < pixel_count; ++i) {
        current_context->color_buffer[i] = color;
    }
}

void sgl_clear_depth(float depth) {
    if (!current_context || !current_context->depth_buffer) return;

    size_t pixel_count = current_context->width * current_context->height;
    
    for (size_t i = 0; i < pixel_count; ++i) {
        current_context->depth_buffer[i] = depth;
    }
}

void sgl_clear(sgl_color_t color, float depth) {
    sgl_clear_color(color);
    sgl_clear_depth(depth);
}

void sgl_viewport(int x, int y, int width, int height) {
    if (!current_context) return;

    float w = (float)width / 2.0f;
    float h = (float)height / 2.0f;

    // 构建视口变换矩阵
    // 映射 NDC [-1, 1] -> Screen [x, x+w], [y, y+h]
    // 映射 Z [-1, 1] -> [0, 1]
    
    // 假设 mat4x4 结构体有 m[4][4] 成员
    // 初始化为 0
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            current_context->viewport_matrix.m[i][j] = 0.0f;
        }
    }

    // 对角线缩放
    current_context->viewport_matrix.m[0][0] = w;
    current_context->viewport_matrix.m[1][1] = h;
    current_context->viewport_matrix.m[2][2] = 0.5f;
    current_context->viewport_matrix.m[3][3] = 1.0f;

    // 平移分量 (第4列)
    current_context->viewport_matrix.m[0][3] = (float)x + w;
    current_context->viewport_matrix.m[1][3] = (float)y + h;
    current_context->viewport_matrix.m[2][3] = 0.5f;
}

sgl_color_t* sgl_get_color_buffer() { 
  if (!current_context) return NULL; 
  return current_context->color_buffer; 
}