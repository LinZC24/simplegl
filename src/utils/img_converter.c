#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "../draw/sgl_draw.h"
#include "img_converter.h" // 稍后创建这个头文件
#include <stdlib.h>
#include <stdio.h>

// RGB888 转 RGB565
static uint16_t rgb888_to_rgb565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

/**
 * @brief 从文件系统加载图片并转换为 SimpleGL 格式 (RGB565)
 * 
 * @param filename 图片路径 (支持 JPG, PNG, BMP 等)
 * @param dsc      输出的图片描述符指针
 * @return true    加载成功
 * @return false   加载失败
 */
bool sgl_img_load_from_file(const char *filename, sgl_img_dsc_t *dsc) {
    if (!filename || !dsc) return false;

    int w, h, channels;
    // 强制读取为 3 通道 (RGB)，忽略 Alpha 通道，因为我们目前只支持 RGB565
    unsigned char *data = stbi_load(filename, &w, &h, &channels, 3);
    
    if (!data) {
        printf("SimpleGL: Failed to load image %s\n", filename);
        return false;
    }

    // 计算需要的内存大小 (RGB565 = 2 bytes per pixel)
    size_t data_size = w * h * 2;
    uint8_t *img_data = (uint8_t *)malloc(data_size);
    
    if (!img_data) {
        printf("SimpleGL: Out of memory for image %s\n", filename);
        stbi_image_free(data);
        return false;
    }

    // 转换格式 RGB888 -> RGB565
    for (int i = 0; i < w * h; i++) {
        uint8_t r = data[i*3 + 0];
        uint8_t g = data[i*3 + 1];
        uint8_t b = data[i*3 + 2];
        
        uint16_t color = rgb888_to_rgb565(r, g, b);
        
        // 存入内存 (小端序: 低字节在前)
        img_data[i*2 + 0] = color & 0xFF;
        img_data[i*2 + 1] = (color >> 8) & 0xFF;
    }

    // 填充描述符
    dsc->w = (uint16_t)w;
    dsc->h = (uint16_t)h;
    dsc->data = img_data;
    dsc->has_alpha = false; 

    // 释放 stb 的原始解压 buffer
    stbi_image_free(data);
    
    return true;
}

/**
 * @brief 释放由 sgl_img_load_from_file 分配的内存
 * 
 * @param dsc 图片描述符
 */
void sgl_img_free(sgl_img_dsc_t *dsc) {
    if (dsc && dsc->data) {
        // 因为 data 被定义为 const uint8_t*，释放时需要强转
        free((void*)dsc->data);
        dsc->data = NULL;
        dsc->w = 0;
        dsc->h = 0;
    }
}