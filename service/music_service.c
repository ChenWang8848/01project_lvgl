#include "music_service.h"

int music_service_init(void) { return 0; }
int music_play(const char *path)  { (void)path; return 0; }
int music_pause(void)  { return 0; }
int music_resume(void) { return 0; }
int music_stop(void)   { return 0; }
music_state_t music_get_state(void) { return MUSIC_STATE_STOPPED; }
