/**
 * @file    file_service.c
 * @brief   文件服务实现 — POSIX 目录遍历 + mmap 文件读写
 */

#include "file_service.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include "lvgl/lvgl.h"

/* ---- path 缓冲区最大长度 ---- */
#define PATH_MAX_LEN 1024

/* ================================================================
 *  文件类型检测
 * ================================================================ */

file_type_t file_get_type(const char *name)
{
    /* 目录 */
    /* is_dir 在 list_dir 中通过 stat 判断，此函数仅按扩展名分类 */

    const char *ext = strrchr(name, '.');
    if (!ext) return FILE_TYPE_OTHER;

    /* 图片 */
    if (strcasecmp(ext, ".jpg")  == 0 ||
        strcasecmp(ext, ".jpeg") == 0 ||
        strcasecmp(ext, ".png")  == 0 ||
        strcasecmp(ext, ".bmp")  == 0 ||
        strcasecmp(ext, ".gif")  == 0)
        return FILE_TYPE_IMAGE;

    /* 音乐 */
    if (strcasecmp(ext, ".mp3")  == 0 ||
        strcasecmp(ext, ".wav")  == 0 ||
        strcasecmp(ext, ".ogg")  == 0 ||
        strcasecmp(ext, ".flac") == 0 ||
        strcasecmp(ext, ".aac")  == 0)
        return FILE_TYPE_MUSIC;

    /* 文本 */
    if (strcasecmp(ext, ".txt")  == 0 ||
        strcasecmp(ext, ".log")  == 0 ||
        strcasecmp(ext, ".cfg")  == 0 ||
        strcasecmp(ext, ".conf") == 0 ||
        strcasecmp(ext, ".ini")  == 0 ||
        strcasecmp(ext, ".sh")   == 0 ||
        strcasecmp(ext, ".c")    == 0 ||
        strcasecmp(ext, ".h")    == 0 ||
        strcasecmp(ext, ".py")   == 0 ||
        strcasecmp(ext, ".md")   == 0)
        return FILE_TYPE_TEXT;

    return FILE_TYPE_OTHER;
}

const char *file_type_icon(file_type_t type)
{
    switch (type) {
        case FILE_TYPE_DIR:   return LV_SYMBOL_DIRECTORY;
        case FILE_TYPE_IMAGE: return LV_SYMBOL_IMAGE;
        case FILE_TYPE_MUSIC: return LV_SYMBOL_AUDIO;
        case FILE_TYPE_TEXT:  return LV_SYMBOL_LIST;
        default:              return LV_SYMBOL_FILE;
    }
}

/* ================================================================
 *  辅助: 目录条目排序比较 (目录优先, 字母序)
 * ================================================================ */

static int entry_cmp(const void *a, const void *b)
{
    const dir_entry_t *ea = (const dir_entry_t *)a;
    const dir_entry_t *eb = (const dir_entry_t *)b;
    /* 目录排在文件前面 */
    if (ea->is_dir && !eb->is_dir) return -1;
    if (!ea->is_dir && eb->is_dir) return  1;
    /* 同类型按名称排序 */
    return strcasecmp(ea->name, eb->name);
}

/* ================================================================
 *  目录遍历
 * ================================================================ */

int file_list_dir(const char *path, dir_entry_t *entries, int max_entries)
{
    DIR *dir = opendir(path);
    if (!dir) {
        fprintf(stderr, "file_list_dir: opendir(%s) failed: %s\n",
                path, strerror(errno));
        return -1;
    }

    int count = 0;
    struct dirent *de;
    char full_path[PATH_MAX_LEN];

    while ((de = readdir(dir)) != NULL && count < max_entries) {
        /* 跳过隐藏文件 (以 . 开头的文件/目录不显示) */
        if (de->d_name[0] == '.') continue;

        /* 拼接完整路径用于 stat */
        snprintf(full_path, sizeof(full_path), "%s/%s", path, de->d_name);

        struct stat st;
        if (stat(full_path, &st) != 0) continue;

        /* 只处理目录和普通文件 */
        if (!S_ISDIR(st.st_mode) && !S_ISREG(st.st_mode)) continue;

        /* 填充条目 */
        strncpy(entries[count].name, de->d_name, sizeof(entries[count].name) - 1);
        entries[count].is_dir = S_ISDIR(st.st_mode) ? 1 : 0;
        entries[count].type   = entries[count].is_dir
                               ? FILE_TYPE_DIR : file_get_type(de->d_name);
        count++;
    }

    closedir(dir);

    /* 排序: 目录优先, 然后字母序 */
    qsort(entries, count, sizeof(dir_entry_t), entry_cmp);

    return count;
}

/* ================================================================
 *  读取文本文件
 * ================================================================ */
char *file_read_text(const char *path, size_t *out_len)
{
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        fprintf(stderr, "file_read_text: fopen(%s) failed: %s\n",
                path, strerror(errno));
        return NULL;
    }

    /* 获取文件大小 */
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (fsize <= 0) { fclose(fp); return NULL; }

    /* 分配缓冲区 (+1 for null terminator) */
    char *buf = malloc(fsize + 1);
    if (!buf) { fclose(fp); return NULL; }

    size_t nread = fread(buf, 1, fsize, fp);
    fclose(fp);

    if (nread <= 0) { free(buf); return NULL; }

    buf[nread] = '\0';
    if (out_len) *out_len = nread;
    return buf;
}

/* ================================================================
 *  单例初始化 / mmap 文件映射 (暂未实现)
 * ================================================================ */

int file_service_init(void)
{
    return 0;
}

file_map_t *file_map(const char *path)
{
    (void)path;
    return NULL;
}

void file_unmap(file_map_t *map)
{
    (void)map;
}
