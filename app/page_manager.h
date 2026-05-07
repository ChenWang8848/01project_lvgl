#ifndef PAGE_MANAGER_H
#define PAGE_MANAGER_H

#include "ui/screens/base_screen.h"

void page_manager_init(void);
void page_manager_register(base_screen_t *screen);
void page_manager_navigate(const char *name, void *user_data);
base_screen_t *page_manager_get_current(void);
void page_manager_go_back(void);

#endif
