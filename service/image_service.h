#ifndef IMAGE_SERVICE_H
#define IMAGE_SERVICE_H

#include "lvgl/lvgl.h"

int image_service_init(void);
lv_img_dsc_t *image_load(const char *path);
void image_preload(const char *path);
void image_cancel_preload(const char *path);

#endif
