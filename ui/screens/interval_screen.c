#include "interval_screen.h"
#include <string.h>

static lv_obj_t *interval_create(base_screen_t *b) { (void)b; return NULL; }

void interval_screen_init(interval_screen_t *self)
{
    memset(self, 0, sizeof(*self));
    base_screen_init(&self->base, "interval");
    self->base.create = interval_create;
}
