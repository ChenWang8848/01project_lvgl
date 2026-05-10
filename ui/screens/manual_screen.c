/**
 * @file    manual_screen.c
 * @brief   手动播放 — 文件浏览器实现
 *
 * 左侧: 4个竖排按钮 (退出/返回/上一页/下一页)
 * 右侧: 4列×3行 文件图标网格 (目录/图片/音乐/文本 不同图标)
 *       点击目录→进入, 点击支持的文件→预留处理, 不支持→无反应
 */

#include "manual_screen.h"
#include "app/page_manager.h"
#include "ui/styles.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ================================================================
 *  布局常量 (800×480)
 * ================================================================ */
#define SIDEBAR_W   80      /* 左侧工具栏宽度 */
#define GRID_X      SIDEBAR_W
#define GRID_W      (800 - SIDEBAR_W)  /* 720 */
#define COLS        4
#define ROWS        3
#define ICON_SIZE   75
#define CELL_W      (GRID_W / COLS)    /* 180 */
#define CELL_H      105               /* 图标75 + 间距6 + 标签22 */
#define ICON_OFF_X  ((CELL_W - ICON_SIZE) / 2)  /* (180-75)/2 = 52 */
#define GRID_TOP    95                /* 网格起始 Y */

/* 左侧按钮 */
#define SIDEBAR_X   0
#define BTN_W       72
#define BTN_H       78
#define BTN_SPACE   ((424 - 4 * BTN_H) / 5)  /* 间距: ~23px */
#define BTN_FIRST_Y (56 + BTN_SPACE)          /* 第一个按钮 Y */

/* ================================================================
 *  辅助函数
 * ================================================================ */

/** @brief 获取上级目录路径 */
static void get_parent_path(const char *path, char *out, int out_sz)
{
    if (strcmp(path, "/") == 0) {
        snprintf(out, out_sz, "/");
        return;
    }
    snprintf(out, out_sz, "%s", path);
    char *p = strrchr(out, '/');
    if (p == out) {
        out[1] = '\0';  /* /foo → / */
    } else if (p) {
        *p = '\0';
    }
}

/* ================================================================
 *  事件处理
 * ================================================================ */
static void on_exit_click(lv_event_t *e);
static void on_back_click(lv_event_t *e);
static void on_prev_click(lv_event_t *e);
static void on_next_click(lv_event_t *e);
static void on_grid_item_click(lv_event_t *e);

/* ================================================================
 *  前向声明
 * ================================================================ */
static void load_directory(manual_screen_t *self);
static void render_page(manual_screen_t *self);
static void clear_grid(manual_screen_t *self);

/* ================================================================
 *  构造/析构
 * ================================================================ */
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

/* ================================================================
 *  创建左侧工具栏
 * ================================================================ */
static void create_sidebar(manual_screen_t *self)
{
    self->sidebar = lv_obj_create(self->base.screen);
    lv_obj_set_style_bg_color(self->sidebar, lv_color_hex(0x252540), LV_PART_MAIN);
    lv_obj_set_size(self->sidebar, SIDEBAR_W, 424);
    lv_obj_set_pos(self->sidebar, SIDEBAR_X, 56);

    const char *labels[] = { "退出", "返回", "上一页", "下一页" };
    lv_event_cb_t cbs[]   = { on_exit_click, on_back_click,
                              on_prev_click, on_next_click };

    for (int i = 0; i < 4; i++) {
        int y = BTN_FIRST_Y - 56 + i * (BTN_H + BTN_SPACE);

        lv_obj_t *btn = lv_btn_create(self->sidebar);
        lv_obj_set_size(btn, BTN_W, BTN_H);
        lv_obj_set_pos(btn, 4, y);
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x404060), LV_PART_MAIN);
        lv_obj_set_style_radius(btn, 8, LV_PART_MAIN);
        lv_obj_add_event_cb(btn, cbs[i], LV_EVENT_CLICKED, self);

        lv_obj_t *label = lv_label_create(btn);
        lv_label_set_text(label, labels[i]);
        if (font_cjk_16)
            lv_obj_set_style_text_font(label, font_cjk_16, LV_PART_MAIN);
        lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_center(label);

        /* 保存按钮引用 */
        if (i == 0) self->btn_exit  = btn;
        if (i == 1) self->btn_back  = btn;
        if (i == 2) self->btn_prev  = btn;
        if (i == 3) self->btn_next  = btn;
    }
}

/* ================================================================
 *  创建图标网格容器
 * ================================================================ */
static void create_grid_container(manual_screen_t *self)
{
    self->grid_cont = lv_obj_create(self->base.screen);
    lv_obj_set_style_bg_opa(self->grid_cont, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(self->grid_cont, 0, LV_PART_MAIN);
    lv_obj_set_pos(self->grid_cont, GRID_X, 56);
    lv_obj_set_size(self->grid_cont, GRID_W, 424);

    /* 页面指示 (右下角) */
    self->page_label = lv_label_create(self->base.screen);
    if (font_cjk_16)
        lv_obj_set_style_text_font(self->page_label, font_cjk_16, LV_PART_MAIN);
    lv_obj_set_style_text_color(self->page_label, lv_color_hex(0x888888), LV_PART_MAIN);
    lv_obj_set_pos(self->page_label, GRID_X + GRID_W - 120, 456);
}

/* ================================================================
 *  加载目录
 * ================================================================ */
static void load_directory(manual_screen_t *self)
{
    clear_grid(self);

    self->total_entries = file_list_dir(self->cur_path,
                                         self->entries, MAX_DIR_ENTRIES);
    if (self->total_entries < 0) self->total_entries = 0;

    self->cur_page = 0;
    render_page(self);
}

/* ================================================================
 *  渲染当前页
 * ================================================================ */
static void render_page(manual_screen_t *self)
{
    clear_grid(self);

    int start = self->cur_page * ITEMS_PER_PAGE;
    int end   = start + ITEMS_PER_PAGE;
    if (end > self->total_entries) end = self->total_entries;

    /* 更新标题: 显示当前路径 */
    lv_label_set_text(self->path_label, self->cur_path);

    /* 更新页面指示 */
    int total_pages = (self->total_entries + ITEMS_PER_PAGE - 1) / ITEMS_PER_PAGE;
    if (total_pages < 1) total_pages = 1;
    char page_str[32];
    snprintf(page_str, sizeof(page_str), "%d/%d", self->cur_page + 1, total_pages);
    lv_label_set_text(self->page_label, page_str);

    /* 更新上一页/下一页按钮样式 (不可用时变暗) */
    lv_obj_set_style_bg_opa(self->btn_prev,
        (self->cur_page > 0) ? LV_OPA_COVER : LV_OPA_50, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(self->btn_next,
        (end < self->total_entries) ? LV_OPA_COVER : LV_OPA_50, LV_PART_MAIN);

    /* 绘制图标 */
    for (int i = 0; i < (end - start); i++) {
        int idx    = start + i;
        int col    = i % COLS;
        int row    = i / COLS;
        int x      = col * CELL_W + ICON_OFF_X;
        int y      = GRID_TOP - 56 + row * CELL_H;

        dir_entry_t *entry = &self->entries[idx];
        const char  *icon_sym = file_type_icon(entry->type);

        /* ---- 图标方块 ---- */
        lv_obj_t *icon = lv_obj_create(self->grid_cont);
        lv_obj_set_size(icon, ICON_SIZE, ICON_SIZE);
        lv_obj_set_pos(icon, x, y);
        lv_obj_set_style_radius(icon, 12, LV_PART_MAIN);
        lv_obj_add_flag(icon, LV_OBJ_FLAG_CLICKABLE);

        /* 根据类型设置颜色 */
        lv_color_t bg;
        switch (entry->type) {
            case FILE_TYPE_DIR:   bg = lv_color_hex(0x4A90D9); break;
            case FILE_TYPE_IMAGE: bg = lv_color_hex(0x27AE60); break;
            case FILE_TYPE_MUSIC: bg = lv_color_hex(0xE67E22); break;
            case FILE_TYPE_TEXT:  bg = lv_color_hex(0x8E44AD); break;
            default:              bg = lv_color_hex(0x555555); break;
        }
        lv_obj_set_style_bg_color(icon, bg, LV_PART_MAIN);
        lv_obj_set_style_bg_opa(icon, LV_OPA_COVER, LV_PART_MAIN);

        /* 点击事件携带 self 指针 (通过 on_grid_item_click 查找 idx) */
        lv_obj_add_event_cb(icon, on_grid_item_click, LV_EVENT_CLICKED, self);

        /* ---- 图标内符号 ---- */
        lv_obj_t *sym = lv_label_create(icon);
        lv_label_set_text(sym, icon_sym);
        lv_obj_set_style_text_font(sym, &lv_font_montserrat_28, LV_PART_MAIN);
        lv_obj_set_style_text_color(sym, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_center(sym);

        /* ---- 文件名标签 ---- */
        lv_obj_t *name_label = lv_label_create(self->grid_cont);
        /* 截断过长文件名 */
        char disp_name[32];
        snprintf(disp_name, sizeof(disp_name), "%.30s", entry->name);
        lv_label_set_text(name_label, disp_name);
        if (font_cjk_16)
            lv_obj_set_style_text_font(name_label, font_cjk_16, LV_PART_MAIN);
        lv_obj_set_style_text_color(name_label, lv_color_hex(0xCCCCCC), LV_PART_MAIN);
        lv_obj_set_style_text_align(name_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_obj_set_pos(name_label, col * CELL_W, y + ICON_SIZE + 4);
        lv_obj_set_width(name_label, CELL_W);

        self->item_objs[i]   = icon;
        self->item_labels[i] = name_label;
    }
}

/* ================================================================
 *  清空网格
 * ================================================================ */
static void clear_grid(manual_screen_t *self)
{
    for (int i = 0; i < ITEMS_PER_PAGE; i++) {
        if (self->item_objs[i])   { lv_obj_del(self->item_objs[i]);   self->item_objs[i]   = NULL; }
        if (self->item_labels[i]) { lv_obj_del(self->item_labels[i]); self->item_labels[i] = NULL; }
    }
}

/* ================================================================
 *  事件: 退出 → 主菜单
 * ================================================================ */
static void on_exit_click(lv_event_t *e)
{
    (void)e;
    page_manager_navigate("main", NULL);
}

/* ================================================================
 *  事件: 返回 → 上级目录
 * ================================================================ */
static void on_back_click(lv_event_t *e)
{
    manual_screen_t *self = (manual_screen_t *)lv_event_get_user_data(e);
    if (strcmp(self->cur_path, "/") == 0) return; /* 根目录不返回 */

    get_parent_path(self->cur_path, self->cur_path, sizeof(self->cur_path));
    load_directory(self);
}

/* ================================================================
 *  事件: 上一页
 * ================================================================ */
static void on_prev_click(lv_event_t *e)
{
    manual_screen_t *self = (manual_screen_t *)lv_event_get_user_data(e);
    if (self->cur_page > 0) {
        self->cur_page--;
        render_page(self);
    }
}

/* ================================================================
 *  事件: 下一页
 * ================================================================ */
static void on_next_click(lv_event_t *e)
{
    manual_screen_t *self = (manual_screen_t *)lv_event_get_user_data(e);
    int total_pages = (self->total_entries + ITEMS_PER_PAGE - 1) / ITEMS_PER_PAGE;
    if (self->cur_page < total_pages - 1) {
        self->cur_page++;
        render_page(self);
    }
}

/* ================================================================
 *  事件: 点击文件/目录图标 (通过 self 指针访问 manual_screen)
 * ================================================================ */
static void on_grid_item_click(lv_event_t *e)
{
    manual_screen_t *self = (manual_screen_t *)lv_event_get_user_data(e);
    lv_obj_t *target = lv_event_get_target(e);

    /* 查找点击的是哪个 item */
    for (int i = 0; i < ITEMS_PER_PAGE; i++) {
        if (self->item_objs[i] == target) {
            int idx = self->cur_page * ITEMS_PER_PAGE + i;
            if (idx >= self->total_entries) return;

            dir_entry_t *entry = &self->entries[idx];

            if (entry->is_dir) {
                /* 进入子目录 */
                char new_path[512];
                snprintf(new_path, sizeof(new_path), "%s/%s",
                         self->cur_path, entry->name);
                snprintf(self->cur_path, sizeof(self->cur_path), "%s", new_path);
                load_directory(self);
            } else {
                /* 文件: 根据类型处理 */
                switch (entry->type) {
                    case FILE_TYPE_IMAGE:
                        /* TODO: Phase 6 — 图片查看器 */
                        printf("[INFO] 打开图片: %s/%s\n",
                               self->cur_path, entry->name);
                        break;
                    case FILE_TYPE_MUSIC:
                        /* TODO: Phase 7 — 音乐播放器 */
                        printf("[INFO] 播放音乐: %s/%s\n",
                               self->cur_path, entry->name);
                        break;
                    case FILE_TYPE_TEXT:
                        /* TODO: Phase 6 — 文本阅读器 */
                        printf("[INFO] 打开文本: %s/%s\n",
                               self->cur_path, entry->name);
                        break;
                    default:
                        /* 不支持的文件类型: 无反应 */
                        break;
                }
            }
            return;
        }
    }
}

/* ================================================================
 *  构建主页面
 * ================================================================ */
static lv_obj_t *manual_screen_create(base_screen_t *base)
{
    manual_screen_t *self = (manual_screen_t *)base;

    /* 创建 screen */
    self->base.screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(self->base.screen,
                              lv_color_hex(0x1A1A2E), LV_PART_MAIN);

    /* ---- 标题栏 ---- */
    self->title_bar = lv_obj_create(self->base.screen);
    lv_obj_set_style_bg_color(self->title_bar, lv_color_hex(0x303030), LV_PART_MAIN);
    lv_obj_set_pos(self->title_bar, 0, 0);
    lv_obj_set_size(self->title_bar, 800, 56);

    self->path_label = lv_label_create(self->title_bar);
    lv_label_set_text(self->path_label, "/");
    lv_obj_set_style_text_color(self->path_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    if (font_cjk_16)
        lv_obj_set_style_text_font(self->path_label, font_cjk_16, LV_PART_MAIN);
    lv_obj_set_pos(self->path_label, 10, 16);

    /* ---- 左侧工具栏 ---- */
    create_sidebar(self);

    /* ---- 文件网格容器 ---- */
    create_grid_container(self);

    /* ---- 加载根目录 ---- */
    load_directory(self);

    lv_scr_load(self->base.screen);
    return self->base.screen;
}

static void manual_screen_destroy(base_screen_t *base)
{
    manual_screen_t *self = (manual_screen_t *)base;
    clear_grid(self);
    if (self->base.screen) {
        lv_obj_del(self->base.screen);
        self->base.screen = NULL;
    }
}
