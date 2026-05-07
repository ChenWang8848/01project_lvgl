#include "browse_screen.h"
#include <string.h>

static lv_obj_t *browse_create(base_screen_t *b) { (void)b; return NULL; }

void browse_screen_init(browse_screen_t *self)
{
    memset(self, 0, sizeof(*self));
    base_screen_init(&self->base, "browse");
    self->base.create = browse_create;
}
