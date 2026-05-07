/**
 * @file    file_browser.c
 * @brief   文件浏览器网格控件实现
 *
 * 当前为 Phase 2 桩代码。Phase 6 将实现:
 *   1. 调用 file_service 遍历目录
 *   2. 创建图标网格 (lv_obj 矩阵布局)
 *   3. 滚动支持
 *   4. 点击事件处理 (进入子目录 / 打开文件)
 */

#include "file_browser.h"

lv_obj_t *file_browser_create(lv_obj_t *parent)
{
    (void)parent;
    return NULL; /* TODO: Phase 6 */
}

void file_browser_set_dir(lv_obj_t *browser, const char *path)
{
    (void)browser;
    (void)path;
    /* TODO: Phase 6 */
}
