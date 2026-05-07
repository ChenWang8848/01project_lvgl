#include "file_service.h"

int file_service_init(void)
{
    return 0; /* stub */
}

file_map_t *file_map(const char *path)
{
    (void)path;
    return NULL; /* stub */
}

void file_unmap(file_map_t *map)
{
    (void)map;
}

int file_list_dir(const char *path, dir_entry_t *entries, int max_entries)
{
    (void)path;
    (void)entries;
    (void)max_entries;
    return 0; /* stub */
}
