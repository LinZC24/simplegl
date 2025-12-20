#ifndef SGL_FONT_H
#define SGL_FONT_H

#include <stdint.h>
#include <stdbool.h>

/* 字形描述符：描述单个字符的属性 */
typedef struct {
    uint8_t box_w;       /* 字形的位图宽度 */
    uint8_t box_h;       /* 字形的位图高度 */
    uint8_t adv_w;       /* 字符绘制后的水平步进距离 (通常等于 box_w + 间距) */
    int8_t  ofs_x;       /* X轴偏移 */
    int8_t  ofs_y;       /* Y轴偏移 (用于对齐基线) */
    const uint8_t *bitmap; /* 指向位图数据的指针 */
} sgl_glyph_dsc_t;

/* 字体描述符：描述整个字体 */
typedef struct sgl_font_t {
    uint8_t line_height; /* 行高 */
    uint8_t base_line;   /* 基线位置 */
    
    /* 核心接口：根据字符编码获取字形描述 */
    bool (*get_glyph_dsc)(const struct sgl_font_t *font, sgl_glyph_dsc_t *dsc, char letter);
} sgl_font_t;

/* 声明一个内置的简单字体 */
extern const sgl_font_t sgl_font_ascii_16;

#endif /* SGL_FONT_H */