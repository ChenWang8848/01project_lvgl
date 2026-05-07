#ifndef FILE_SERVICE_H
#define FILE_SERVICE_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
    void   *addr;
    size_t  size;
} file_map_t;

typedef struct {
    char name[256];
    int  is_dir;
} dir_entry_t;

int  file_service_init(void);
file_map_t *file_map(const char *path);
void file_unmap(file_map_t *map);
int  file_list_dir(const char *path, dir_entry_t *entries, int max_entries);

#endif
