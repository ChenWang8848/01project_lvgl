/**
 * @file    main_screen.c
 * @brief   主菜单页面 — 手机桌面风格图标网格
 *
 * 布局: 2行×6列 图标网格 (共12槽位)
 *   第一行: 手动播放 / 自动播放 / 设置 (前3槽, 后3槽预留)
 *   第二行: 全部预留 (后续扩展)
 *
 *   每槽: 90×90 彩色圆角图标 + 下方文字标签
 */

#include "main_screen.h"
#include "app/page_manager.h"
#include "ui/styles.h"
#include <string.h>

/* ================================================================
 *  布局常量 (800×480)
 * ================================================================ */
#define COLS        6           /* 列数 */
#define CELL_W      126         /* 单格宽度: (800-40)/6 ≈ 126 */
#define ICON_SIZE   90          /* 图标边长 */
#define ICON_OFF_X  ((CELL_W - ICON_SIZE) / 2)  /* 图标在格内水平偏移 */

#define ROW1_Y      100         /* 第一行图标 Y */
#define ROW2_Y      260         /* 第二行图标 Y (图标90+标签20+间距50) */
#define LABEL_OFF_Y (ICON_SIZE + 6) /* 标签在图标下方偏移 */

/* ================================================================
 *  图标数据
 * ================================================================ */
typedef struct {
    const char   *char_text;    /* 图标内大字 */
    const char   *label;       /* 底部标签 */
    const char   *target;      /* 目标页面名称 */
} icon_entry_t;

static const icon_entry_t g_icons[] = {
    { LV_SYMBOL_IMAGE,    "手动播放", "manual"  },
    { LV_SYMBOL_PLAY,     "自动播放", "auto"    },
    { LV_SYMBOL_SETTINGS, "设置",    "setting" },
};

/** @brief 根据目标页面名返回对应的图标背景样式 */
static lv_style_t *get_icon_style(const char *target)
{
    if (strcmp(target, "manual")  == 0) return style_icon_blue;
    if (strcmp(target, "auto")    == 0) return style_icon_green;
    if (strcmp(target, "setting") == 0) return style_icon_orange;
    return style_icon_blue; /* 兜底 */
}
#define ICON_COUNT (sizeof(g_icons) / sizeof(g_icons[0]))

/* ================================================================
 *  事件处理
 * ================================================================ */

/**
 * @brief 图标点击事件 — 导航到对应子页面
 */
static void on_icon_click(lv_event_t *e)
{
    const char *target = (const char *)lv_event_get_user_data(e);
    page_manager_navigate(target, NULL);
}

/* ================================================================
 *  构造/析构
 * ================================================================ */

static lv_obj_t *main_screen_create(base_screen_t *base);
static void main_screen_destroy(base_screen_t *base);

void main_screen_init(main_screen_t *self)
{
    memset(self, 0, sizeof(*self));
    base_screen_init(&self->base, "main");
    self->base.create  = main_screen_create;
    self->base.destroy = main_screen_destroy;
}

/**
 * @brief 创建图标项 (彩色方块 + 内部大字 + 底部标签)
 */
static lv_obj_t *create_icon(lv_obj_t *parent, const icon_entry_t *entry,
                              int col, int row)
{
    int x = 20 + col * CELL_W + ICON_OFF_X;
    int y = (row == 0) ? ROW1_Y : ROW2_Y;

    /* 图标主体: 彩色圆角方块 */
    lv_obj_t *icon = lv_obj_create(parent);
    lv_obj_add_style(icon, get_icon_style(entry->target), LV_PART_MAIN);
    lv_obj_set_pos(icon, x, y);
    lv_obj_set_size(icon, ICON_SIZE, ICON_SIZE);

    /* 添加点击事件 */
    lv_obj_add_flag(icon, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(icon, on_icon_click, LV_EVENT_CLICKED,
                        (void *)entry->target);

    /* 图标内大字 (居中) */
    lv_obj_t *char_label = lv_label_create(icon);
    lv_label_set_text(char_label, entry->char_text);
    lv_obj_add_style(char_label, style_icon_text, LV_PART_MAIN);
    lv_obj_center(char_label);

    /* 底部中文说明文字 (FreeType 字体) */
    lv_obj_t *desc = lv_label_create(parent);
    lv_label_set_text(desc, entry->label);
    lv_obj_add_style(desc, style_label_cn, LV_PART_MAIN);
    lv_obj_set_pos(desc, x - 10, y + LABEL_OFF_Y);
    lv_obj_set_width(desc, ICON_SIZE + 20);

    return icon;
}

/**
 * @brief 构建主菜单页面
 */
static lv_obj_t *main_screen_create(base_screen_t *base)
{
    main_screen_t *self = (main_screen_t *)base;

    /* 创建空白 screen (深色背景) */
    self->base.screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(self->base.screen,
                              lv_color_hex(0x1A1A2E), LV_PART_MAIN);

    /* 创建图标: 第一行 col 0,1,2 */
    for (int i = 0; i < (int)ICON_COUNT; i++) {
        create_icon(self->base.screen, &g_icons[i], i % COLS, i / COLS);
    }

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
