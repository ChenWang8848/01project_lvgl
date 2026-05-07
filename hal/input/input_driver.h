#ifndef INPUT_DRIVER_H
#define INPUT_DRIVER_H

#include "lvgl/lvgl.h"
#include <stdbool.h>

typedef struct input_driver_s input_driver_t;

struct input_driver_s {
    const char *name;
    int  (*init)(input_driver_t *self);
    bool (*read)(input_driver_t *self, lv_indev_drv_t *drv, lv_indev_data_t *data);
    void (*deinit)(input_driver_t *self);
};

#endif
