#ifndef CONFIG_H
#define CONFIG_H

/* Display */
#define DISP_HOR_RES  800
#define DISP_VER_RES  480
#define DISP_BUF_SIZE (DISP_HOR_RES * DISP_VER_RES)

/* Input device paths */
#define TOUCHSCREEN_DEV "/dev/input/event2"
#define MOUSE_DEV       "/dev/input/event3"

/* Audio */
#define AUDIO_SAMPLE_RATE  44100
#define AUDIO_CHANNELS     2
#define AUDIO_BITS         16

/* Debug */
#define DEBUG_ENABLE 1
#define DEBUG_UDP_PORT 12345

#endif
