/** @file manual_screen.c 图片查看器页面桩 (Phase 6 实现) */
#include "manual_screen.h"
#include <string.h>
static lv_obj_t *create(base_screen_t *b) { (void)b; return NULL; }
void manual_screen_init(manual_screen_t *self) {
    memset(self, 0, sizeof(*self));
    base_screen_init(&self->base, "manual");
    self->base.create = create;
}
