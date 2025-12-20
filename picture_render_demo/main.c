#include <stdio.h>
#include <unistd.h>
#include "../src/core/sgl_core.h"
#include "../src/core/sgl_obj.h"
#include "../src/draw/sgl_draw.h"
#include "../src/utils/img_converter.h"

/* 
 * 辅助函数：创建图片对象 
 * (如果 src/core/sgl_obj.c 中没有实现 sgl_img_create，这里提供一个本地实现)
 */
static void _local_img_draw_cb(sgl_obj_t *obj, int abs_x, int abs_y) {
    const sgl_img_dsc_t *img = (const sgl_img_dsc_t *)obj->user_data;
    if (img) {
        sgl_draw_image(abs_x, abs_y, img);
    }
}

sgl_obj_t *create_image_obj(sgl_obj_t *parent, const sgl_img_dsc_t *img_dsc) {
    sgl_obj_t *obj = sgl_obj_create(parent);
    if (obj) {
        sgl_obj_set_size(obj, img_dsc->w, img_dsc->h);
        // 假设 sgl_obj_t 已经添加了 void *user_data 字段
        obj->user_data = (void*)img_dsc; 
        sgl_obj_set_draw_cb(obj, _local_img_draw_cb);
    }
    return obj;
}

int main(int argc, char **argv) {
    // 1. 初始化 SimpleGL
    if (sgl_init() != 0) {
        printf("SimpleGL init failed!\n");
        return -1;
    }

    sgl_obj_t *scr = sgl_get_scr_act();
    sgl_obj_set_color(scr, 0xFFFF); // 背景白色

    printf("SimpleGL Picture Render Demo\n");

    // 2. 动态加载图片 (需要文件系统中有 test.png)
    // 请确保运行目录下有 test.png，或者提供绝对路径
    const char *img_path = "test.png"; 
    sgl_img_dsc_t my_img;
    bool img_loaded = false;

    if (sgl_img_load_from_file(img_path, &my_img)) {
        printf("Image loaded: %dx%d\n", my_img.w, my_img.h);
        
        // 创建图片对象
        sgl_obj_t *img_obj = create_image_obj(scr, &my_img);
        
        // 居中显示
        int screen_w = 800; // 假设屏幕宽
        int screen_h = 480; // 假设屏幕高
        int x = (screen_w - my_img.w) / 2;
        int y = (screen_h - my_img.h) / 2;
        
        sgl_obj_set_pos(img_obj, x, y);
        
        img_loaded = true;
    } else {
        printf("Failed to load image: %s\n", img_path);
        printf("Please place a 'test.png' file in the current directory.\n");
        
        // 如果加载失败，显示一个红色方块代替
        sgl_obj_t *err_box = sgl_obj_create(scr);
        sgl_obj_set_pos(err_box, 350, 200);
        sgl_obj_set_size(err_box, 100, 100);
        sgl_obj_set_color(err_box, 0xF800); // Red
    }

    // 3. 主循环
    while(1) {
        sgl_refresh();
        usleep(20000); // 2
        // 4. 清理资源
        if (img_loaded) {
            sgl_img_free(&my_img);
        }
        sgl_deinit();
      }
    return 0;
}    