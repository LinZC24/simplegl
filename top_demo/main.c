#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "../src/core/sgl_core.h"
#include "../src/core/sgl_obj.h"
#include "../src/draw/sgl_draw.h"
#include "../src/font/sgl_font.h"

// --- 系统信息获取部分 ---

static float g_cpu_usage = 0.0f;
static long g_mem_total = 0;
static long g_mem_free = 0;
static long g_mem_available = 0;

// 读取 CPU 使用率
void update_cpu_usage() {
    static unsigned long long prev_user = 0, prev_nice = 0, prev_system = 0, prev_idle = 0, prev_iowait = 0, prev_irq = 0, prev_softirq = 0, prev_steal = 0;
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
    
    FILE *fp = fopen("/proc/stat", "r");
    if (!fp) return;

    char buff[256];
    if (fgets(buff, sizeof(buff), fp)) {
        sscanf(buff, "cpu  %llu %llu %llu %llu %llu %llu %llu %llu",
               &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal);
    }
    fclose(fp);

    unsigned long long prev_idle_total = prev_idle + prev_iowait;
    unsigned long long idle_total = idle + iowait;

    unsigned long long prev_non_idle = prev_user + prev_nice + prev_system + prev_irq + prev_softirq + prev_steal;
    unsigned long long non_idle = user + nice + system + irq + softirq + steal;

    unsigned long long prev_total = prev_idle_total + prev_non_idle;
    unsigned long long total = idle_total + non_idle;

    unsigned long long total_d = total - prev_total;
    unsigned long long idle_d = idle_total - prev_idle_total;

    if (total_d > 0) {
        g_cpu_usage = (float)(total_d - idle_d) / total_d * 100.0f;
    }

    prev_user = user; prev_nice = nice; prev_system = system; prev_idle = idle;
    prev_iowait = iowait; prev_irq = irq; prev_softirq = softirq; prev_steal = steal;
}

// 读取内存信息 (单位 KB)
void update_mem_usage() {
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp) return;

    char buff[256];
    long mem_total = 0, mem_available = 0;
    
    while (fgets(buff, sizeof(buff), fp)) {
        if (strncmp(buff, "MemTotal:", 9) == 0) {
            sscanf(buff, "MemTotal: %ld kB", &mem_total);
        } else if (strncmp(buff, "MemAvailable:", 13) == 0) {
            sscanf(buff, "MemAvailable: %ld kB", &mem_available);
        }
    }
    fclose(fp);

    g_mem_total = mem_total;
    g_mem_available = mem_available;
    g_mem_free = mem_available; // 简化处理
}

// --- SimpleGL 渲染部分 ---

// 自定义绘制回调：显示数据
static void monitor_draw_cb(sgl_obj_t *obj, int abs_x, int abs_y) {
    char buf[64];
    int line_height = 24;
    int start_x = abs_x + 10;
    int start_y = abs_y + 10;

    // 1. 绘制标题
    sgl_draw_string(start_x, start_y, "=== System Monitor ===", &sgl_font_ascii_16, 0x0000); // 黑色

    // 2. 绘制 CPU 信息
    // 根据负载变色：低负载绿色，高负载红色
    sgl_color_t cpu_color = (g_cpu_usage > 80.0f) ? 0xF800 : 0x07E0; // Red or Green
    snprintf(buf, sizeof(buf), "CPU Usage: %.1f %%", g_cpu_usage);
    sgl_draw_string(start_x, start_y + line_height * 2, buf, &sgl_font_ascii_16, cpu_color);

    // 绘制 CPU 进度条边框
    sgl_draw_rect(start_x, start_y + line_height * 3, 200, 15, 0x0000);
    // 绘制 CPU 进度条填充
    int bar_w = (int)(200 * (g_cpu_usage / 100.0f));
    if (bar_w > 200) bar_w = 200;
    sgl_draw_fill_rect(start_x + 1, start_y + line_height * 3 + 1, bar_w, 13, cpu_color);


    // 3. 绘制内存信息
    long used = g_mem_total - g_mem_available;
    float mem_pct = (float)used / g_mem_total * 100.0f;
    
    snprintf(buf, sizeof(buf), "Memory: %ld / %ld MB", used / 1024, g_mem_total / 1024);
    sgl_draw_string(start_x, start_y + line_height * 5, buf, &sgl_font_ascii_16, 0x001F); // Blue

    snprintf(buf, sizeof(buf), "Mem Usage: %.1f %%", mem_pct);
    sgl_draw_string(start_x, start_y + line_height * 6, buf, &sgl_font_ascii_16, 0x001F);

    // 绘制内存进度条
    sgl_draw_rect(start_x, start_y + line_height * 7, 200, 15, 0x0000);
    bar_w = (int)(200 * (mem_pct / 100.0f));
    if (bar_w > 200) bar_w = 200;
    sgl_draw_fill_rect(start_x + 1, start_y + line_height * 7 + 1, bar_w, 13, 0x001F);
}

int main(int argc, char **argv) {
    // 1. 初始化
    if (sgl_init() != 0) {
        printf("SimpleGL init failed!\n");
        return -1;
    }

    sgl_obj_t *scr = sgl_get_scr_act();
    sgl_obj_set_color(scr, 0xFFFF); // 背景白色

    // 2. 创建一个面板对象用于显示监控信息
    sgl_obj_t *panel = sgl_obj_create(scr);
    sgl_obj_set_pos(panel, 50, 50);
    sgl_obj_set_size(panel, 300, 250);
    sgl_obj_set_color(panel, 0xE71C); // 浅灰色背景 (RGB565)
    
    // 设置自定义绘制回调
    sgl_obj_set_draw_cb(panel, monitor_draw_cb);

    printf("Starting Top Demo...\n");

    int frame_count = 0;

    // 3. 主循环
    while(1) {
        // 每 50 帧 (约 1秒) 更新一次系统数据，避免频繁读取文件
        if (frame_count % 50 == 0) {
            update_cpu_usage();
            update_mem_usage();
            // 标记对象需要重绘 (如果实现了脏矩形)
            // sgl_obj_invalidate(panel); 
        }

        sgl_refresh();
        
        usleep(20000); // 20ms per frame
        frame_count++;
    }

    sgl_deinit();
    return 0;
}