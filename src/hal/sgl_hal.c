#include "sgl_hal.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdint.h> // 引入 intptr_t
#include <linux/input.h>

static int indev_fd = -1;
static int cur_x = 0;
static int cur_y = 0;
static int cur_pressed = 0;

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 480

static int fb_fd = 0;
static struct fb_var_screeninfo vinfo;
static struct fb_fix_screeninfo finfo;
static long int screensize = 0;
static char *fbp = 0; // 显存指针
static char *back_buffer = NULL; // 后备缓冲区指针 (字节指针)

void sgl_hal_indev_init(void) {
    // 打开输入设备，通常是 event0, event1 等
    // 可以使用 'ls /dev/input/' 查看，或使用 'evtest' 工具确认是哪个设备
    indev_fd = open("/dev/input/event2", O_RDONLY | O_NONBLOCK);
    if (indev_fd < 0) {
        perror("SimpleGL: Failed to open input device (/dev/input/event0)");
    } else {
        printf("SimpleGL: Input device opened successfully.\n");
    }
}

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
  // 修复1: 使用 intptr_t 避免指针转 int 的警告
  if ((intptr_t)fbp == -1) {
    perror("Error: failed to map framebuffer device to memory");
    return -1;
  }

  // 启用双缓冲
  // 修复2: back_buffer 是 char*，malloc 返回 void*，直接赋值即可，或者转为 char*
  back_buffer = (char *)malloc(screensize);
    if (!back_buffer) {
        perror("Error: failed to allocate back buffer");
        return -1;
    }
    // 初始化为全黑
    memset(back_buffer, 0, screensize);
    //初始化触摸屏
    sgl_hal_indev_init();

    printf("HAL Init: %dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);
    return 0;
}

void sgl_hal_deinit(void) {

  if (back_buffer) {
    free(back_buffer);
    back_buffer = NULL;
  }

  if(fbp && (intptr_t)fbp != -1) {
    munmap(fbp, screensize);
    fbp = NULL;
  }
  if(fb_fd > 0) {
    close(fb_fd);
    fb_fd = 0;
  }
  if(indev_fd > 0) {
    close(indev_fd);
    indev_fd = -1;
  }
}

void sgl_hal_draw_pixel(int x, int y, sgl_color_t color) {
  if(x < 0 || x >= vinfo.xres || y < 0 || y >= vinfo.yres) {
    return; // 越界检查
  }

  // 计算字节偏移量
  long int location = (x + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) +
                      (y + vinfo.yoffset) * finfo.line_length;
  
  // 修复3: 根据色深正确写入像素数据 (解决只能写1字节的问题)
  if (vinfo.bits_per_pixel == 32) {
      *((uint32_t*)(back_buffer + location)) = color;
  } else if (vinfo.bits_per_pixel == 16) {
      *((uint16_t*)(back_buffer + location)) = (uint16_t)color;
  } else {
      // 8 bit
      back_buffer[location] = (uint8_t)color;
  }
}

void sgl_hal_flush(void) {
    if (fbp && back_buffer) {
        memcpy(fbp, back_buffer, screensize);
    }
}

int sgl_hal_get_width(void) {
  return vinfo.xres;
}

int sgl_hal_get_height(void) {
  return vinfo.yres;
}

void sgl_get_indev(int *x, int *y, int *pressed) {
    struct input_event ev;
    
    // 如果设备未初始化，尝试初始化一次
    if (indev_fd < 0) {
        sgl_hal_indev_init();
    }

    if (indev_fd >= 0) {
        // 循环读取所有挂起的事件
        while (read(indev_fd, &ev, sizeof(ev)) > 0) {
            if (ev.type == EV_ABS) {
                // 触摸屏绝对坐标
                if (ev.code == ABS_X || ev.code == ABS_MT_POSITION_X) {
                    // 映射 X 轴: 输入 0-1023 -> 屏幕 0-800
                    // 公式: (value / 1024.0) * 800
                    cur_x = (ev.value * SCREEN_WIDTH) / 1024;
                    
                } else if (ev.code == ABS_Y || ev.code == ABS_MT_POSITION_Y) {
                    cur_y = (ev.value * SCREEN_HEIGHT) / 600;
                }
            } else if (ev.type == EV_REL) {
                // 鼠标相对移动
                if (ev.code == REL_X) cur_x += ev.value;
                if (ev.code == REL_Y) cur_y += ev.value;
                
                // 限制边界
                if (cur_x < 0) cur_x = 0;
                if (cur_x >= SCREEN_WIDTH) cur_x = SCREEN_WIDTH - 1;
                if (cur_y < 0) cur_y = 0;
                if (cur_y >= SCREEN_HEIGHT) cur_y = SCREEN_HEIGHT - 1;
            } else if (ev.type == EV_KEY) {
                // 按键/点击事件
                if (ev.code == BTN_TOUCH || ev.code == BTN_LEFT) {
                    cur_pressed = ev.value;
                }
            }
        }
    }
    // 返回当前状态
    if (x) *x= cur_x;
    if (y) *y = cur_y;
    if (pressed) *pressed = cur_pressed;
}