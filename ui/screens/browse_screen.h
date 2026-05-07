/** @file browse_screen.h 文件浏览器页面 (Phase 6) */
#ifndef BROWSE_SCREEN_H
#define BROWSE_SCREEN_H
#include "base_screen.h"
typedef struct { base_screen_t base; } browse_screen_t;
void browse_screen_init(browse_screen_t *self);
#endif
