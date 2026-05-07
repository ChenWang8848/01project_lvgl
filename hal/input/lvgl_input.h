#ifndef LVGL_INPUT_H
#define LVGL_INPUT_H

#include "lvgl/lvgl.h"
#include "input_driver.h"

int  lvgl_input_register(input_driver_t *drv, lv_indev_type_t type);
void lvgl_input_init_all(void);

#endif
