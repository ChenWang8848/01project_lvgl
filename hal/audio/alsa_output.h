#ifndef ALSA_OUTPUT_H
#define ALSA_OUTPUT_H

#include "audio_output.h"

typedef struct {
    audio_output_t base;
    void *pcm;
} alsa_output_t;

void alsa_output_init(alsa_output_t *self);

#endif
