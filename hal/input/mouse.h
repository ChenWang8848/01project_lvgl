#ifndef MOUSE_H
#define MOUSE_H

#include "input_driver.h"

typedef struct {
    input_driver_t base;
    int fd;
} mouse_t;

void mouse_init(mouse_t *self, const char *dev_path);

#endif
