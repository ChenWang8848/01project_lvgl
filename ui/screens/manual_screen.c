/**
 * @file    manual_screen.c
 * @brief   手动播放 — 文件浏览器 (米黄主题, 无边框/无滚动)
 *   左侧: 4个竖排按钮 (退出/返回/上一页/下一页)
 *   右侧: 4列×3行 文件图标网格
 */
#include "manual_screen.h"
#include "app/page_manager.h"
#include "ui/styles.h"
#include "ui/screens/text_screen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIDEBAR_W   80
#define GRID_X      SIDEBAR_W
#define GRID_W      (800 - SIDEBAR_W)
#define COLS        4
#define ROWS        3
#define ICON_SIZE   75
#define CELL_W      (GRID_W / COLS)
#define CELL_H      105
#define ICON_OFF_X  ((CELL_W - ICON_SIZE) / 2)
#define GRID_TOP    95
#define BTN_W       72
#define BTN_H       78
#define BTN_SPACE   ((424 - 4 * BTN_H) / 5)
#define BTN_FIRST_Y (56 + BTN_SPACE)

/* ---- 辅助 ---- */
static void get_parent_path(const char *path, char *out, int sz)
{
    if (strcmp(path, "/") == 0) { snprintf(out, sz, "/"); return; }
    snprintf(out, sz, "%s", path);
    char *p = strrchr(out, '/');
    if (p == out) out[1] = '\0';
    else if (p) *p = '\0';
}

/* ---- 事件处理 ---- */
static void on_exit_click(lv_event_t *e);
static void on_back_click(lv_event_t *e);
static void on_prev_click(lv_event_t *e);
static void on_next_click(lv_event_t *e);
static void on_grid_item_click(lv_event_t *e);

/* ---- 内部函数 ---- */
static void load_directory(manual_screen_t *self);
static void render_page(manual_screen_t *self);
static void clear_grid(manual_screen_t *self);

/* ================================================================
 *  创建侧边栏
 * ================================================================ */
static void create_sidebar(manual_screen_t *self)
{
    self->sidebar = lv_obj_create(self->base.screen);
    lv_obj_set_style_bg_color(self->sidebar, lv_color_hex(0xD4C4A0), LV_PART_MAIN);
    lv_obj_set_style_border_width(self->sidebar, 0, LV_PART_MAIN);
    lv_obj_clear_flag(self->sidebar, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(self->sidebar, SIDEBAR_W, 424);
    lv_obj_set_pos(self->sidebar, 0, 56);

    const char *labels[] = { "退出", "返回", "上一页", "下一页" };
    lv_event_cb_t cbs[]   = { on_exit_click, on_back_click,
                              on_prev_click, on_next_click };

    for (int i = 0; i < 4; i++) {
        int y = BTN_FIRST_Y - 56 + i * (BTN_H + BTN_SPACE);
        lv_obj_t *btn = lv_btn_create(self->sidebar);
        lv_obj_set_size(btn, BTN_W, BTN_H);
        lv_obj_set_pos(btn, 4, y);
        lv_obj_set_style_bg_color(btn, lv_color_hex(0xC4B490), LV_PART_MAIN);
        lv_obj_set_style_border_width(btn, 0, LV_PART_MAIN);
        lv_obj_set_style_radius(btn, 8, LV_PART_MAIN);
        lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_event_cb(btn, cbs[i], LV_EVENT_CLICKED, self);

        lv_obj_t *label = lv_label_create(btn);
        lv_label_set_text(label, labels[i]);
        lv_obj_set_style_text_color(label, lv_color_hex(0x5A4A3A), LV_PART_MAIN);
        if (font_cjk_16) lv_obj_set_style_text_font(label, font_cjk_16, LV_PART_MAIN);
        lv_obj_center(label);

        if (i == 0) self->btn_exit  = btn;
        if (i == 1) self->btn_back  = btn;
        if (i == 2) self->btn_prev  = btn;
        if (i == 3) self->btn_next  = btn;
    }
}

/* ---- 网格容器 ---- */
static void create_grid_container(manual_screen_t *self)
{
    self->grid_cont = lv_obj_create(self->base.screen);
    lv_obj_set_style_bg_opa(self->grid_cont, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(self->grid_cont, 0, LV_PART_MAIN);
    lv_obj_clear_flag(self->grid_cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_pos(self->grid_cont, GRID_X, 56);
    lv_obj_set_size(self->grid_cont, GRID_W, 424);

    self->page_label = lv_label_create(self->base.screen);
    lv_obj_set_style_text_color(self->page_label, lv_color_hex(0x8A7A6A), LV_PART_MAIN);
    if (font_cjk_16) lv_obj_set_style_text_font(self->page_label, font_cjk_16, LV_PART_MAIN);
    lv_obj_set_pos(self->page_label, GRID_X + GRID_W - 120, 456);
}

/* ---- 加载 & 渲染 ---- */
static void load_directory(manual_screen_t *self)
{
    clear_grid(self);
    self->total_entries = file_list_dir(self->cur_path, self->entries, MAX_DIR_ENTRIES);
    if (self->total_entries < 0) self->total_entries = 0;
    self->cur_page = 0;
    render_page(self);
}

static void render_page(manual_screen_t *self)
{
    clear_grid(self);
    int start = self->cur_page * ITEMS_PER_PAGE;
    int end   = start + ITEMS_PER_PAGE;
    if (end > self->total_entries) end = self->total_entries;

    lv_label_set_text(self->path_label, self->cur_path);

    int total_p = (self->total_entries + ITEMS_PER_PAGE - 1) / ITEMS_PER_PAGE;
    if (total_p < 1) total_p = 1;
    char ps[32]; snprintf(ps, sizeof(ps), "%d/%d", self->cur_page + 1, total_p);
    lv_label_set_text(self->page_label, ps);

    lv_obj_set_style_bg_opa(self->btn_prev,
        (self->cur_page > 0) ? LV_OPA_COVER : LV_OPA_50, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(self->btn_next,
        (end < self->total_entries) ? LV_OPA_COVER : LV_OPA_50, LV_PART_MAIN);

    for (int i = 0; i < (end - start); i++) {
        int idx = start + i, col = i % COLS, row = i / COLS;
        int x = col * CELL_W + ICON_OFF_X;
        int y = GRID_TOP - 56 + row * CELL_H;
        dir_entry_t *e = &self->entries[idx];

        lv_obj_t *icon = lv_obj_create(self->grid_cont);
        lv_obj_set_size(icon, ICON_SIZE, ICON_SIZE);
        lv_obj_set_pos(icon, x, y);
        lv_obj_set_style_radius(icon, 12, LV_PART_MAIN);
        lv_obj_set_style_border_width(icon, 0, LV_PART_MAIN);
        lv_obj_clear_flag(icon, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_flag(icon, LV_OBJ_FLAG_CLICKABLE);

        lv_color_t bg;
        switch (e->type) {
            case FILE_TYPE_DIR:   bg = lv_color_hex(0x4A90D9); break;
            case FILE_TYPE_IMAGE: bg = lv_color_hex(0x27AE60); break;
            case FILE_TYPE_MUSIC: bg = lv_color_hex(0xE67E22); break;
            case FILE_TYPE_TEXT:  bg = lv_color_hex(0x8E44AD); break;
            default:              bg = lv_color_hex(0x999999); break;
        }
        lv_obj_set_style_bg_color(icon, bg, LV_PART_MAIN);
        lv_obj_set_style_bg_opa(icon, LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_add_event_cb(icon, on_grid_item_click, LV_EVENT_CLICKED, self);

        lv_obj_t *sym = lv_label_create(icon);
        lv_label_set_text(sym, file_type_icon(e->type));
        lv_obj_set_style_text_font(sym, &lv_font_montserrat_28, LV_PART_MAIN);
        lv_obj_set_style_text_color(sym, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_center(sym);

        lv_obj_t *nl = lv_label_create(self->grid_cont);
        char dn[32]; snprintf(dn, sizeof(dn), "%.30s", e->name);
        lv_label_set_text(nl, dn);
        lv_obj_set_style_text_color(nl, lv_color_hex(0x5A4A3A), LV_PART_MAIN);
        if (font_cjk_16) lv_obj_set_style_text_font(nl, font_cjk_16, LV_PART_MAIN);
        lv_obj_set_style_text_align(nl, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_obj_set_pos(nl, col * CELL_W, y + ICON_SIZE + 4);
        lv_obj_set_width(nl, CELL_W);

        self->item_objs[i]   = icon;
        self->item_labels[i] = nl;
    }
}

static void clear_grid(manual_screen_t *self)
{
    for (int i = 0; i < ITEMS_PER_PAGE; i++) {
        if (self->item_objs[i])   { lv_obj_del(self->item_objs[i]);   self->item_objs[i]   = NULL; }
        if (self->item_labels[i]) { lv_obj_del(self->item_labels[i]); self->item_labels[i] = NULL; }
    }
}

/* ---- 事件: 侧边栏 ---- */
static void on_exit_click(lv_event_t *e) { (void)e; page_manager_navigate("main", NULL); }

static void on_back_click(lv_event_t *e)
{
    manual_screen_t *s = (manual_screen_t *)lv_event_get_user_data(e);
    if (strcmp(s->cur_path, "/") == 0) return;
    char new_path[512];
    get_parent_path(s->cur_path, new_path, sizeof(new_path));
    snprintf(s->cur_path, sizeof(s->cur_path), "%s", new_path);
    load_directory(s);
}

static void on_prev_click(lv_event_t *e)
{
    manual_screen_t *s = (manual_screen_t *)lv_event_get_user_data(e);
    if (s->cur_page > 0) { s->cur_page--; render_page(s); }
}

static void on_next_click(lv_event_t *e)
{
    manual_screen_t *s = (manual_screen_t *)lv_event_get_user_data(e);
    int tp = (s->total_entries + ITEMS_PER_PAGE - 1) / ITEMS_PER_PAGE;
    if (s->cur_page < tp - 1) { s->cur_page++; render_page(s); }
}

/* ---- 事件: 点击图标 ---- */
static void on_grid_item_click(lv_event_t *e)
{
    manual_screen_t *s = (manual_screen_t *)lv_event_get_user_data(e);
    lv_obj_t *target = lv_event_get_target(e);
    for (int i = 0; i < ITEMS_PER_PAGE; i++) {
        if (s->item_objs[i] == target) {
            int idx = s->cur_page * ITEMS_PER_PAGE + i;
            if (idx >= s->total_entries) return;
            dir_entry_t *ent = &s->entries[idx];
            if (ent->is_dir) {
                char np[512];
                if (strcmp(s->cur_path, "/") == 0)
                    snprintf(np, sizeof(np), "/%s", ent->name);
                else
                    snprintf(np, sizeof(np), "%s/%s", s->cur_path, ent->name);
                snprintf(s->cur_path, sizeof(s->cur_path), "%s", np);
                load_directory(s);
            } else {
                switch (ent->type) {
                    case FILE_TYPE_IMAGE:
                        printf("[INFO] 打开图片: %s/%s\n", s->cur_path, ent->name); break;
                    case FILE_TYPE_MUSIC:
                        printf("[INFO] 播放音乐: %s/%s\n", s->cur_path, ent->name); break;
                    case FILE_TYPE_TEXT: {
                        char fp[512];
                        if (strcmp(s->cur_path, "/") == 0)
                            snprintf(fp, sizeof(fp), "/%s", ent->name);
                        else
                            snprintf(fp, sizeof(fp), "%s/%s", s->cur_path, ent->name);
                        if (g_text_screen) {
                            text_screen_set_path(g_text_screen, fp);
                            page_manager_navigate("text", NULL);
                        }
                        break;
                    }
                    default: break;
                }
            }
            return;
        }
    }
}

/* ---- 构造 ---- */
static lv_obj_t *manual_screen_create(base_screen_t *base);
static void manual_screen_destroy(base_screen_t *base);

void manual_screen_init(manual_screen_t *self)
{
    memset(self, 0, sizeof(*self));
    base_screen_init(&self->base, "manual");
    self->base.create  = manual_screen_create;
    self->base.destroy = manual_screen_destroy;
    snprintf(self->cur_path, sizeof(self->cur_path), "/");
}

static lv_obj_t *manual_screen_create(base_screen_t *base)
{
    manual_screen_t *self = (manual_screen_t *)base;

    self->base.screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(self->base.screen, lv_color_hex(0xE7DBB5), LV_PART_MAIN);
    lv_obj_clear_flag(self->base.screen, LV_OBJ_FLAG_SCROLLABLE);

    self->title_bar = lv_obj_create(self->base.screen);
    lv_obj_set_style_bg_color(self->title_bar, lv_color_hex(0xD4C4A0), LV_PART_MAIN);
    lv_obj_set_style_border_width(self->title_bar, 0, LV_PART_MAIN);
    lv_obj_clear_flag(self->title_bar, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_pos(self->title_bar, 0, 0);
    lv_obj_set_size(self->title_bar, 800, 56);

    self->path_label = lv_label_create(self->title_bar);
    lv_label_set_text(self->path_label, "/");
    lv_obj_set_style_text_color(self->path_label, lv_color_hex(0x5A4A3A), LV_PART_MAIN);
    if (font_cjk_16) lv_obj_set_style_text_font(self->path_label, font_cjk_16, LV_PART_MAIN);
    lv_obj_set_pos(self->path_label, 10, 16);

    create_sidebar(self);
    create_grid_container(self);
    lv_obj_move_foreground(self->sidebar); /* 确保侧边栏在最前 */
    load_directory(self);
    lv_scr_load(self->base.screen);
    return self->base.screen;
}

static void manual_screen_destroy(base_screen_t *base)
{
    manual_screen_t *self = (manual_screen_t *)base;
    clear_grid(self);
    if (self->base.screen) { lv_obj_del(self->base.screen); self->base.screen = NULL; }
}
