/**
 * @file    file_service.h
 * @brief   文件服务 — 提供 mmap 文件读写、目录遍历功能
 *
 * Phase 4 将从旧代码迁移完整的文件操作逻辑。
 */

#ifndef FILE_SERVICE_H
#define FILE_SERVICE_H

#include <stdint.h>
#include <stddef.h>

/** @brief mmap 文件映射句柄 */
typedef struct {
    void   *addr;               /**< mmap 映射地址 */
    size_t  size;               /**< 文件大小 (字节) */
} file_map_t;

/** @brief 目录条目 */
typedef struct {
    char name[256];             /**< 文件/目录名 */
    int  is_dir;                /**< 1: 目录, 0: 文件 */
} dir_entry_t;

/** @brief 初始化文件服务 */
int  file_service_init(void);

/** @brief mmap 映射文件到内存 (只读) */
file_map_t *file_map(const char *path);

/** @brief 解除 mmap 映射 */
void file_unmap(file_map_t *map);

/** @brief 列出目录下的文件和子目录 */
int  file_list_dir(const char *path, dir_entry_t *entries, int max_entries);

#endif
