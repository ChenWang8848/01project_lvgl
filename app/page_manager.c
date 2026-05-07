#include "page_manager.h"
#include <string.h>

#define MAX_SCREENS 16

static base_screen_t *g_screens[MAX_SCREENS];
static int            g_screen_count = 0;
static base_screen_t *g_current = NULL;

void page_manager_init(void)
{
    g_screen_count = 0;
    g_current = NULL;
    memset(g_screens, 0, sizeof(g_screens));
}

void page_manager_register(base_screen_t *screen)
{
    if (g_screen_count < MAX_SCREENS) {
        g_screens[g_screen_count++] = screen;
    }
}

void page_manager_navigate(const char *name, void *user_data)
{
    for (int i = 0; i < g_screen_count; i++) {
        if (strcmp(g_screens[i]->name, name) == 0) {
            /* exit old screen */
            if (g_current && g_current->on_exit)
                g_current->on_exit(g_current);

            /* create new screen */
            g_current = g_screens[i];
            if (g_current->create)
                g_current->create(g_current);

            if (g_current->on_enter)
                g_current->on_enter(g_current, user_data);

            return;
        }
    }
}

base_screen_t *page_manager_get_current(void)
{
    return g_current;
}

void page_manager_go_back(void)
{
    /* simplified: no back stack for now */
}
