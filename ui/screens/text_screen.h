#ifndef TEXT_SCREEN_H
#define TEXT_SCREEN_H
#include "base_screen.h"
typedef struct { base_screen_t base; } text_screen_t;
void text_screen_init(text_screen_t *self);
#endif
