#ifndef FB_DRIVER_H
#define FB_DRIVER_H

#include "display_driver.h"
#include <linux/fb.h>
#include <stdint.h>

typedef struct {
    display_driver_t base;
    int              fd;
    uint8_t         *fb_mmap;
    size_t           fb_size;
    const char      *dev_path;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
} fb_driver_t;

void fb_driver_init(fb_driver_t *self, const char *dev_path);

#endif
