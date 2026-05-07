/**
 * @file    base_screen.h
 * @brief   页面虚基类 — 定义所有页面的统一接口 (vtable)
 *
 * 设计模式:
 *   继承: 子类第一个成员为 base_screen_t (结构体嵌套)
 *   多态: 函数指针实现运行时多态 (create/on_enter/on_exit/destroy)
 *
 * 生命週期:
 *   1. <页面>_init()     — 构造对象, 填充 vtable
 *   2. create()           — 构建 LVGL 控件树, 调用 lv_scr_load()
 *   3. on_enter()         — 页面进入回调 (接收上级页面传来的数据)
 *   4. on_exit()          — 页面退出回调 (保存状态等)
 *   5. destroy()          — 销毁控件树, 释放资源
 *
 * 8 个页面子类:
 *   main_screen   (主菜单)
 *   browse_screen (文件浏览器)
 *   manual_screen (图片查看器)
 *   auto_screen   (幻灯片播放)
 *   setting_screen (设置菜单)
 *   interval_screen (间隔设置)
 *   text_screen   (文本阅读器)
 *   music_screen  (音乐播放器)
 */

#ifndef BASE_SCREEN_H
#define BASE_SCREEN_H

#include "lvgl/lvgl.h"
#include <string.h>

typedef struct base_screen_s base_screen_t;

/**
 * @brief 页面虚基类
 */
struct base_screen_s {
    const char *name;           /**< 页面名称 (用于 page_manager 查找) */
    lv_obj_t   *screen;         /**< LVGL screen 对象 (lv_obj_create(NULL)) */

    /**
     * @brief 创建页面 — 构建控件树并加载屏幕
     * @return LVGL screen 对象指针
     */
    lv_obj_t  *(*create)(base_screen_t *self);

    /**
     * @brief 页面进入回调 — 接收上级页面传递的数据
     * @param data 自定义数据 (由 navigate 的 user_data 参数传入)
     */
    void (*on_enter)(base_screen_t *self, void *data);

    /**
     * @brief 页面退出回调 — 保存状态、清理临时数据
     */
    void (*on_exit)(base_screen_t *self);

    /**
     * @brief 销毁页面 — 删除所有控件, 释放内存
     */
    void (*destroy)(base_screen_t *self);
};

/**
 * @brief 页面基类初始化 (子类构造函数中调用)
 * @param self 页面对象
 * @param name 页面名称 (用于注册和导航)
 */
static inline void base_screen_init(base_screen_t *self, const char *name)
{
    memset(self, 0, sizeof(*self));
    self->name = name;
}

#endif
