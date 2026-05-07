#ifndef AUDIO_OUTPUT_H
#define AUDIO_OUTPUT_H

#include <stdint.h>

typedef struct audio_output_s audio_output_t;

struct audio_output_s {
    const char *name;
    int  (*init)(audio_output_t *self, uint32_t sample_rate, uint8_t channels, uint8_t bits);
    int  (*write)(audio_output_t *self, const void *buf, uint32_t frames);
    int  (*drain)(audio_output_t *self);
    void (*deinit)(audio_output_t *self);
};

#endif
