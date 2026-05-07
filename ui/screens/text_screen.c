#include "text_screen.h"
#include <string.h>

static lv_obj_t *text_create(base_screen_t *b) { (void)b; return NULL; }

void text_screen_init(text_screen_t *self)
{
    memset(self, 0, sizeof(*self));
    base_screen_init(&self->base, "text");
    self->base.create = text_create;
}
