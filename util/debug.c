/**
 * @file    debug.c
 * @brief   调试日志模块实现
 *
 * 当前为简单 stdout/stderr 输出。
 * 后续可扩展:
 *   - UDP 远程日志发送 (debug_udp_send)
 *   - 日志级别过滤 (基于 DEBUG_ENABLE 宏)
 *   - 时间戳前缀
 */

#include "debug.h"

void debug_init(void)
{
    LOG_INFO("debug module initialized");
}
