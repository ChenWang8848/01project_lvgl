#include "alsa_output.h"
#include <string.h>

static int  alsa_init(audio_output_t *base, uint32_t rate, uint8_t ch, uint8_t bits);
static int  alsa_write(audio_output_t *base, const void *buf, uint32_t frames);
static int  alsa_drain(audio_output_t *base);
static void alsa_deinit(audio_output_t *base);

void alsa_output_init(alsa_output_t *self)
{
    memset(self, 0, sizeof(*self));
    self->base.name   = "alsa";
    self->base.init   = alsa_init;
    self->base.write  = alsa_write;
    self->base.drain  = alsa_drain;
    self->base.deinit = alsa_deinit;
}

static int alsa_init(audio_output_t *base, uint32_t rate, uint8_t ch, uint8_t bits)
{
    (void)base; (void)rate; (void)ch; (void)bits;
    return 0; /* stub: TODO Phase 7 */
}

static int alsa_write(audio_output_t *base, const void *buf, uint32_t frames)
{
    (void)base; (void)buf; (void)frames;
    return 0;
}

static int alsa_drain(audio_output_t *base)
{
    (void)base;
    return 0;
}

static void alsa_deinit(audio_output_t *base)
{
    (void)base;
}
