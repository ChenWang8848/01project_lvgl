#ifndef MUSIC_SERVICE_H
#define MUSIC_SERVICE_H

typedef enum {
    MUSIC_STATE_STOPPED,
    MUSIC_STATE_PLAYING,
    MUSIC_STATE_PAUSED
} music_state_t;

int  music_service_init(void);
int  music_play(const char *path);
int  music_pause(void);
int  music_resume(void);
int  music_stop(void);
music_state_t music_get_state(void);

#endif
