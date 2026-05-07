#ifndef MANUAL_SCREEN_H
#define MANUAL_SCREEN_H
#include "base_screen.h"
typedef struct { base_screen_t base; } manual_screen_t;
void manual_screen_init(manual_screen_t *self);
#endif
