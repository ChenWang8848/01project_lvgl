#ifndef INTERVAL_SCREEN_H
#define INTERVAL_SCREEN_H
#include "base_screen.h"
typedef struct { base_screen_t base; } interval_screen_t;
void interval_screen_init(interval_screen_t *self);
#endif
