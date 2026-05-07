#ifndef FILE_BROWSER_H
#define FILE_BROWSER_H
#include "lvgl/lvgl.h"
lv_obj_t *file_browser_create(lv_obj_t *parent);
void file_browser_set_dir(lv_obj_t *browser, const char *path);
#endif
