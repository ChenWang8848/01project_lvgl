#include "image_service.h"

int image_service_init(void) { return 0; }
lv_img_dsc_t *image_load(const char *path) { (void)path; return NULL; }
void image_preload(const char *path)        { (void)path; }
void image_cancel_preload(const char *path) { (void)path; }
