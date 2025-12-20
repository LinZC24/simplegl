#ifndef SGL_CONF_H
#define SGL_CONF_H

//color settings
#ifndef SGL_COLOR_DEPTH
    /* 选项: 1, 8, 16, 32 */
    #define SGL_COLOR_DEPTH 16
#endif

//memory settings
#ifndef SGL_USE_STDLIB_MALLOC
    /* 1: 使用标准库 malloc/free (Linux/Windows)
     * 0: 使用内部静态内存池 (裸机) */
    #define SGL_USE_STDLIB_MALLOC 1
#endif

//display settings
#ifndef SGL_HOR_RES
    #define SGL_HOR_RES 800
#endif

#ifndef SGL_VER_RES
    #define SGL_VER_RES 480
#endif

// Linux Framebuffer settings
#ifndef SGL_FBDEV_PATH
    #define SGL_FBDEV_PATH "/dev/fb0"
#endif

#ifndef SGL_USE_DOUBLE_BUFFER
    /* 1: 开启双缓冲 (防止撕裂)
     * 0: 直接绘制到显存 */
    #define SGL_USE_DOUBLE_BUFFER 0
#endif

#endif // SGL_CONF_H