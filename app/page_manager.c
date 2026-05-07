/**
 * @file    page_manager.c
 * @brief   页面管理器实现
 *
 * 使用静态数组存储所有注册页面的指针 (最大 16 个)。
 * 页面查找采用线性搜索 (因页面数量少, 性能足够)。
 *
 * 当前为简化版本: 没有回退栈。后续可扩展为支持页面栈的版本。
 */

#include "page_manager.h"
#include <string.h>

#define MAX_SCREENS 16          /**< 最大页面注册数量 */

static base_screen_t *g_screens[MAX_SCREENS];  /**< 页面注册表 */
static int            g_screen_count = 0;      /**< 已注册页面数 */
static base_screen_t *g_current = NULL;        /**< 当前活动页面 */

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

/**
 * @brief 页面导航
 *
 * 根据页面名称查找已注册的页面，执行页面切换:
 *   1. 退出旧页面 (on_exit)
 *   2. 创建新页面 (create) — 构建 LVGL 控件树
 *   3. 进入新页面 (on_enter)
 */
void page_manager_navigate(const char *name, void *user_data)
{
    for (int i = 0; i < g_screen_count; i++) {
        if (strcmp(g_screens[i]->name, name) == 0) {
            /* 退出旧页面 */
            if (g_current && g_current->on_exit)
                g_current->on_exit(g_current);

            /* 创建并进入新页面 */
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

/**
 * @brief 回退到上一页面
 *
 * TODO: Phase 5+ — 实现页面栈 (回退历史记录)
 */
void page_manager_go_back(void)
{
    /* 简化实现: 暂不维护回退栈 */
}
