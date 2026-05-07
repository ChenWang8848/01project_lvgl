#include "main_screen.h"
#include <string.h>

static lv_obj_t *main_screen_create(base_screen_t *base);
static void main_screen_destroy(base_screen_t *base);

void main_screen_init(main_screen_t *self)
{
    memset(self, 0, sizeof(*self));
    base_screen_init(&self->base, "main");
    self->base.create  = main_screen_create;
    self->base.destroy = main_screen_destroy;
}

static lv_obj_t *main_screen_create(base_screen_t *base)
{
    main_screen_t *self = (main_screen_t *)base;

    self->base.screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(self->base.screen,
                              lv_color_hex(0x202020), LV_PART_MAIN);

    self->label = lv_label_create(self->base.screen);
    lv_label_set_text(self->label, "hello lvgl");
    lv_obj_set_style_text_color(self->label,
                                lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(self->label,
                               &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_center(self->label);

    lv_scr_load(self->base.screen);

    return self->base.screen;
}

static void main_screen_destroy(base_screen_t *base)
{
    main_screen_t *self = (main_screen_t *)base;
    if (self->base.screen) {
        lv_obj_del(self->base.screen);
        self->base.screen = NULL;
    }
}
