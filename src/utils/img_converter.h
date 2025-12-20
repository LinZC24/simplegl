#ifndef IMG_CONVERTER_H
#define IMG_CONVERTER_H

#include <stdbool.h>
#include "../draw/sgl_draw.h"

// 动态加载图片文件
bool sgl_img_load_from_file(const char *filename, sgl_img_dsc_t *dsc);

// 释放图片资源
void sgl_img_free(sgl_img_dsc_t *dsc);

#endif // IMG_CONVERTER_H