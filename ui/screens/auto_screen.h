/** @file auto_screen.h 幻灯片播放页面 (Phase 6) */
#ifndef AUTO_SCREEN_H
#define AUTO_SCREEN_H
#include "base_screen.h"
typedef struct { base_screen_t base; } auto_screen_t;
void auto_screen_init(auto_screen_t *self);
#endif
