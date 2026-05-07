#include "fb_driver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

/* --- static helper --- */
static int  fb_init(display_driver_t *base);
static void fb_get_resolution(display_driver_t *base, int *w, int *h);
static int  fb_get_bpp(display_driver_t *base);
static void fb_flush(display_driver_t *base, lv_disp_drv_t *drv,
                     const lv_area_t *area, lv_color_t *color_p);
static void fb_wait_cb(display_driver_t *base, lv_disp_drv_t *drv);
static void fb_deinit(display_driver_t *base);

void fb_driver_init(fb_driver_t *self, const char *dev_path)
{
    memset(self, 0, sizeof(*self));

    /* vtable */
    self->base.name          = "fbdev";
    self->base.init          = fb_init;
    self->base.get_resolution = fb_get_resolution;
    self->base.get_bpp       = fb_get_bpp;
    self->base.flush          = fb_flush;
    self->base.wait_cb        = fb_wait_cb;
    self->base.deinit         = fb_deinit;

    self->fd       = -1;
    self->dev_path = dev_path ? dev_path : "/dev/fb0";
}

/* -------- vtable implementations -------- */

static int fb_init(display_driver_t *base)
{
    fb_driver_t *self = (fb_driver_t *)base;

    self->fd = open(self->dev_path, O_RDWR);
    if (self->fd < 0) {
        perror("fb_driver: open");
        return -1;
    }

    if (ioctl(self->fd, FBIOGET_VSCREENINFO, &self->vinfo) < 0) {
        perror("fb_driver: FBIOGET_VSCREENINFO");
        close(self->fd);
        return -1;
    }

    if (ioctl(self->fd, FBIOGET_FSCREENINFO, &self->finfo) < 0) {
        perror("fb_driver: FBIOGET_FSCREENINFO");
        close(self->fd);
        return -1;
    }

    self->fb_size = self->finfo.smem_len;
    self->fb_mmap = (uint8_t *)mmap(NULL, self->fb_size,
                                     PROT_READ | PROT_WRITE,
                                     MAP_SHARED, self->fd, 0);
    if (self->fb_mmap == MAP_FAILED) {
        perror("fb_driver: mmap");
        close(self->fd);
        return -1;
    }

    printf("fb_driver: %dx%d, bpp=%d, mem=%zu bytes\n",
           self->vinfo.xres, self->vinfo.yres,
           self->vinfo.bits_per_pixel, self->fb_size);

    return 0;
}

static void fb_get_resolution(display_driver_t *base, int *w, int *h)
{
    fb_driver_t *self = (fb_driver_t *)base;
    *w = self->vinfo.xres;
    *h = self->vinfo.yres;
}

static int fb_get_bpp(display_driver_t *base)
{
    fb_driver_t *self = (fb_driver_t *)base;
    return self->vinfo.bits_per_pixel;
}

static void fb_flush(display_driver_t *base, lv_disp_drv_t *drv,
                     const lv_area_t *area, lv_color_t *color_p)
{
    fb_driver_t *self = (fb_driver_t *)base;
    (void)drv;

    if (!self->fb_mmap) {
        lv_disp_flush_ready(drv);
        return;
    }

    uint32_t xres = self->vinfo.xres;
    uint32_t yres = self->vinfo.yres;
    uint32_t bpp  = self->vinfo.bits_per_pixel;

    int32_t w = lv_area_get_width(area);
    int32_t h = lv_area_get_height(area);

    /* clip */
    if (area->x1 >= (int32_t)xres || area->y1 >= (int32_t)yres) {
        lv_disp_flush_ready(drv);
        return;
    }
    if (area->x1 + w > (int32_t)xres) w = xres - area->x1;
    if (area->y1 + h > (int32_t)yres) h = yres - area->y1;

    for (int32_t y = 0; y < h; y++) {
        uint32_t fb_y = (area->y1 + y) * self->finfo.line_length;
        for (int32_t x = 0; x < w; x++) {
            lv_color_t c = color_p[y * w + x];
            uint32_t off = fb_y + (area->x1 + x) * (bpp / 8);
#if LV_COLOR_DEPTH == 32
            self->fb_mmap[off + 0] = c.ch.blue;
            self->fb_mmap[off + 1] = c.ch.green;
            self->fb_mmap[off + 2] = c.ch.red;
            self->fb_mmap[off + 3] = 0xFF;
#elif LV_COLOR_DEPTH == 16
            uint16_t *p = (uint16_t *)(self->fb_mmap + off);
            *p = c.full;
#endif
        }
    }

    lv_disp_flush_ready(drv);
}

static void fb_wait_cb(display_driver_t *base, lv_disp_drv_t *drv)
{
    (void)base;
    (void)drv;
}

static void fb_deinit(display_driver_t *base)
{
    fb_driver_t *self = (fb_driver_t *)base;
    if (self->fb_mmap && self->fb_mmap != MAP_FAILED)
        munmap(self->fb_mmap, self->fb_size);
    if (self->fd >= 0)
        close(self->fd);
    self->fb_mmap = NULL;
    self->fd = -1;
}
