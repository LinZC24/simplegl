#include "sgl_hal.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <string.h>

static int fb_fd = 0;
static struct fb_var_screeninfo vinfo;
static struct fb_fix_screeninfo finfo;
static long int screensize = 0;
static char *fbp = 0; //显存指针
static char *back_buffer = 0; //后备缓冲区指针，用于双缓冲模式

int sgl_hal_init(void) {
  // 打开帧缓冲设备
  fb_fd = open(SGL_FBDEV_PATH, O_RDWR);
  if (fb_fd == -1) {
    perror("Error: cannot open framebuffer device");
    return -1;
  }

  // 获取屏幕信息
  if(ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo) == -1) {
    perror("Error reading fixed information");
    return -1;
  }

  // 获取可变屏幕信息
  if(ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
    perror("Error reading variable information");
    return -1;
  }

  // 计算屏幕大小
  screensize = vinfo.xres_virtual * vinfo.yres_virtual * vinfo.bits_per_pixel / 8;

  // 映射显存到用户空间
  fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
  if ((int)fbp == -1) {
    perror("Error: failed to map framebuffer device to memory");
    return -1;
  }

  // 启用双缓冲的解决
#if SGL_USE_DOUBLE_BUFFER
  back_buffer = (char *)malloc(screensize);
  if (!back_buffer) {
    perror("Error: failed to allocate back buffer");
    return -1;
  }
  memset(back_buffer, 0, screensize);
#endif
  printf("Framebuffer initialized: %dx%d, %d bpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);
  return 0;
}

void sgl_hal_deinit(void) {
#if SGL_USE_DOUBLE_BUFFER
  if (back_buffer) {
    free(back_buffer);
    back_buffer = 0;
  }
#endif
  if(fbp && (intptr_t)fbp != -1) {
    munmap(fbp, screensize);
    fbp = NULL;
  }
  if(fb_fd > 0) {
    close(fb_fd);
    fb_fd = 0;
  }
}

void sgl_hal_draw_pixel(int x, int y, sgl_color_t color) {
  if(x < 0 || x >= vinfo.xres || y < 0 || y >= vinfo.yres) {
    return; // 越界检查
  }

  long int location = (x + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) +
                      (y + vinfo.yoffset) * finfo.line_length;
#if SGL_USE_DOUBLE_BUFFER
  // 在后备缓冲区绘制像素
  char *target_buffer = back_buffer;
#else
  // 直接在显存绘制像素
  char *target_buffer = fbp;
#endif  

  *(sgl_color_t *)(target_buffer + location) = color;
  
}

void sgl_hal_flush(void) {
#if SGL_USE_DOUBLE_BUFFER
  // 将后备缓冲区内容复制到显存
  if(fbp && back_buffer) {
    memcpy(fbp, back_buffer, screensize);
  }
#else
  // 非双缓冲模式不需要刷新
#endif
}

int sgl_hal_get_width(void) {
  return vinfo.xres;
}

int sgl_hal_get_height(void) {
  return vinfo.yres;
}