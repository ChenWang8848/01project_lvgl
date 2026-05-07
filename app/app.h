#ifndef APP_H
#define APP_H

typedef struct {
    int initialized;
} app_controller_t;

int  app_controller_init(app_controller_t *self, int argc, char *argv[]);
void app_controller_run(app_controller_t *self);
void app_controller_cleanup(app_controller_t *self);

#endif
