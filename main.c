/**
 * @file    main.c
 * @brief   程序入口 — 初始化应用控制器，进入 LVGL 主循环
 *
 * 硬件平台: ARM Linux (arm-linux-gnueabihf-)
 * 显示设备: 800x480 framebuffer (/dev/fb0)
 * UI 框架: LVGL v8.2
 */

#include "app/app.h"
#include <stdint.h>
#include <stddef.h>
#include <sys/time.h>

/**
 * @brief  LVGL 自定义 tick 源 — 返回系统启动以来的毫秒数
 *
 * LVGL 通过 LV_TICK_CUSTOM_SYS_TIME_EXPR 宏调用此函数获取时间戳。
 * 首次调用时记录起始时间，后续返回相对于起始时间的差值。
 */
uint32_t custom_tick_get(void)
{
    static uint64_t start_ms = 0;
    if (start_ms == 0) {
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);
        start_ms = (tv_start.tv_sec * 1000000 + tv_start.tv_usec) / 1000;
    }

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    uint64_t now_ms;
    now_ms = (tv_now.tv_sec * 1000000 + tv_now.tv_usec) / 1000;

    uint32_t time_ms = now_ms - start_ms;
    return time_ms;
}

/**
 * @brief  程序主入口
 *
 * 执行流程:
 *   1. app_controller_init()  — 初始化 HAL / 服务 / UI / 页面
 *   2. app_controller_run()   — 进入 LVGL 事件主循环 (while(1))
 *   3. app_controller_cleanup() — 资源清理 (正常情况下不会到达)
 */
int main(int argc, char *argv[])
{
    app_controller_t app;

    if (app_controller_init(&app, argc, argv) != 0)
        return -1;

    app_controller_run(&app);

    app_controller_cleanup(&app);
    return 0;
}
