/**
 * @file    main_screen.c
 * @brief   主菜单页面 — 手机桌面风格图标网格 (米黄主题)
 */
#include "main_screen.h"
#include "app/page_manager.h"
#include "ui/styles.h"
#include <string.h>

#define COLS        6
#define CELL_W      126
#define ICON_SIZE   90
#define ICON_OFF_X  ((CELL_W - ICON_SIZE) / 2)
#define ROW1_Y      100
#define ROW2_Y      260
#define LABEL_OFF_Y (ICON_SIZE + 6)

typedef struct {
    const char *char_text;
    const char *label;
    const char *target;
} icon_entry_t;

static const icon_entry_t g_icons[] = {
    { LV_SYMBOL_IMAGE,    "手动播放", "manual"  },
    { LV_SYMBOL_PLAY,     "自动播放", "auto"    },
    { LV_SYMBOL_SETTINGS, "设置",    "setting" },
};
#define ICON_COUNT (sizeof(g_icons) / sizeof(g_icons[0]))

static lv_style_t *get_icon_style(const char *target)
{
    if (strcmp(target, "manual")  == 0) return style_icon_blue;
    if (strcmp(target, "auto")    == 0) return style_icon_green;
    if (strcmp(target, "setting") == 0) return style_icon_orange;
    return style_icon_blue;
}

static void on_icon_click(lv_event_t *e)
{
    const char *target = (const char *)lv_event_get_user_data(e);
    page_manager_navigate(target, NULL);
}

static lv_obj_t *create_icon(lv_obj_t *parent, const icon_entry_t *entry,
                              int col, int row)
{
    int x = 20 + col * CELL_W + ICON_OFF_X;
    int y = (row == 0) ? ROW1_Y : ROW2_Y;

    lv_obj_t *icon = lv_obj_create(parent);
    lv_obj_add_style(icon, get_icon_style(entry->target), LV_PART_MAIN);
    lv_obj_add_style(icon, style_no_scroll, LV_PART_MAIN);
    lv_obj_set_pos(icon, x, y);
    lv_obj_set_size(icon, ICON_SIZE, ICON_SIZE);
    lv_obj_clear_flag(icon, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(icon, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(icon, on_icon_click, LV_EVENT_CLICKED,
                        (void *)entry->target);

    lv_obj_t *char_label = lv_label_create(icon);
    lv_label_set_text(char_label, entry->char_text);
    lv_obj_add_style(char_label, style_icon_text, LV_PART_MAIN);
    lv_obj_center(char_label);

    lv_obj_t *desc = lv_label_create(parent);
    lv_label_set_text(desc, entry->label);
    lv_obj_add_style(desc, style_label_cn, LV_PART_MAIN);
    lv_obj_set_pos(desc, x - 10, y + LABEL_OFF_Y);
    lv_obj_set_width(desc, ICON_SIZE + 20);

    return icon;
}

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
                              lv_color_hex(0xE7DBB5), LV_PART_MAIN);
    lv_obj_clear_flag(self->base.screen, LV_OBJ_FLAG_SCROLLABLE);

    for (int i = 0; i < (int)ICON_COUNT; i++)
        create_icon(self->base.screen, &g_icons[i], i % COLS, i / COLS);

    lv_scr_load(self->base.screen);
    return self->base.screen;
}

static void main_screen_destroy(base_screen_t *base)
{
    main_screen_t *self = (main_screen_t *)base;
    if (self->base.screen) { lv_obj_del(self->base.screen); self->base.screen = NULL; }
}
