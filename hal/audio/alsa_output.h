/**
 * @file    alsa_output.h
 * @brief   ALSA PCM 音频输出 — audio_output_t 的具体实现
 *
 * Phase 7 将实现完整的 ALSA 播放逻辑。
 */

#ifndef ALSA_OUTPUT_H
#define ALSA_OUTPUT_H

#include "audio_output.h"

typedef struct {
    audio_output_t base;        /**< 父类 (vtable) */
    void *pcm;                  /**< snd_pcm_t* PCM 句柄 (void* 避免头文件依赖) */
} alsa_output_t;

void alsa_output_init(alsa_output_t *self);

#endif
