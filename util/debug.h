/**
 * @file    debug.h
 * @brief   调试日志模块 — 分级日志宏 (ERROR/WARN/INFO/DEBUG)
 *
 * 使用 printf/fprintf 输出到 stdout/stderr。
 * 后续可扩展 UDP 远程日志功能 (通过 DEBUG_UDP_PORT 配置)。
 */

#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

/** @brief 错误日志 — 输出到 stderr */
#define LOG_ERROR(fmt, ...) fprintf(stderr, "[ERROR] " fmt "\n", ##__VA_ARGS__)

/** @brief 警告日志 — 输出到 stderr */
#define LOG_WARN(fmt, ...)  fprintf(stderr, "[WARN]  " fmt "\n", ##__VA_ARGS__)

/** @brief 信息日志 — 输出到 stdout */
#define LOG_INFO(fmt, ...)  printf("[INFO]  " fmt "\n", ##__VA_ARGS__)

/** @brief 调试日志 — 输出到 stdout */
#define LOG_DEBUG(fmt, ...) printf("[DEBUG] " fmt "\n", ##__VA_ARGS__)

/** @brief 初始化调试模块 */
void debug_init(void);

#endif
