/**
 * @file    mp3_decoder.c
 * @brief   MP3 解码器实现 (libmad 封装)
 *
 * 当前为 Phase 2 桩代码。Phase 7 将实现:
 *   1. mad_stream_init() / mad_frame_init() / mad_synth_init()
 *   2. mad_stream_buffer() 喂入 MP3 数据
 *   3. mad_frame_decode() 解码帧
 *   4. mad_synth_frame() 合成 PCM
 */

#include "mp3_decoder.h"
#include <string.h>

static int  mp3_init(void *s);
static int  mp3_decode(void *s, const uint8_t *data, uint32_t len,
                       int16_t **pcm, uint32_t *pcm_len);
static int  mp3_seek(void *s, uint32_t pos);
static void mp3_deinit(void *s);

void mp3_decoder_init(mp3_decoder_t *self)
{
    memset(self, 0, sizeof(*self));
    self->init   = mp3_init;
    self->decode = mp3_decode;
    self->seek   = mp3_seek;
    self->deinit = mp3_deinit;
}

/* --- 桩实现 --- */
static int mp3_init(void *s) { (void)s; return 0; }
static int mp3_decode(void *s, const uint8_t *data, uint32_t len,
                      int16_t **pcm, uint32_t *pcm_len)
{ (void)s; (void)data; (void)len; (void)pcm; (void)pcm_len; return 0; }
static int mp3_seek(void *s, uint32_t pos) { (void)s; (void)pos; return 0; }
static void mp3_deinit(void *s) { (void)s; }
