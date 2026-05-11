/**
 * @file    auto_screen.h
 * @brief   图片查看器 — 等比缩放显示, 上一张/下一张浏览
 */
#ifndef AUTO_SCREEN_H
#define AUTO_SCREEN_H
#include "base_screen.h"

#define MAX_IMAGES 512

typedef struct {
    base_screen_t base;
    lv_obj_t *title_bar, *title_label, *back_btn;
    lv_obj_t *bottom_bar, *btn_prev, *btn_next, *page_label;
    lv_obj_t *img_obj;

    /* 图片列表 */
    char image_list[MAX_IMAGES][512];  /**< 当前目录所有图片的完整路径 */
    int  total_images;                  /**< 图片总数 */
    int  cur_index;                     /**< 当前索引 (0-based) */

    char cur_file[512];
} auto_screen_t;

void auto_screen_init(auto_screen_t *self);

/**
 * @brief 设置图片列表并打开指定文件
 * @param list      图片路径数组 (完整路径)
 * @param count     图片数量
 * @param focus_idx 初始显示的图片索引 (0-based)
 */
void auto_screen_open_images(auto_screen_t *self,
                              const char (*list)[512],
                              int count, int focus_idx);

extern auto_screen_t *g_auto_screen;

#endif
