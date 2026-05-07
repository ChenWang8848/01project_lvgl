/**
 * @file    page_manager.h
 * @brief   页面管理器 — 页面注册、切换、回退
 *
 * 设计模式: 单例 (全局唯一)。
 *
 * 每个页面是一个 base_screen_t 子类实例。
 * 切换页面时:
 *   1. 调用旧页面的 on_exit() 回调
 *   2. 调用新页面的 create() 构建控件树
 *   3. 调用新页面的 on_enter() 回调
 *   4. 通过 lv_scr_load() 切换到新 screen
 */

#ifndef PAGE_MANAGER_H
#define PAGE_MANAGER_H

#include "ui/screens/base_screen.h"

/** @brief 初始化页面管理器 (清空注册表) */
void page_manager_init(void);

/** @brief 注册一个页面 (存储在内部数组中) */
void page_manager_register(base_screen_t *screen);

/** @brief 切换到指定名称的页面 */
void page_manager_navigate(const char *name, void *user_data);

/** @brief 获取当前活动页面 */
base_screen_t *page_manager_get_current(void);

/** @brief 回到上一页面 (当前为简化实现) */
void page_manager_go_back(void);

#endif
