#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H

#include "input_driver.h"

typedef struct {
    input_driver_t base;
    int fd;
} touchscreen_t;

void touchscreen_init(touchscreen_t *self, const char *dev_path);

#endif
