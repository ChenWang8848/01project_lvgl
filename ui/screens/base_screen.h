#ifndef BASE_SCREEN_H
#define BASE_SCREEN_H

#include "lvgl/lvgl.h"

typedef struct base_screen_s base_screen_t;

struct base_screen_s {
    const char *name;
    lv_obj_t   *screen;
    lv_obj_t  *(*create)(base_screen_t *self);
    void (*on_enter)(base_screen_t *self, void *data);
    void (*on_exit)(base_screen_t *self);
    void (*destroy)(base_screen_t *self);
};

#include <string.h>

static inline void base_screen_init(base_screen_t *self, const char *name)
{
    memset(self, 0, sizeof(*self));
    self->name = name;
}

#endif
