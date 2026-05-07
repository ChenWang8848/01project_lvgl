/**
 * @file    app.h
 * @brief   应用控制器 — 初始化序列、主循环、资源清理
 *
 * 设计模式: 单例 (全局唯一实例)。
 *
 * 初始化流程 (按依赖顺序):
 *   1. LVGL 核心初始化  (lv_init)
 *   2. HAL 层初始化     (显示/输入/音频)
 *   3. Service 层初始化 (文件/音乐/图片)
 *   4. UI 层初始化      (全局样式)
 *   5. App 层初始化     (注册页面, 导航到主菜单)
 */

#ifndef APP_H
#define APP_H

/**
 * @brief 应用控制器 (单例)
 */
typedef struct {
    int initialized;            /**< 初始化完成标志 */
} app_controller_t;

/**
 * @brief 总初始化 — 按顺序初始化所有子系统
 * @param argc 命令行参数个数
 * @param argv 命令行参数数组
 * @return 0: 成功, -1: 初始化失败
 */
int  app_controller_init(app_controller_t *self, int argc, char *argv[]);

/**
 * @brief 进入 LVGL 事件主循环 (永不返回)
 *
 * while(1) { lv_timer_handler(); usleep(5000); }
 */
void app_controller_run(app_controller_t *self);

/**
 * @brief 清理资源 (正常情况下不会执行到)
 */
void app_controller_cleanup(app_controller_t *self);

#endif
