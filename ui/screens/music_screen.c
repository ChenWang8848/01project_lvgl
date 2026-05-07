#include "music_screen.h"
#include <string.h>

static lv_obj_t *music_create(base_screen_t *b) { (void)b; return NULL; }

void music_screen_init(music_screen_t *self)
{
    memset(self, 0, sizeof(*self));
    base_screen_init(&self->base, "music");
    self->base.create = music_create;
}
