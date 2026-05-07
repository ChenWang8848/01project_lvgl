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

static int mp3_init(void *s) { (void)s; return 0; }
static int mp3_decode(void *s, const uint8_t *data, uint32_t len,
                      int16_t **pcm, uint32_t *pcm_len)
{ (void)s; (void)data; (void)len; (void)pcm; (void)pcm_len; return 0; }
static int mp3_seek(void *s, uint32_t pos) { (void)s; (void)pos; return 0; }
static void mp3_deinit(void *s) { (void)s; }
