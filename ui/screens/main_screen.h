#ifndef MAIN_SCREEN_H
#define MAIN_SCREEN_H

#include "base_screen.h"

typedef struct {
    base_screen_t base;
    lv_obj_t *label;
} main_screen_t;

void main_screen_init(main_screen_t *self);

#endif
