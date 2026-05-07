/**
 * @file    file_service.c
 * @brief   文件服务实现
 *
 * 当前为 Phase 2 桩代码。Phase 4 将从旧代码迁移:
 *   1. file_map()   — open + fstat + mmap
 *   2. file_unmap() — munmap
 *   3. file_list_dir() — opendir + readdir + closedir
 *   4. file_walk_dir() — 递归遍历子目录
 */

#include "file_service.h"

int file_service_init(void)
{
    return 0; /* TODO: Phase 4 */
}

file_map_t *file_map(const char *path)
{
    (void)path;
    return NULL; /* TODO: Phase 4 — open + mmap */
}

void file_unmap(file_map_t *map)
{
    (void)map;
    /* TODO: Phase 4 — munmap */
}

int file_list_dir(const char *path, dir_entry_t *entries, int max_entries)
{
    (void)path;
    (void)entries;
    (void)max_entries;
    return 0; /* TODO: Phase 4 — opendir/readdir */
}
