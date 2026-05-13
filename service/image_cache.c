/**
 * @file    image_cache.c
 * @brief   图片预加载缓存实现 — pthread_mutex + LRU 淘汰
 */

#include "image_cache.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

static cache_slot_t g_slots[CACHE_SLOTS];
static int g_next_evict = 0;    /* 轮转淘汰指针 */
static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

void image_cache_init(void)
{
    pthread_mutex_lock(&g_mutex);
    memset(g_slots, 0, sizeof(g_slots));
    pthread_mutex_unlock(&g_mutex);
}

void image_cache_destroy(void)
{
    pthread_mutex_lock(&g_mutex);
    for (int i = 0; i < CACHE_SLOTS; i++) {
        if (g_slots[i].pixels) {
            free(g_slots[i].pixels);
            g_slots[i].pixels = NULL;
        }
    }
    memset(g_slots, 0, sizeof(g_slots));
    pthread_mutex_unlock(&g_mutex);
}

uint8_t *image_cache_lookup(const char *path, uint32_t *w, uint32_t *h, uint8_t *cf)
{
    pthread_mutex_lock(&g_mutex);
    for (int i = 0; i < CACHE_SLOTS; i++) {
        if (g_slots[i].ready &&
            g_slots[i].pixels &&
            strcmp(g_slots[i].path, path) == 0) {
            *w  = g_slots[i].w;
            *h  = g_slots[i].h;
            *cf = g_slots[i].cf;
            uint8_t *px = g_slots[i].pixels;
            pthread_mutex_unlock(&g_mutex);
            return px;
        }
    }
    pthread_mutex_unlock(&g_mutex);
    return NULL;
}

void image_cache_put(const char *path, uint8_t *pixels, uint32_t w, uint32_t h, uint8_t cf)
{
    if (!pixels) return;

    pthread_mutex_lock(&g_mutex);

    /* 先检查是否已有相同路径 (避免重复) */
    for (int i = 0; i < CACHE_SLOTS; i++) {
        if (strcmp(g_slots[i].path, path) == 0) {
            /* 更新已有槽 */
            if (g_slots[i].pixels) free(g_slots[i].pixels);
            g_slots[i].pixels = pixels;
            g_slots[i].w       = w;
            g_slots[i].h       = h;
            g_slots[i].cf      = cf;
            g_slots[i].ready   = 1;
            pthread_mutex_unlock(&g_mutex);
            return;
        }
    }

    /* 找空闲槽或轮转淘汰 */
    int slot = -1;
    for (int i = 0; i < CACHE_SLOTS; i++) {
        if (!g_slots[i].pixels || !g_slots[i].ready) {
            slot = i;
            break;
        }
    }
    if (slot < 0) {
        /* 全部满: 轮转淘汰 */
        free(g_slots[g_next_evict].pixels);
        slot = g_next_evict;
        g_next_evict = (g_next_evict + 1) % CACHE_SLOTS;
    }

    snprintf(g_slots[slot].path, sizeof(g_slots[slot].path), "%s", path);
    g_slots[slot].pixels = pixels;
    g_slots[slot].w      = w;
    g_slots[slot].h      = h;
    g_slots[slot].cf     = cf;
    g_slots[slot].ready  = 1;

    pthread_mutex_unlock(&g_mutex);
}
