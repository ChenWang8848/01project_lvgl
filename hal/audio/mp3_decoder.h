/**
 * @file    mp3_decoder.h
 * @brief   MP3 解码器接口 — libmad 封装
 *
 * Phase 7 将实现完整的 libmad 解码流程。
 */

#ifndef MP3_DECODER_H
#define MP3_DECODER_H

#include <stdint.h>

/**
 * @brief MP3 解码器 (独立类, 使用函数指针表)
 */
typedef struct {
    int  (*init)(void *self);       /**< 初始化解码器 */
    int  (*decode)(void *self, const uint8_t *data, uint32_t len,
                   int16_t **pcm, uint32_t *pcm_len);  /**< 解码一帧 MP3 */
    int  (*seek)(void *self, uint32_t pos);             /**< 跳转到指定位置 */
    void (*deinit)(void *self);     /**< 释放解码器资源 */
} mp3_decoder_t;

void mp3_decoder_init(mp3_decoder_t *self);

#endif
