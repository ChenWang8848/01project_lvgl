/**
 * @file    music_service.h
 * @brief   音乐服务 — MP3 播放控制状态机
 *
 * Phase 7 将实现完整的音乐播放控制逻辑。
 */

#ifndef MUSIC_SERVICE_H
#define MUSIC_SERVICE_H

/** @brief 音乐播放状态 */
typedef enum {
    MUSIC_STATE_STOPPED,        /**< 已停止 */
    MUSIC_STATE_PLAYING,        /**< 播放中 */
    MUSIC_STATE_PAUSED          /**< 已暂停 */
} music_state_t;

int  music_service_init(void);
int  music_play(const char *path);
int  music_pause(void);
int  music_resume(void);
int  music_stop(void);
music_state_t music_get_state(void);

#endif
