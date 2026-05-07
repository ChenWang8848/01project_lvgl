/** @file music_screen.h 音乐播放器页面 (Phase 7) */
#ifndef MUSIC_SCREEN_H
#define MUSIC_SCREEN_H
#include "base_screen.h"
typedef struct { base_screen_t base; } music_screen_t;
void music_screen_init(music_screen_t *self);
#endif
