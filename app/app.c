#include "app.h"
#include "page_manager.h"
#include "lvgl/lvgl.h"
#include <unistd.h>

/* HAL */
#include "hal/display/fb_driver.h"
#include "hal/display/lvgl_display.h"
#include "hal/input/touchscreen.h"
#include "hal/input/mouse.h"
#include "hal/input/lvgl_input.h"
#include "hal/audio/alsa_output.h"
#include "hal/audio/mp3_decoder.h"

/* Service */
#include "service/file_service.h"
#include "service/music_service.h"
#include "service/image_service.h"

/* UI */
#include "ui/styles.h"
#include "ui/screens/main_screen.h"
#include "ui/screens/browse_screen.h"
#include "ui/screens/manual_screen.h"
#include "ui/screens/auto_screen.h"
#include "ui/screens/setting_screen.h"
#include "ui/screens/interval_screen.h"
#include "ui/screens/text_screen.h"
#include "ui/screens/music_screen.h"

/* Util */
#include "util/debug.h"

/* --- singleton holder for HAL objects (lifetime = app) --- */
static fb_driver_t    g_fb;
static lvgl_display_t g_disp;

int app_controller_init(app_controller_t *self, int argc, char *argv[])
{
    (void)argc; (void)argv;

    debug_init();
    LOG_INFO("app_controller_init: starting...");

    /* 1. LVGL core */
    lv_init();

    /* 2. HAL — display */
    fb_driver_init(&g_fb, "/dev/fb0");
    if (g_fb.base.init(&g_fb.base) != 0) {
        LOG_ERROR("Failed to init framebuffer driver");
        return -1;
    }

    if (lvgl_display_init(&g_disp, &g_fb.base) != 0) {
        LOG_ERROR("Failed to init LVGL display");
        return -1;
    }

    /* HAL — input (stubs for now) */
    lvgl_input_init_all();

    /* 3. Services (stubs for now) */
    file_service_init();
    music_service_init();
    image_service_init();

    /* 4. UI — global styles */
    styles_init();

    /* 5. App — register screens, navigate to main */
    page_manager_init();

    static main_screen_t    scr_main;
    static browse_screen_t  scr_browse;
    static manual_screen_t  scr_manual;
    static auto_screen_t    scr_auto;
    static setting_screen_t scr_setting;
    static interval_screen_t scr_interval;
    static text_screen_t    scr_text;
    static music_screen_t   scr_music;

    main_screen_init(&scr_main);
    browse_screen_init(&scr_browse);
    manual_screen_init(&scr_manual);
    auto_screen_init(&scr_auto);
    setting_screen_init(&scr_setting);
    interval_screen_init(&scr_interval);
    text_screen_init(&scr_text);
    music_screen_init(&scr_music);

    page_manager_register(&scr_main.base);
    page_manager_register(&scr_browse.base);
    page_manager_register(&scr_manual.base);
    page_manager_register(&scr_auto.base);
    page_manager_register(&scr_setting.base);
    page_manager_register(&scr_interval.base);
    page_manager_register(&scr_text.base);
    page_manager_register(&scr_music.base);

    page_manager_navigate("main", NULL);

    self->initialized = 1;
    LOG_INFO("app_controller_init: done");
    return 0;
}

void app_controller_run(app_controller_t *self)
{
    if (!self->initialized) return;

    while (1) {
        lv_timer_handler();
        usleep(5000);
    }
}

void app_controller_cleanup(app_controller_t *self)
{
    (void)self;
    lvgl_display_deinit(&g_disp);
    g_fb.base.deinit(&g_fb.base);
}
