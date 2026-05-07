/** @file setting_screen.c 设置菜单页面桩 (Phase 6 实现) */
#include "setting_screen.h"
#include <string.h>
static lv_obj_t *create(base_screen_t *b) { (void)b; return NULL; }
void setting_screen_init(setting_screen_t *self) {
    memset(self, 0, sizeof(*self));
    base_screen_init(&self->base, "setting");
    self->base.create = create;
}
