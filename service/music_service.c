/**
 * @file    music_service.c
 * @brief   音乐服务实现
 *
 * 当前为 Phase 2 桩代码。Phase 7 将实现:
 *   1. play()  — 启动 MP3 解码线程, 通过 ALSA 播放
 *   2. pause() / resume() — 暂停/恢复解码
 *   3. stop()  — 停止播放, 释放资源
 *   4. 回调注册 — 播放进度/状态变化通知 UI
 */

#include "music_service.h"

int music_service_init(void)        { return 0; }
int music_play(const char *path)    { (void)path; return 0; }
int music_pause(void)               { return 0; }
int music_resume(void)              { return 0; }
int music_stop(void)                { return 0; }
music_state_t music_get_state(void) { return MUSIC_STATE_STOPPED; }
