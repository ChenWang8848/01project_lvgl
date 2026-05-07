#include "auto_screen.h"
#include <string.h>

static lv_obj_t *auto_create(base_screen_t *b) { (void)b; return NULL; }

void auto_screen_init(auto_screen_t *self)
{
    memset(self, 0, sizeof(*self));
    base_screen_init(&self->base, "auto");
    self->base.create = auto_create;
}
