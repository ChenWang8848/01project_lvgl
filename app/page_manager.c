/**
 * @file    page_manager.c
 * @brief   页面管理器实现
 *
 * 使用静态数组存储所有注册页面的指针 (最大 16 个)。
 * 页面查找采用线性搜索 (因页面数量少, 性能足够)。
 * 维护上一页面指针 (g_prev) 用于返回导航。
 */

#include "page_manager.h"
#include <string.h>

#define MAX_SCREENS 16          /**< 最大页面注册数量 */

static base_screen_t *g_screens[MAX_SCREENS];  /**< 页面注册表 */
static int            g_screen_count = 0;      /**< 已注册页面数 */
static base_screen_t *g_current = NULL;        /**< 当前活动页面 */
static base_screen_t *g_prev   = NULL;        /**< 上一页面 (用于返回) */

void page_manager_init(void)
{
    g_screen_count = 0;
    g_current = NULL;
    g_prev   = NULL;
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
 * 保存当前页面到 g_prev，然后切换到目标页面:
 *   1. 退出旧页面 (on_exit)
 *   2. 创建新页面 (create) — 构建 LVGL 控件树
 *   3. 进入新页面 (on_enter)
 */
void page_manager_navigate(const char *name, void *user_data)
{
    for (int i = 0; i < g_screen_count; i++) {
        if (strcmp(g_screens[i]->name, name) == 0) {
            /* 保存当前页面作为返回目标 */
            g_prev = g_current;

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
 * 如果存在上一页面 (g_prev)，则切回。
 * 此时不清除 g_prev (允许再次返回可反复切回)。
 */
void page_manager_go_back(void)
{
    if (g_prev && g_prev->create) {
        base_screen_t *target = g_prev;
        g_prev = g_current;  /* 当前页成为新的"上一页" */

        /* 退出当前页面 */
        if (g_current && g_current->on_exit)
            g_current->on_exit(g_current);

        /* 重新创建目标页面 */
        g_current = target;
        g_current->create(g_current);

        if (g_current->on_enter)
            g_current->on_enter(g_current, NULL);
    }
}
