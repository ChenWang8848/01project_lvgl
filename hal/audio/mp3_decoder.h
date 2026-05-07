#ifndef MP3_DECODER_H
#define MP3_DECODER_H

#include <stdint.h>

typedef struct {
    int  (*init)(void *self);
    int  (*decode)(void *self, const uint8_t *data, uint32_t len,
                   int16_t **pcm, uint32_t *pcm_len);
    int  (*seek)(void *self, uint32_t pos);
    void (*deinit)(void *self);
} mp3_decoder_t;

void mp3_decoder_init(mp3_decoder_t *self);

#endif
