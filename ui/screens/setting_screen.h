/** @file setting_screen.h 设置菜单页面 (Phase 6) */
#ifndef SETTING_SCREEN_H
#define SETTING_SCREEN_H
#include "base_screen.h"
typedef struct { base_screen_t base; } setting_screen_t;
void setting_screen_init(setting_screen_t *self);
#endif
