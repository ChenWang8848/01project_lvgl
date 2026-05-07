#include "app/app.h"
#include <stdint.h>
#include <sys/time.h>

uint32_t custom_tick_get(void)
{
    static uint64_t start_ms = 0;
    if (start_ms == 0) {
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);
        start_ms = (tv_start.tv_sec * 1000000 + tv_start.tv_usec) / 1000;
    }

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    uint64_t now_ms;
    now_ms = (tv_now.tv_sec * 1000000 + tv_now.tv_usec) / 1000;

    uint32_t time_ms = now_ms - start_ms;
    return time_ms;
}

int main(int argc, char *argv[])
{
    app_controller_t app;

    if (app_controller_init(&app, argc, argv) != 0)
        return -1;

    app_controller_run(&app);

    app_controller_cleanup(&app);
    return 0;
}
