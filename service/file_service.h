/**
 * @file    file_service.h
 * @brief   文件服务 — mmap 文件读写 / 目录遍历 / 文件类型检测
 */

#ifndef FILE_SERVICE_H
#define FILE_SERVICE_H

#include <stdint.h>
#include <stddef.h>

/** @brief 文件类型 (用于图标选择和操作分发) */
typedef enum {
    FILE_TYPE_DIR    = 0,   /**< 目录 */
    FILE_TYPE_IMAGE  = 1,   /**< 图片 (jpg/png/bmp/gif) */
    FILE_TYPE_MUSIC  = 2,   /**< 音乐 (mp3/wav/ogg/flac) */
    FILE_TYPE_TEXT   = 3,   /**< 文本 (txt/log/cfg) */
    FILE_TYPE_OTHER  = 4,   /**< 其他 (不支持) */
} file_type_t;

/** @brief mmap 文件映射句柄 */
typedef struct {
    void   *addr;
    size_t  size;
} file_map_t;

/** @brief 目录条目 */
typedef struct {
    char name[256];           /**< 文件/目录名 */
    int  is_dir;              /**< 1: 目录, 0: 文件 */
    file_type_t type;         /**< 文件类型 */
} dir_entry_t;

int  file_service_init(void);
file_map_t *file_map(const char *path);
void file_unmap(file_map_t *map);

/**
 * @brief 列出目录内容 (按目录优先、字母排序)
 * @param path        目录路径
 * @param entries     [out] 条目数组
 * @param max_entries 最大条目数
 * @return 实际条目数, <0: 错误
 */
int  file_list_dir(const char *path, dir_entry_t *entries, int max_entries);

/**
 * @brief 读取文本文件全部内容到 malloc 缓冲区
 * @param path  文件路径
 * @param out_len [out] 文件长度 (不含末尾\0), 可为 NULL
 * @return malloc 分配的字符串 (调用者需 free), 失败返回 NULL
 */
char *file_read_text(const char *path, size_t *out_len);

/**
 * @brief 根据扩展名判断文件类型
 */
file_type_t file_get_type(const char *name);

/**
 * @brief 根据文件类型返回 LVGL 符号图标
 */
const char *file_type_icon(file_type_t type);

#endif
