/**
 * @file    app.c
 * @brief   应用控制器实现 — 所有子系统的初始化与生命周期管理
 *
 * 初始化顺序严格按依赖关系:
 *   底层 (HAL) → 服务层 (Service) → UI 层 → App 层
 *
 * 上层依赖下层，下层不依赖上层。
 */

#include "app.h"
#include "page_manager.h"
#include "lvgl/lvgl.h"
#include <unistd.h>

/* ================================================================
 *  HAL 层
 * ================================================================ */
#include "hal/display/fb_driver.h"
#include "hal/display/lvgl_display.h"
#include "hal/input/touchscreen.h"
#include "hal/input/mouse.h"
#include "hal/input/lvgl_input.h"
#include "hal/audio/alsa_output.h"
#include "hal/audio/mp3_decoder.h"

/* ================================================================
 *  Service 层
 * ================================================================ */
#include "service/file_service.h"
#include "service/music_service.h"
#include "service/image_service.h"

/* ================================================================
 *  UI 层
 * ================================================================ */
#include "ui/styles.h"
#include "ui/screens/main_screen.h"
#include "ui/screens/browse_screen.h"
#include "ui/screens/manual_screen.h"
#include "ui/screens/auto_screen.h"
#include "ui/screens/setting_screen.h"
#include "ui/screens/interval_screen.h"
#include "ui/screens/text_screen.h"
#include "ui/screens/music_screen.h"

/* ================================================================
 *  工具层
 * ================================================================ */
#include "util/debug.h"

/* ================================================================
 *  全局单例 — HAL 对象 (应用生命周期内存在)
 * ================================================================ */
static fb_driver_t    g_fb;      /**< framebuffer 显示驱动 */
static lvgl_display_t g_disp;    /**< LVGL 显示适配器 */

/* ================================================================
 *  公开接口
 * ================================================================ */

/**
 * @brief 总初始化序列
 *
 * 步骤:
 *   1. LVGL 核心  (lv_init)
 *   2. 显示子系统 (fb_driver + lvgl_display)
 *   3. 输入子系统 (触摸屏 + 鼠标, 当前为桩)
 *   4. 服务层     (文件/音乐/图片, 当前为桩)
 *   5. UI 层      (全局样式)
 *   6. App 层     (注册全部 8 个页面, 导航到主菜单)
 */
int app_controller_init(app_controller_t *self, int argc, char *argv[])
{
    (void)argc; (void)argv; // 这种写法是消除未使用警告的常见做法
    debug_init();
    LOG_INFO("app_controller_init: starting...");

    /* --- 1. LVGL 核心初始化 --- */
    lv_init();

    /* --- 2. HAL — 显示 --- */
    fb_driver_init(&g_fb, "/dev/fb0");
    if (g_fb.base.init(&g_fb.base) != 0) {
        LOG_ERROR("Failed to init framebuffer driver");
        return -1;
    }

    if (lvgl_display_init(&g_disp, &g_fb.base) != 0) {
        LOG_ERROR("Failed to init LVGL display");
        return -1;
    }

    /* --- 3. HAL — 输入 (Phase 3 桩) --- */
    lvgl_input_init_all();

    /* --- 4. 服务层初始化 (Phase 4/7/8 桩) --- */
    file_service_init();
    music_service_init();
    image_service_init();

    /* --- 5. UI — 全局样式 --- */
    styles_init();

    /* --- 6. App — 注册全部页面, 导航到主菜单 --- */
    page_manager_init();

    /* 创建所有页面实例 (static 保证生命周期) */
    static main_screen_t    scr_main;
    static browse_screen_t  scr_browse;
    static manual_screen_t  scr_manual;
    static auto_screen_t    scr_auto;
    static setting_screen_t scr_setting;
    static interval_screen_t scr_interval;
    static text_screen_t    scr_text;
    static music_screen_t   scr_music;

    main_screen_init(&scr_main);
    browse_screen_init(&scr_browse);
    manual_screen_init(&scr_manual);
    auto_screen_init(&scr_auto);
    setting_screen_init(&scr_setting);
    interval_screen_init(&scr_interval);
    text_screen_init(&scr_text);
    music_screen_init(&scr_music);

    /* 注册全部 8 个页面到页面管理器 */
    page_manager_register(&scr_main.base);
    page_manager_register(&scr_browse.base);
    page_manager_register(&scr_manual.base);
    page_manager_register(&scr_auto.base);
    page_manager_register(&scr_setting.base);
    page_manager_register(&scr_interval.base);
    page_manager_register(&scr_text.base);
    page_manager_register(&scr_music.base);

    /* 加载主菜单页面 (显示 "hello lvgl") */
    page_manager_navigate("main", NULL);

    self->initialized = 1;
    LOG_INFO("app_controller_init: done");
    return 0;
}

/**
 * @brief 进入 LVGL 主循环 (永不返回)
 *
 * 每 5ms 调用一次 lv_timer_handler() 驱动 LVGL 定时器和刷新。
 * LVGL 内部处理:
 *   1. 定时器超时检测
 *   2. 输入设备轮询
 *   3. 脏区域重绘
 *   4. flush 回调刷新到 framebuffer
 */
void app_controller_run(app_controller_t *self)
{
    if (!self->initialized) return;

    while (1) {
        lv_timer_handler();
        usleep(5000);
    }
}

/**
 * @brief 清理所有子系统资源 (反序释放)
 */
void app_controller_cleanup(app_controller_t *self)
{
    (void)self;
    lvgl_display_deinit(&g_disp);
    g_fb.base.deinit(&g_fb.base);
}
