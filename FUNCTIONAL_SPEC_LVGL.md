# digitpic LVGL 重构功能说明书

> 本文档按照实现顺序，逐步描述每个模块的功能、接口、数据结构和关键实现细节。可按照章节顺序逐一完成开发。

---

## 目录

- [第一阶段：项目骨架搭建](#第一阶段项目骨架搭建)
- [第二阶段：LVGL 环境集成](#第二阶段lvgl-环境集成)
- [第三阶段：显示驱动 HAL](#第三阶段显示驱动-hal)
- [第四阶段：输入驱动 HAL](#第四阶段输入驱动-hal)
- [第五阶段：工具层](#第五阶段工具层)
- [第六阶段：文件服务](#第六阶段文件服务)
- [第七阶段：UI 全局样式](#第七阶段ui-全局样式)
- [第八阶段：自定义控件](#第八阶段自定义控件)
- [第九阶段：基础页面实现](#第九阶段基础页面实现)
- [第十阶段：图片查看器页面](#第十阶段图片查看器页面)
- [第十一阶段：文件浏览器页面](#第十一阶段文件浏览器页面)
- [第十二阶段：幻灯片与设置页面](#第十二阶段幻灯片与设置页面)
- [第十三阶段：音乐播放子系统](#第十三阶段音乐播放子系统)
- [第十四阶段：文本阅读器页面](#第十四阶段文本阅读器页面)
- [第十五阶段：应用层集成与联调](#第十五阶段应用层集成与联调)

---

## 第一阶段：项目骨架搭建

### 目标

建立完整的目录结构、公共编译配置、全局配置文件，确保空壳可以编译通过并生成空白的 `digitpic` 可执行文件。

### 1.1 创建目录结构

在项目根目录 `digitpic-lvgl/` 下创建以下全部文件夹：

```bash
mkdir -p app
mkdir -p ui/screens ui/widgets
mkdir -p service
mkdir -p hal/display hal/input hal/audio
mkdir -p util
mkdir -p config
mkdir -p lib
mkdir -p assets/fonts assets/icons
```

### 1.2 config/config.h — 全局配置文件

**功能**：定义整个项目的编译时常量、屏幕参数、颜色值、按键映射、内存限制等。

**需定义的核心宏**：

```c
#ifndef CONFIG_H
#define CONFIG_H

/* ====== 屏幕默认参数 ====== */
#define DEFAULT_SCREEN_WIDTH   1024
#define DEFAULT_SCREEN_HEIGHT  600
#define DEFAULT_SCREEN_BPP     32        // ARGB8888

/* ====== LVGL 参数（与 lv_conf.h 配合） ====== */
#define LVGL_DRAW_BUF_SIZE     (DEFAULT_SCREEN_WIDTH * 60)  // 双缓冲行数

/* ====== 颜色定义（16 进制 RGB） ====== */
#define COLOR_BACKGROUND      0x1A1A2E
#define COLOR_FOREGROUND      0xE94560
#define COLOR_TEXT_PRIMARY    0xFFFFFF
#define COLOR_TEXT_SECONDARY  0xB0B0B0
#define COLOR_BUTTON_NORMAL   0x16213E
#define COLOR_BUTTON_PRESSED  0x0F3460
#define COLOR_BUTTON_FOCUS    0x533483
#define COLOR_PROGRESS_BG     0x333333
#define COLOR_PROGRESS_FG     0xE94560

/* ====== 字体参数 ====== */
#define DEFAULT_FONT_PATH     "/assets/fonts/MSYH.TTF"
#define DEFAULT_FONT_SIZE     16
#define TITLE_FONT_SIZE       24
#define ICON_FONT_SIZE        12

/* ====== 布局参数 ====== */
#define MENU_ICON_SIZE        64
#define MENU_GAP              20
#define BUTTON_HEIGHT         48
#define TOP_BAR_HEIGHT        40
#define BOTTOM_BAR_HEIGHT     50

/* ====== 文件系统限制 ====== */
#define MAX_FILE_PATH_LEN     1024
#define MAX_DIR_DEPTH         10
#define MAX_DIR_ENTRIES       512
#define MAX_FILE_MAP_SIZE     (50 * 1024 * 1024)  // 最大 mmap 文件 50MB

/* ====== 输入事件类型 ====== */
#define INPUT_TYPE_TOUCH      1
#define INPUT_TYPE_MOUSE      2
#define INPUT_TYPE_KEY        3

/* ====== 音乐播放参数 ====== */
#define DEFAULT_VOLUME        80
#define VOLUME_STEP           5
#define PROGRESS_BAR_WIDTH    400
#define PROGRESS_BAR_HEIGHT   20

/* ====== 调试开关 ====== */
#define DEBUG_ENABLE          1
#define DEBUG_UDP_PORT        8888
#define DEBUG_UDP_ADDR        "192.168.1.100"

#endif /* CONFIG_H */
```

### 1.3 common.mk — 公共编译文件

**文件位置**：`digitpic-lvgl/common.mk`

**功能**：定义交叉编译工具链、头文件路径、编译选项，被每个子目录的 Makefile 通过 `include` 引用。

```makefile
# 交叉编译工具链
CROSS_COMPILE := arm-linux-gnueabihf-
CC  := $(CROSS_COMPILE)gcc
AR  := $(CROSS_COMPILE)ar

# 自动推断项目根目录
TOP_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))

# 第三方库路径
LVGL_DIR     := $(TOP_DIR)/lib/lvgl
FREETYPE_DIR := $(TOP_DIR)/lib/freetype-2.14.3
TSLIB_DIR    := $(TOP_DIR)/lib/tslib
JPEG_DIR     := $(TOP_DIR)/lib/libjpeg-turbo-1.2.1
MAD_DIR      := $(TOP_DIR)/lib/libmad-0.15.1b
ALSA_DIR     := $(TOP_DIR)/lib/alsa-lib-1.2.7

# 头文件搜索路径（含所有业务目录）
INCLUDES := \
	-I$(TOP_DIR)/config \
	-I$(TOP_DIR)/app \
	-I$(TOP_DIR)/ui -I$(TOP_DIR)/ui/screens -I$(TOP_DIR)/ui/widgets \
	-I$(TOP_DIR)/service \
	-I$(TOP_DIR)/hal/display -I$(TOP_DIR)/hal/input -I$(TOP_DIR)/hal/audio \
	-I$(TOP_DIR)/util \
	-I$(LVGL_DIR) \
	-I$(FREETYPE_DIR)/include \
	-I$(TSLIB_DIR)/include \
	-I$(JPEG_DIR) \
	-I$(MAD_DIR) \
	-I$(ALSA_DIR)/include

CFLAGS := -Wall -O2 -g -std=gnu99 $(INCLUDES)

# 链接选项（仅顶层 Makefile 使用）
LDFLAGS := -L$(FREETYPE_DIR) -L$(TSLIB_DIR)/lib \
           -L$(JPEG_DIR) -L$(MAD_DIR) -L$(ALSA_DIR)/lib
LDLIBS  := -lfreetype -lts -ljpeg -lmad -lasound -lm -lpthread
```

### 1.4 顶层 Makefile

见架构文档 6.1 节，此处省略重复内容。

### 1.5 各子目录 Makefile

见架构文档 6.2 ~ 6.12 节。

### 1.6 main.c — 最小可编译入口

```c
#include <stdio.h>
#include "config.h"

int main(int argc, char *argv[])
{
    printf("digitpic-lvgl startup...\n");
    printf("Screen: %dx%d @ %dbpp\n",
           DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, DEFAULT_SCREEN_BPP);
    printf("Font path: %s, size: %d\n", DEFAULT_FONT_PATH, DEFAULT_FONT_SIZE);
    return 0;
}
```

### 1.7 第一阶段验证

```bash
make clean && make
```

应成功编译出 `digitpic` 可执行文件，运行后打印配置信息。

---

## 第二阶段：LVGL 环境集成

### 目标

将 LVGL 库编译进项目，创建最小的 LVGL 初始化流程，在屏幕上显示纯色背景作为验证。

### 2.1 准备 LVGL 库

将 LVGL 源码放置在 `lib/lvgl/` 目录下。推荐 LVGL v8.3.x 或 v8.4.x（ARM Linux 兼容性最佳）。

### 2.2 lv_conf.h 配置

从 `lib/lvgl/lv_conf_template.h` 复制为 `lib/lvgl/lv_conf.h`，修改以下关键项：

```c
#define LV_COLOR_DEPTH          32           // 与 DEFAULT_SCREEN_BPP 一致
#define LV_MEM_SIZE             (4 * 1024 * 1024)  // 4MB LVGL heap
#define LV_TICK_CUSTOM          1            // 使用自定义 tick
#define LV_TICK_CUSTOM_INCLUDE  <stdint.h>
#define LV_TICK_CUSTOM_SYS_TIME_EXPR (lv_custom_tick_get())
#define LV_USE_LOG              1
#define LV_USE_FREETYPE         1            // 启用 FreeType 字体
#define LV_FREETYPE_CACHE_SIZE  (4 * 1024 * 1024)
#define LV_USE_FS_STDIO         1            // 文件系统使用标准 IO（可选）
```

### 2.3 实现自定义 tick

LVGL 需要 1ms 精度的时间基准。在 `util/` 中实现。

**文件**：`util/tick.h`、`util/tick.c`

```c
// util/tick.h
#ifndef TICK_H
#define TICK_H
#include <stdint.h>

int  tick_init(void);
uint32_t lv_custom_tick_get(void);   // LVGL 调用的 tick 函数
void tick_deinit(void);
#endif
```

```c
// util/tick.c
#include "tick.h"
#include <unistd.h>
#include <time.h>
#include <sys/timerfd.h>

static int timer_fd = -1;

int tick_init(void)
{
    timer_fd = timerfd_create(CLOCK_MONOTONIC, 0);
    if (timer_fd < 0) return -1;

    struct itimerspec its = {
        .it_interval = { .tv_sec = 0, .tv_nsec = 1000000 },  // 1ms
        .it_value    = { .tv_sec = 0, .tv_nsec = 1000000 },
    };
    timerfd_settime(timer_fd, 0, &its, NULL);
    return 0;
}

uint32_t lv_custom_tick_get(void)
{
    static uint32_t tick = 0;
    if (timer_fd >= 0) {
        uint64_t expirations;
        ssize_t n = read(timer_fd, &expirations, sizeof(expirations));
        if (n > 0) tick += (uint32_t)expirations;
    }
    return tick;
}

void tick_deinit(void)
{
    if (timer_fd >= 0) { close(timer_fd); timer_fd = -1; }
}
```

### 2.4 创建占位源文件

为确保 Makefile 能链接成功，在所有源目录下创建最小占位文件。

**util/list.h + util/list.c**（占位）：
```c
// util/list.h
#ifndef LIST_H
#define LIST_H
// 第三阶段实现
#endif
```

```c
// util/list.c
#include "list.h"
// 第三阶段实现
```

**util/debug.h + util/debug.c**（占位）：
```c
// util/debug.h
#ifndef DEBUG_H
#define DEBUG_H
#include <stdio.h>
#define DBG_PRINTF(fmt, ...) printf("[DEBUG] " fmt "\n", ##__VA_ARGS__)
#endif
```

**其余各模块的 .h 和 .c 文件**均创建为带有空函数体的占位文件（头文件声明必要函数，.c 文件提供空实现），确保链接不报错。

具体占位接口列表（按模块）：

| 文件 | 需声明的函数/类型 |
|---|---|
| `hal/display/display_driver.h` | `display_driver_t` 结构体（见第三阶段） |
| `hal/display/fb_driver.h` | `int fb_driver_init(void)` |
| `hal/display/lvgl_display.h` | `int lvgl_display_init(void)` |
| `hal/input/input_driver.h` | `input_driver_t` 结构体（见第四阶段） |
| `hal/input/lvgl_input.h` | `int lvgl_input_init(void)` |
| `hal/audio/audio_output.h` | `audio_output_t` 结构体（见第十三阶段） |
| `hal/audio/alsa_output.h` | `int alsa_output_init(void)` |
| `hal/audio/mp3_decoder.h` | `int mp3_decoder_init(void)` |
| `service/file_service.h` | `int file_service_init(void)` |
| `service/music_service.h` | `int music_service_init(void)` |
| `service/image_service.h` | `int image_service_init(void)` |
| `ui/styles.h` | `void styles_init(void)` |
| `ui/screens/base_screen.h` | `base_screen_t` 结构体（见第九阶段） |
| `app/app.h` | `int app_init(int, char**)`、`void app_run(void)` |
| `app/page_manager.h` | `int page_manager_init(void)` |

### 2.5 第二阶段验证

```bash
make clean && make
```

编译成功后，LVGL 库应与所有占位模块链接成功。当前 `main.c` 不调用 LVGL，仅验证链接。

---

## 第三阶段：显示驱动 HAL

### 目标

实现 Linux framebuffer 驱动 + LVGL 显示适配，使 LVGL 能够在屏幕上渲染内容。验证方式：屏幕显示纯色背景（红/绿/蓝交替）。

### 3.1 display_driver.h — 显示驱动虚基类

```c
#ifndef DISPLAY_DRIVER_H
#define DISPLAY_DRIVER_H

#include <stdint.h>
#include "lvgl.h"

typedef struct display_driver {
    const char *name;

    /* 初始化显示设备，返回 0 成功 */
    int (*init)(struct display_driver *self);

    /* 获取屏幕分辨率 */
    void (*get_resolution)(struct display_driver *self,
                           int32_t *width, int32_t *height);

    /* 获取色深（BPP） */
    int (*get_bpp)(struct display_driver *self);

    /* 获取一行像素的字节数 */
    int32_t (*get_stride)(struct display_driver *self);

    /* LVGL flush 回调：将 area 区域内的像素写入显示设备 */
    void (*flush)(lv_display_t *disp, const lv_area_t *area,
                  uint8_t *px_map);

    /* 设备私有数据 */
    void *priv;
} display_driver_t;

#endif
```

### 3.2 fb_driver.h / fb_driver.c — Framebuffer 驱动

**功能**：
- 打开 `/dev/fb0` 设备
- 通过 `ioctl(FBIOGET_VSCREENINFO)` 获取屏幕信息
- 通过 `mmap` 映射 framebuffer 到用户空间
- 提供 LVGL flush 回调所需的像素写入能力

**fb_driver.h**：
```c
#ifndef FB_DRIVER_H
#define FB_DRIVER_H
#include "display_driver.h"

/* 创建并初始化 framebuffer 显示驱动，返回 display_driver_t 指针 */
display_driver_t *fb_driver_create(void);

/* 销毁驱动 */
void fb_driver_destroy(display_driver_t *drv);
#endif
```

**fb_driver.c 核心数据结构**：
```c
typedef struct {
    int  fb_fd;                    // /dev/fb0 文件描述符
    char *fb_path;                 // 设备路径，默认 "/dev/fb0"
    uint8_t *fb_mem;              // mmap 映射的 framebuffer 内存
    uint32_t fb_size;             // mmap 大小
    int32_t width;                // 屏幕宽度
    int32_t height;               // 屏幕高度
    int32_t stride;               // 每行字节数
    int bpp;                      // 色深（bits per pixel）
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
} fb_priv_t;
```

**fb_driver.c 关键函数实现逻辑**：

```
fb_init(self):
  1. 获取 fb_priv_t *priv = self->priv
  2. open(priv->fb_path, O_RDWR) → 保存 fb_fd
  3. ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo) → 获取分辨率/BPP
  4. ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo) → 获取 stride/内存大小
  5. 将分辨率/BPP/stride 存入 priv
  6. mmap(NULL, finfo.smem_len, PROT_READ|PROT_WRITE,
          MAP_SHARED, fb_fd, 0) → 保存 fb_mem
  7. 返回 0

fb_get_resolution(self, *w, *h):
  1. 从 priv 读取并返回 width/height

fb_get_bpp(self):
  1. 从 priv 读取并返回 bpp

fb_flush(disp, area, px_map):
  1. 获取 fb_priv_t *priv = display_driver_t->priv
  2. 逐行 memcpy：
     for y = area->y1 to area->y2:
       dst = priv->fb_mem + y * priv->stride + area->x1 * (priv->bpp/8)
       src = px_map + (y - area->y1) * area_width * (priv->bpp/8)
       memcpy(dst, src, bytes_per_line)
  3. lv_display_flush_ready(disp)  // 通知 LVGL 刷新完成
```

**注意**：flush 回调中需要正确处理 `px_map` 的像素格式转换。如果 fb 是 RGB565 但 LVGL 输出 ARGB8888，需要逐像素转换。建议统一使用 32bpp 避免转换开销。

### 3.3 lvgl_display.h / lvgl_display.c — LVGL 显示适配

**功能**：创建 LVGL 的 `lv_display_t` 对象，绑定 flush 回调，配置 draw buffer。

```c
// lvgl_display.h
#ifndef LVGL_DISPLAY_H
#define LVGL_DISPLAY_H
#include <stdint.h>

int  lvgl_display_init(display_driver_t *drv);
void lvgl_display_deinit(void);
#endif
```

**lvgl_display.c 实现逻辑**：

```
lvgl_display_init(drv):
  1. 从 drv 获取分辨率 width / height / bpp
  2. lv_display_t *disp = lv_display_create(width, height)
  3. lv_display_set_flush_cb(disp, drv->flush)     // 绑定 flush 回调
  4. lv_display_set_color_format(disp, LV_COLOR_FORMAT_ARGB8888)
  5. 分配 draw buffer：
     buf_size = width * 60 * (bpp/8)   // 缓冲 60 行
     buf1 = malloc(buf_size)
     buf2 = malloc(buf_size)           // 双缓冲
     lv_display_set_buffers(disp, buf1, buf2, buf_size,
                            LV_DISPLAY_RENDER_MODE_PARTIAL)
  6. 设置默认屏幕背景色为黑色：
     lv_obj_t *scr = lv_screen_active()
     lv_obj_set_style_bg_color(scr, lv_color_black(), 0)
  7. 返回 0
```

### 3.4 hal/display/Makefile 更新

```makefile
include ../../common.mk
SRCS := fb_driver.c lvgl_display.c
OBJS := $(SRCS:.c=.o)
TARGET := built-in.a

all: $(TARGET)
$(TARGET): $(OBJS)
	$(AR) rcs $@ $^
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f *.o *.a *.d
```

### 3.5 第三阶段验证

修改 `main.c`：
```c
int main(int argc, char *argv[])
{
    lv_init();
    tick_init();

    display_driver_t *fb = fb_driver_create();
    fb->init(fb);

    lvgl_display_init(fb);

    /* 创建纯红色背景 screen 验证 */
    lv_obj_t *scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0xFF0000), 0);

    /* 最小 LVGL 循环 */
    while (1) {
        lv_timer_handler();
        usleep(5000);
    }
    return 0;
}
```

屏幕应显示纯红色。

---

## 第四阶段：输入驱动 HAL

### 目标

实现触摸屏和鼠标的 LVGL 输入设备驱动，使用户能通过触摸/点击与 LVGL 控件交互。

### 4.1 input_driver.h — 输入设备虚基类

```c
#ifndef INPUT_DRIVER_H
#define INPUT_DRIVER_H

#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"

typedef enum {
    INPUT_TYPE_TOUCHSCREEN = 0,
    INPUT_TYPE_MOUSE,
    INPUT_TYPE_KEYBOARD,
} input_device_type_t;

typedef struct {
    int x;                        // 触摸点 X（-1 表示未按下）
    int y;                        // 触摸点 Y
    bool pressed;                 // 是否按下
} touch_data_t;

typedef struct {
    int x;                        // 鼠标 X
    int y;                        // 鼠标 Y
    bool left_pressed;            // 左键是否按下
    int wheel;                    // 滚轮增量
} mouse_data_t;

typedef struct input_driver {
    const char *name;
    input_device_type_t type;

    /* 初始化设备 */
    int (*init)(struct input_driver *self);

    /* 读取设备数据（非阻塞），返回 true 表示有新数据 */
    bool (*read)(struct input_driver *self, void *data);

    /* 关闭设备 */
    void (*deinit)(struct input_driver *self);

    void *priv;
} input_driver_t;

#endif
```

### 4.2 touchscreen.h / touchscreen.c — 触摸屏驱动

**功能**：
- 打开 tslib 设备（`/dev/input/event1`）
- 读取触摸坐标和压力值
- 封装为 LVGL indev 的 read 回调

**数据结构**：
```c
typedef struct {
    struct tsdev *ts;             // tslib 设备句柄
    char *ts_path;               // 默认 "/dev/input/event1"
    int screen_width;            // 屏幕宽度（用于坐标裁剪）
    int screen_height;           // 屏幕高度
    int last_x, last_y;
    bool last_pressed;
} touchscreen_priv_t;
```

**read 函数逻辑**：
```
touchscreen_read(self, data):
  1. 获取 touchscreen_priv_t *priv = self->priv
  2. ts_read(priv->ts, &sample, 1)  // 非阻塞读取
  3. 如果读到数据：
     data->x = sample.x
     data->y = sample.y
     data->pressed = (sample.pressure > 0)
     如果坐标超出屏幕范围，裁剪到合法范围
     返回 true
  4. 否则返回 false
```

### 4.3 mouse.h / mouse.c — 鼠标驱动

**功能**：
- 打开 `/dev/input/event3`（USB 鼠标设备）
- 解析 Linux `input_event` 结构体
- 跟踪鼠标绝对位置（累积相对位移），并裁剪到屏幕范围内

**数据结构**：
```c
typedef struct {
    int fd;
    char *dev_path;              // 默认 "/dev/input/event3"
    int abs_x, abs_y;            // 绝对坐标（累积值）
    bool left_pressed;
    int wheel;
    int screen_width, screen_height;
} mouse_priv_t;
```

**read 函数逻辑**：
```
mouse_read(self, data):
  1. 获取 mouse_priv_t *priv = self->priv
  2. 循环 read(fd, &ev, sizeof(struct input_event)) 直到 EAGAIN
  3. 根据 ev.type / ev.code / ev.value：
     - EV_REL + REL_X → priv->abs_x += ev.value，裁剪到 [0, width)
     - EV_REL + REL_Y → priv->abs_y += ev.value，裁剪到 [0, height)
     - EV_KEY + BTN_LEFT → priv->left_pressed = ev.value
     - EV_REL + REL_WHEEL → data->wheel = ev.value
  4. data->x = priv->abs_x
     data->y = priv->abs_y
     data->left_pressed = priv->left_pressed
  5. 返回 true（鼠标总是有数据）
```

**注意**：LVGL 的鼠标 indev 需要设置 `lv_indev_set_cursor()` 来显示光标。

### 4.4 lvgl_input.h / lvgl_input.c — LVGL 输入适配

**功能**：将 `input_driver_t` 包装为 LVGL 的 `lv_indev_t` 设备。

```c
#ifndef LVGL_INPUT_H
#define LVGL_INPUT_H

#include "input_driver.h"

/* 注册触摸屏设备 */
int lvgl_touchscreen_init(input_driver_t *drv);

/* 注册鼠标设备 */
int lvgl_mouse_init(input_driver_t *drv);

/* 注册键盘设备（预留） */
int lvgl_keyboard_init(input_driver_t *drv);

void lvgl_input_deinit(void);
#endif
```

**LVGL read 回调实现**：

```
// 触摸屏 read 回调
touchscreen_lvgl_read(indeve, data):
  1. 获取绑定的 input_driver_t *drv = indev->driver_data
  2. touch_data_t touch;
  3. if (drv->read(drv, &touch)):
       data->point.x = touch.x
       data->point.y = touch.y
       data->state   = touch.pressed ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL
  4. 否则：data->state = 无变化

// 鼠标 read 回调
mouse_lvgl_read(indev, data):
  1. 获取绑定的 input_driver_t *drv = indev->driver_data
  2. mouse_data_t mouse;
  3. drv->read(drv, &mouse)
  4. data->point.x = mouse.x
     data->point.y = mouse.y
     data->state   = mouse.left_pressed ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL
     data->enc_diff = mouse.wheel  // 滚轮
```

**lvgl_input_init 实现逻辑**：
```
lvgl_touchscreen_init(drv):
  1. lv_indev_t *indev = lv_indev_create()
  2. lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER)
  3. lv_indev_set_read_cb(indev, touchscreen_lvgl_read)
  4. indev->driver_data = drv       // 将驱动指针绑定到 indev
  5. 返回 0

lvgl_mouse_init(drv):
  1. lv_indev_t *indev = lv_indev_create()
  2. lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER)
  3. lv_indev_set_read_cb(indev, mouse_lvgl_read)
  4. indev->driver_data = drv
  5. 创建鼠标光标对象：lv_obj_t *cursor = lv_img_create(lv_screen_active())
     设置光标图像为小箭头图标（或内置图标）
     lv_indev_set_cursor(indev, cursor)
  6. 返回 0
```

### 4.5 第四阶段验证

修改 `main.c`，在 LVGL 循环前加入：

```c
// 创建触摸屏驱动
input_driver_t *touch = touchscreen_create();
touch->init(touch);
lvgl_touchscreen_init(touch);

// 创建鼠标驱动（可选，若设备无鼠标则跳过）
input_driver_t *mouse = mouse_create();
if (mouse->init(mouse) == 0) {
    lvgl_mouse_init(mouse);
}

// 创建一个 LVGL 按钮用于测试点击
lv_obj_t *btn = lv_btn_create(lv_screen_active());
lv_obj_set_size(btn, 200, 100);
lv_obj_center(btn);
lv_obj_t *label = lv_label_create(btn);
lv_label_set_text(label, "Click Me");
lv_obj_center(label);
lv_obj_add_event_cb(btn, btn_click_handler, LV_EVENT_CLICKED, NULL);
```

点击按钮时应有视觉反馈（按钮颜色变化）。

---

## 第五阶段：工具层

### 目标

实现通用工具模块：调试日志系统、双向链表数据结构。这些模块被后续各层依赖。

### 5.1 debug.h / debug.c — 调试日志系统

**功能**：
- 多通道日志输出（stdout + UDP 网络输出）
- 日志级别过滤（0~7，与原有系统兼容）
- 宏 `DBG_PRINTF` 提供便捷的格式化输出

**debug.h**：
```c
#ifndef DEBUG_H
#define DEBUG_H

#include <stdint.h>

#define DEBUG_LEVEL_EMERG   0
#define DEBUG_LEVEL_ALERT   1
#define DEBUG_LEVEL_CRIT    2
#define DEBUG_LEVEL_ERR     3
#define DEBUG_LEVEL_WARN    4
#define DEBUG_LEVEL_NOTICE  5
#define DEBUG_LEVEL_INFO    6
#define DEBUG_LEVEL_DEBUG   7

/* 全局日志级别，只输出 ≤ 此级别的日志 */
extern int g_debug_level;

typedef struct debug_channel {
    const char *name;
    int  enabled;                           // 1 = 启用
    int  (*init)(struct debug_channel *ch);
    void (*output)(struct debug_channel *ch, const char *msg);
    void (*deinit)(struct debug_channel *ch);
    void *priv;
} debug_channel_t;

/* 初始化调试系统 */
int  debug_init(void);

/* 注册一个调试输出通道 */
int  debug_register_channel(debug_channel_t *ch);

/* 设置全局日志级别 */
void debug_set_level(int level);

/* 发送调试消息到所有启用的通道 */
void debug_print(int level, const char *fmt, ...);

/* 便捷宏 */
#define DBG_PRINTF(level, fmt, ...) \
    debug_print(level, "[%s:%d] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)

#endif
```

**debug.c 实现逻辑**：
```
debug_print(level, fmt, ...):
  1. if (level > g_debug_level) return   // 级别过滤
  2. va_list args; va_start(args, fmt)
  3. vsnprintf(buffer, sizeof(buffer), fmt, args)
  4. 遍历所有注册的 debug_channel：
       if (ch->enabled) ch->output(ch, buffer)
  5. va_end(args)
```

**stdout 通道实现**：
```
stdout_output(ch, msg):
  printf("%s\n", msg)
  fflush(stdout)
```

**UDP 通道实现（可选）**：
```
netprint_output(ch, msg):
  sendto(sock_fd, msg, strlen(msg), 0,
         (struct sockaddr *)&addr, sizeof(addr))
```

### 5.2 list.h / list.c — 双向链表

从原有代码 `system/common_st.c` + `include/common_st.h` 迁移。

**list.h**：
```c
#ifndef LIST_H
#define LIST_H

#include <stddef.h>

/* 内核风格双向链表节点 */
typedef struct list_head {
    struct list_head *next;
    struct list_head *prev;
} list_head_t;

/* 静态初始化链表头 */
#define LIST_HEAD_INIT(head) { &(head), &(head) }
#define LIST_HEAD(name)      list_head_t name = LIST_HEAD_INIT(name)

/* 从链表节点指针获取包含它的结构体指针 */
#define list_entry(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

/* 遍历链表 */
#define list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

#define list_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); \
         pos = n, n = pos->next)

/* 操作函数 */
void list_init(list_head_t *head);
void list_add_tail(list_head_t *new_node, list_head_t *head);
void list_del(list_head_t *entry);
int  list_empty(const list_head_t *head);
int  list_count(const list_head_t *head);

#endif
```

### 5.3 util/Makefile

```makefile
include ../common.mk
SRCS := debug.c list.c tick.c
OBJS := $(SRCS:.c=.o)
TARGET := built-in.a

all: $(TARGET)
$(TARGET): $(OBJS)
	$(AR) rcs $@ $^
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f *.o *.a *.d
```

### 5.4 第五阶段验证

编译通过即可（无独立可验证的 UI 行为，被后续阶段使用）。

---

## 第六阶段：文件服务

### 目标

从原有代码 `file/file.c` 迁移文件 I/O 功能，提供 `mmap` 文件映射、目录遍历、文件过滤等能力。

### 6.1 file_service.h / file_service.c

**file_service.h**：
```c
#ifndef FILE_SERVICE_H
#define FILE_SERVICE_H

#include <stdint.h>
#include <stdio.h>

#define MAX_FILE_PATH    1024
#define MAX_FILE_NAME    256

/* mmap 文件映射结构 */
typedef struct file_map {
    char  path[MAX_FILE_PATH];
    int   fd;
    FILE *fp;
    long  file_size;
    unsigned char *mem;          // mmap 指针
} file_map_t;

/* 目录条目 */
typedef struct dir_entry {
    char  name[MAX_FILE_NAME];
    char  full_path[MAX_FILE_PATH];
    int   is_dir;                // 1 = 目录, 0 = 常规文件
    long  file_size;
} dir_entry_t;

/* 文件列表 */
typedef struct file_list {
    char **files;                // 字符串数组
    int   count;                 // 文件数量
    int   capacity;              // 分配容量
} file_list_t;

/* 文件服务（单例） */
typedef struct file_service {
    /* mmap 文件 */
    int (*map_file)(const char *path, file_map_t *map);
    void (*unmap_file)(file_map_t *map);

    /* 获取文件扩展名（返回指向 path 内部的位置，不需要释放） */
    const char *(*get_extension)(const char *path);

    /* 判断文件是否为指定类型 */
    int (*is_image_file)(const char *path);
    int (*is_music_file)(const char *path);
    int (*is_text_file)(const char *path);

    /* 目录操作 */
    int (*list_dir)(const char *path, dir_entry_t **entries, int *count);
    void (*free_dir_entries)(dir_entry_t *entries, int count);

    /* 递归遍历目录收集所有文件 */
    int (*walk_dir)(const char *path, file_list_t *list, int max_depth);

    /* 文件列表操作 */
    void (*file_list_init)(file_list_t *list);
    void (*file_list_free)(file_list_t *list);
} file_service_t;

/* 获取全局文件服务实例 */
file_service_t *file_service_get(void);

#endif
```

**file_service.c 关键实现逻辑**：

```
map_file(path, map):
  1. strncpy(map->path, path, MAX_FILE_PATH)
  2. map->fp = fopen(path, "rb")
  3. fseek(fp, 0, SEEK_END); map->file_size = ftell(fp)
  4. map->fd = fileno(fp)
  5. map->mem = mmap(NULL, file_size, PROT_READ, MAP_SHARED, fd, 0)
  6. return 0

unmap_file(map):
  1. munmap(map->mem, map->file_size)
  2. fclose(map->fp)
  3. memset(map, 0, sizeof(*map))

get_extension(path):
  1. 找到最后一个 '.'，返回其位置+1
  2. 若没有 '.' 返回空字符串 ""

is_image_file(path):
  1. 获取扩展名
  2. 比较 ".bmp" / ".jpg" / ".jpeg" / ".png"（大小写不敏感）

is_music_file(path):
  1. 比较 ".mp3" / ".wav" / ".flac"

is_text_file(path):
  1. 比较 ".txt" / ".log" / ".c" / ".h" / ".py" 等

list_dir(path, entries, count):
  1. 使用 scandir(path, &namelist, NULL, alphasort)
  2. 遍历 namelist：
     - 跳过 "." 和 ".."
     - stat 获取文件类型和大小
     - 填充 dir_entry_t
  3. *count = 实际条目数
  4. 返回 0

walk_dir(path, list, max_depth):
  1. 如果 max_depth <= 0，返回
  2. list_dir(path, &entries, &n)
  3. 遍历 entries：
     - 如果是常规文件 → 添加到 list->files
     - 如果是目录：
       - 追加到文件列表作为 "目录" 标记（可选）
       - 递归调用 walk_dir(entry.full_path, list, max_depth - 1)
  4. free_dir_entries(entries, n)
```

**已知文件类型映射表**（用于 `is_xxx_file` 和后续页面功能）：
| 扩展名 | 类型 | 用途 |
|---|---|---|
| `.bmp` `.jpg` `.jpeg` `.png` | 图片 | 图片查看器 / 幻灯片 |
| `.mp3` `.wav` | 音乐 | 音乐播放器 |
| `.txt` `.log` | 文本 | 文本阅读器 |

### 6.2 第六阶段验证

编译通过，可在 `main.c` 中调用 `file_service` 测试目录遍历：

```c
file_service_t *fs = file_service_get();
file_list_t list;
fs->file_list_init(&list);
fs->walk_dir("/path/to/test/dir", &list, 3);
printf("Found %d files\n", list.count);
fs->file_list_free(&list);
```

---

## 第七阶段：UI 全局样式

### 目标

定义全局 LVGL 样式（style），确保所有页面的视觉风格统一。避免每个页面重复定义相同的样式代码。

### 7.1 styles.h / styles.c

**styles.h**：
```c
#ifndef STYLES_H
#define STYLES_H

#include "lvgl.h"
#include "config.h"

/* 提供一套预定义的样式对象，各页面直接引用 */

typedef struct app_styles {
    lv_style_t bg;                // 页面背景
    lv_style_t title;             // 标题文字
    lv_style_t text_normal;       // 正文文字
    lv_style_t text_small;        // 小字（图标注释）
    lv_style_t btn_normal;        // 普通按钮
    lv_style_t btn_focused;       // 按钮获得焦点
    lv_style_t btn_pressed;       // 按钮按下
    lv_style_t btn_icon;          // 图标按钮（正方形，含图标+文字）
    lv_style_t bar_bg;            // 进度条背景
    lv_style_t bar_indicator;     // 进度条指示器
    lv_style_t menu_bar;          // 菜单栏/工具栏
    lv_style_t grid_cell;         // 图标网格单元格
    lv_style_t container;         // 通用容器
} app_styles_t;

/* 初始化所有样式，返回全局样式对象指针 */
app_styles_t *styles_init(void);

/* 获取全局样式单例 */
app_styles_t *styles_get(void);

#endif
```

**styles.c 核心逻辑**：

```
styles_init():
  1. 分配 app_styles_t 全局单例
  2. 初始化每个 lv_style_t：

  // 页面背景
  lv_style_init(&s->bg)
  lv_style_set_bg_color(&s->bg, lv_color_hex(COLOR_BACKGROUND))
  lv_style_set_bg_opa(&s->bg, LV_OPA_COVER)

  // 标题
  lv_style_init(&s->title)
  lv_style_set_text_color(&s->title, lv_color_hex(COLOR_TEXT_PRIMARY))
  lv_style_set_text_font(&s->title, &lv_font_montserrat_24)

  // 正文
  lv_style_init(&s->text_normal)
  lv_style_set_text_color(&s->text_normal, lv_color_hex(COLOR_TEXT_PRIMARY))
  lv_style_set_text_font(&s->text_normal, &lv_font_montserrat_16)

  // 小字
  lv_style_init(&s->text_small)
  lv_style_set_text_color(&s->text_small, lv_color_hex(COLOR_TEXT_SECONDARY))
  lv_style_set_text_font(&s->text_small, &lv_font_montserrat_12)

  // 普通按钮
  lv_style_init(&s->btn_normal)
  lv_style_set_bg_color(&s->btn_normal, lv_color_hex(COLOR_BUTTON_NORMAL))
  lv_style_set_bg_opa(&s->btn_normal, LV_OPA_COVER)
  lv_style_set_border_width(&s->btn_normal, 2)
  lv_style_set_border_color(&s->btn_normal, lv_color_hex(COLOR_FOREGROUND))
  lv_style_set_radius(&s->btn_normal, 8)
  lv_style_set_pad_all(&s->btn_normal, 10)

  // 按钮按下
  lv_style_init(&s->btn_pressed)
  lv_style_set_bg_color(&s->btn_pressed, lv_color_hex(COLOR_BUTTON_PRESSED))

  // 按钮获得焦点
  lv_style_init(&s->btn_focused)
  lv_style_set_border_color(&s->btn_focused, lv_color_hex(COLOR_BUTTON_FOCUS))
  lv_style_set_border_width(&s->btn_focused, 3)

  // 图标按钮（正方形，居中排列图标+文字）
  lv_style_init(&s->btn_icon)
  lv_style_set_bg_color(&s->btn_icon, lv_color_hex(COLOR_BUTTON_NORMAL))
  lv_style_set_radius(&s->btn_icon, 12)
  lv_style_set_width(&s->btn_icon, MENU_ICON_SIZE + 20)

  // 进度条
  lv_style_init(&s->bar_bg)
  lv_style_set_bg_color(&s->bar_bg, lv_color_hex(COLOR_PROGRESS_BG))
  lv_style_set_radius(&s->bar_bg, 5)

  lv_style_init(&s->bar_indicator)
  lv_style_set_bg_color(&s->bar_indicator, lv_color_hex(COLOR_PROGRESS_FG))
  lv_style_set_radius(&s->bar_indicator, 5)

  // 菜单栏（横跨顶部或底部）
  lv_style_init(&s->menu_bar)
  lv_style_set_bg_color(&s->menu_bar, lv_color_hex(0x0F3460))
  lv_style_set_bg_opa(&s->menu_bar, LV_OPA_COVER)
  lv_style_set_pad_all(&s->menu_bar, 5)
  lv_style_set_height(&s->menu_bar, TOP_BAR_HEIGHT)

  // 网格单元格
  lv_style_init(&s->grid_cell)
  lv_style_set_pad_all(&s->grid_cell, 5)
  lv_style_set_border_width(&s->grid_cell, 1)
  lv_style_set_border_color(&s->grid_cell, lv_color_hex(0x333333))

  3. 返回 s
```

### 7.2 第七阶段验证

在 `main.c` 中调用 `styles_init()`，编译通过即可（样式被后续阶段页面使用）。

---

## 第八阶段：自定义控件

### 目标

创建项目特有的可复用 LVGL 控件，封装常见 UI 模式，供各页面调用。

### 8.1 icon_button.h / icon_button.c — 图标按钮控件

**功能**：组合图标（emoji 或图片）+ 文本标签的按钮，用于菜单选择。

**接口**：
```c
#ifndef ICON_BUTTON_H
#define ICON_BUTTON_H

#include "lvgl.h"

/**
 * 创建图标按钮
 * @param parent  父容器
 * @param icon    图标文字（如 LV_SYMBOL_FILE、LV_SYMBOL_IMAGE）
 * @param label   按钮下方文字
 * @param width   按钮宽度
 * @param height  按钮高度
 * @return        按钮对象（lv_obj_t*）
 */
lv_obj_t *icon_button_create(lv_obj_t *parent,
                              const char *icon,
                              const char *label,
                              int32_t width, int32_t height);

/**
 * 创建仅文本按钮（用于简单操作按钮）
 */
lv_obj_t *text_button_create(lv_obj_t *parent, const char *text,
                              int32_t width, int32_t height);

#endif
```

**实现逻辑**：
```
icon_button_create(parent, icon, label, width, height):
  1. lv_obj_t *btn = lv_btn_create(parent)
  2. lv_obj_set_size(btn, width, height)
  3. 应用全局样式：lv_obj_add_style(btn, styles_get()->btn_normal, 0)
  4. 创建垂直 flex 布局：
     lv_obj_set_flex_flow(btn, LV_FLEX_FLOW_COLUMN)
     lv_obj_set_flex_align(btn, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                           LV_FLEX_ALIGN_CENTER)
  5. 创建图标 label：
     lv_obj_t *icon_label = lv_label_create(btn)
     lv_label_set_text(icon_label, icon)
     lv_obj_add_style(icon_label, styles_get()->title, 0)
  6. 创建文字 label：
     lv_obj_t *text_label = lv_label_create(btn)
     lv_label_set_text(text_label, label)
     lv_obj_add_style(text_label, styles_get()->text_small, 0)
  7. 返回 btn
```

### 8.2 file_browser.h / file_browser.c — 文件浏览器控件

**功能**：显示目录树和文件网格，支持导航（进入子目录、返回上级目录）、文件选择和分页。

**接口**：
```c
#ifndef FILE_BROWSER_H
#define FILE_BROWSER_H

#include "lvgl.h"
#include "service/file_service.h"

/* 文件浏览器事件类型 */
typedef enum {
    FB_EVENT_DIR_SELECTED,       // 目录被选中（进入子目录）
    FB_EVENT_FILE_SELECTED,      // 文件被选中
    FB_EVENT_BACK,               // 返回上级目录
    FB_EVENT_PAGE_CHANGED,       // 页码改变
} fb_event_type_t;

/* 文件浏览器配置 */
typedef struct {
    int32_t cols;                // 每行列数
    int32_t rows;                // 行数
    int32_t cell_width;          // 单元格宽度
    int32_t cell_height;         // 单元格高度
    int32_t gap;                 // 间距
    const char *filter_ext;      // 过滤扩展名（NULL = 不过滤）
} fb_config_t;

/* 文件浏览器 */
typedef struct file_browser {
    lv_obj_t *container;         // 外层容器
    lv_obj_t *grid;              // 内部网格容器
    lv_obj_t *path_label;        // 当前路径显示
    lv_obj_t *page_label;        // 页码显示 "1/5"
    lv_obj_t *btn_up;            // 返回上级按钮
    lv_obj_t *btn_prev;          // 上一页按钮
    lv_obj_t *btn_next;          // 下一页按钮
    fb_config_t config;          // 配置
    char current_path[MAX_FILE_PATH];  // 当前路径
    int current_page;            // 当前页码
    int total_pages;             // 总页数
} file_browser_t;

/**
 * 创建文件浏览器
 * @param parent   父容器
 * @param config   配置参数
 * @return         文件浏览器对象（需调用方管理生命周期）
 */
file_browser_t *file_browser_create(lv_obj_t *parent, fb_config_t *config);

/**
 * 导航到指定路径
 */
void file_browser_navigate(file_browser_t *fb, const char *path);

/**
 * 刷新当前页面显示
 */
void file_browser_refresh(file_browser_t *fb);

/**
 * 注册事件回调
 * @param fb       文件浏览器
 * @param event_type  事件类型
 * @param callback 回调函数
 * @param user_data  回调自定义数据
 */
void file_browser_add_event_cb(file_browser_t *fb, fb_event_type_t event_type,
                                lv_event_cb_t callback, void *user_data);

/**
 * 销毁文件浏览器，释放所有资源
 */
void file_browser_destroy(file_browser_t *fb);

#endif
```

**file_browser_navigate 实现逻辑**：
```
file_browser_navigate(fb, path):
  1. strncpy(fb->current_path, path, MAX_FILE_PATH)
  2. file_service_t *fs = file_service_get()
  3. dir_entry_t *entries; int count
  4. fs->list_dir(path, &entries, &count)
  5. 过滤 entries：仅保留目录 + 符合 filter_ext 的文件
  6. 分页计算：
     per_page = config->cols * config->rows
     fp->total_pages = ceil(filtered_count / per_page)
     fp->current_page = 0
  7. file_browser_refresh(fb)
```

**file_browser_refresh 实现逻辑**：
```
file_browser_refresh(fb):
  1. 清空 grid 内所有子对象
  2. 计算当前页的条目范围 [start, end)
  3. 遍历当前页条目，为每个条目创建 icon_button：
     - 目录 → LV_SYMBOL_DIRECTORY + entry.name
     - 图片 → LV_SYMBOL_IMAGE + entry.name
     - 音乐 → LV_SYMBOL_AUDIO + entry.name
     - 文本 → LV_SYMBOL_FILE + entry.name
  4. 更新 path_label 显示当前路径
  5. 更新 page_label 显示 "PAGE/总页数"
  6. 更新按钮状态（首尾页时禁用 prev/next 按钮）
```

### 8.3 第八阶段验证

在 `main.c` 中创建一个测试页面，放置 `file_browser` 并导航到某个目录，确认网格显示正常、导航按钮可用。

---

## 第九阶段：基础页面实现

### 目标

建立页面基类和页面管理器，实现主菜单页面（main_screen）。

### 9.1 base_screen.h — 页面虚基类

```c
#ifndef BASE_SCREEN_H
#define BASE_SCREEN_H

#include "lvgl.h"

typedef struct base_screen {
    const char *name;                    // 页面名称（用于注册和切换）

    /* 创建页面：构建 widget 树，返回 lv_screen 对象 */
    lv_obj_t *(*create)(struct base_screen *self);

    /* 进入页面时调用（从其他页面切换过来） */
    void (*on_enter)(struct base_screen *self, void *user_data);

    /* 离开页面时调用 */
    void (*on_exit)(struct base_screen *self);

    /* 销毁页面（释放资源，可留空让 LVGL 自动管理 */
    void (*destroy)(struct base_screen *self);

    /* 子类私有数据 */
    void *priv;
} base_screen_t;

#endif
```

### 9.2 page_manager.h / page_manager.c — 页面管理器

```c
#ifndef PAGE_MANAGER_H
#define PAGE_MANAGER_H

#include "ui/screens/base_screen.h"

/* 注册一个页面 */
int page_manager_register(base_screen_t *screen);

/* 切换到指定页面 */
int page_manager_navigate(const char *name, void *user_data);

/* 返回上一页（如果有） */
int page_manager_go_back(void);

/* 获取当前活动页面名称 */
const char *page_manager_current(void);

/* 初始化所有页面并跳转到主菜单 */
int page_manager_init(void);

/* 获取页面实例（用于跨页面数据传递） */
base_screen_t *page_manager_get(const char *name);

#endif
```

**page_manager.c 实现逻辑**：
```
内部数据：
  static base_screen_t *screens[MAX_SCREENS]   // 注册表
  static int screen_count
  static int history[MAX_HISTORY]               // 返回栈
  static int history_count

page_manager_register(screen):
  1. screens[screen_count++] = screen
  2. 调用 screen->create(screen) 预创建（或延迟到首次导航时创建）
  3. 返回 0

page_manager_navigate(name, user_data):
  1. 在 screens[] 中查找 name
  2. 如果找到：
     - 调用目标 screen->on_enter(target, user_data)
     - lv_screen_load(target->screen_obj)
     - 将当前页面推入 history 栈
     - 更新 current
  3. 返回 0（或 -1 未找到）

page_manager_go_back():
  1. 从 history 栈弹出上一页
  2. page_manager_navigate(prev_name, NULL)
```

### 9.3 main_screen.h / main_screen.c — 主菜单页面

**功能**：
- 显示 3 个图标按钮：**浏览模式**、**自动播放**、**设置**
- 点击按钮跳转到对应页面
- 显示应用标题

**布局**（从上到下）：
```
┌──────────────────────────────────────────┐
│          ┌───────────────────┐           │  标题区域
│          │  digitpic 数码相框 │           │  (1/5 屏幕高度)
│          └───────────────────┘           │
│                                          │
│   ┌─────────┐ ┌─────────┐ ┌─────────┐   │
│   │   📁    │ │   ▶️    │ │   ⚙️    │   │  按钮区域
│   │  浏览    │ │ 自动播放 │ │  设置    │   │  (居中对齐)
│   └─────────┘ └─────────┘ └─────────┘   │
│                                          │
└──────────────────────────────────────────┘
```

**实现逻辑**：

```
main_screen_create(self):
  1. lv_obj_t *scr = lv_obj_create(NULL)          // 新建 screen
  2. 应用背景样式 styles_get()->bg
  3. 创建标题 label：
     lv_obj_t *title = lv_label_create(scr)
     lv_label_set_text(title, "digitpic 数码相框")
     lv_obj_add_style(title, styles_get()->title, 0)
     lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 40)
  4. 创建按钮容器（flex 水平居中）：
     lv_obj_t *btn_row = lv_obj_create(scr)
     设置 flex 水平布局 + 等间距
  5. 创建 3 个图标按钮：
     icon_button_create(btn_row, LV_SYMBOL_FILE,  "浏览", 160, 180)
       → 绑定 LV_EVENT_CLICKED → 跳转 "browse"
     icon_button_create(btn_row, LV_SYMBOL_PLAY,  "自动播放", 160, 180)
       → 绑定 LV_EVENT_CLICKED → 跳转 "auto"
     icon_button_create(btn_row, LV_SYMBOL_SETTINGS, "设置", 160, 180)
       → 绑定 LV_EVENT_CLICKED → 跳转 "setting"
  6. 保存 scr 到 self 的内部数据
  7. 返回 scr
```

**注意**：LVGL 的 LV_SYMBOL_xxx 是内置图标字体。如果需要自定义图标，使用 `assets/icons/` 中的 PNG 图片创建 `lv_img` 对象。

### 9.4 app/app.c — 应用初始化序列

```c
#include "app.h"
#include "config.h"
#include "util/debug.h"
#include "util/tick.h"
#include "hal/display/fb_driver.h"
#include "hal/display/lvgl_display.h"
#include "hal/input/lvgl_input.h"
#include "service/file_service.h"
#include "ui/styles.h"
#include "ui/screens/base_screen.h"
#include "page_manager.h"

int app_init(int argc, char *argv[])
{
    /* 1. 启动 LVGL */
    lv_init();

    /* 2. 定时器 tick */
    tick_init();

    /* 3. 调试系统 */
    debug_init();

    /* 4. 显示驱动 */
    display_driver_t *fb = fb_driver_create();
    if (fb->init(fb) != 0) {
        DBG_PRINTF(DEBUG_LEVEL_ERR, "Failed to init framebuffer");
        return -1;
    }
    lvgl_display_init(fb);

    /* 5. 输入驱动 */
    input_driver_t *touch = touchscreen_create();
    if (touch->init(touch) == 0) {
        lvgl_touchscreen_init(touch);
    }

    input_driver_t *mouse = mouse_create();
    if (mouse->init(mouse) == 0) {
        lvgl_mouse_init(mouse);
    }

    /* 6. 全局样式 */
    styles_init();

    /* 7. 服务层 */
    file_service_get();          // 触发单例创建

    /* 8. 页面管理器（注册所有页面、加载主菜单） */
    page_manager_init();

    return 0;
}

void app_run(void)
{
    /* LVGL 主循环 */
    while (1) {
        lv_timer_handler();
        usleep(5000);   // 5ms
    }
}

void app_cleanup(void)
{
    // 按相反顺序释放资源（实际项目中此函数很少被调用）
    tick_deinit();
}
```

### 9.5 page_manager_init 完整注册序列

```
page_manager_init():
  1. 注册所有页面（按名称）：
     page_manager_register(&g_main_screen)
     page_manager_register(&g_browse_screen)     // 先注册占位
     page_manager_register(&g_manual_screen)     // 先注册占位
     page_manager_register(&g_auto_screen)
     page_manager_register(&g_setting_screen)
     page_manager_register(&g_interval_screen)
     page_manager_register(&g_text_screen)
     page_manager_register(&g_music_screen)

  2. 跳转到主菜单：
     page_manager_navigate("main", NULL)
```

### 9.6 第九阶段验证

编译运行后：
- 屏幕显示主菜单，标题为 "digitpic 数码相框"
- 三个图标按钮可见
- 点击按钮时页面切换（目标页面暂为空白占位）

---

## 第十阶段：图片查看器页面

### 目标

实现图片查看页面（manual_screen），支持：
- 加载并显示单张图片
- 缩放（放大/缩小）
- 上一张/下一张切换
- 返回主菜单
- 后台预加载下一张图片

### 10.1 manual_screen.h / manual_screen.c

**布局**：
```
┌──────────────────────────────────────────┐
│                                          │
│                                          │
│          ┌───────────────────┐           │
│          │                   │           │
│          │    图片显示区域    │           │  图片 (可缩放)
│          │                   │           │
│          └───────────────────┘           │
│                                          │
│                                          │
├──────────────────────────────────────────┤
│ [返回] [缩小] [放大] [上一张] [下一张] [幻灯]│  底部工具栏
└──────────────────────────────────────────┘
```

**数据结构**：
```c
typedef struct {
    lv_obj_t *image_obj;          // LVGL 图片对象
    lv_obj_t *toolbar;            // 底部工具栏
    lv_obj_t *status_label;       // 状态显示（当前图片名/编号）

    file_list_t file_list;        // 当前目录下的所有图片文件
    int current_index;            // 当前图片在列表中的索引
    float zoom_factor;            // 当前缩放因子 (0.5 ~ 3.0)
    float zoom_step;              // 缩放步进

    char current_dir[MAX_FILE_PATH];  // 当前浏览目录

    /* 后台预加载 */
    int preload_index;            // 正在预加载的图片索引
    void *preload_data;           // 预加载的图片数据
} manual_screen_priv_t;
```

**核心功能函数**：

```
manual_screen_create(self):
  1. lv_obj_t *scr = lv_obj_create(NULL)
  2. 创建 image_obj 居中：
     lv_obj_t *img = lv_image_create(scr)
     lv_obj_center(img)
  3. 创建底部工具栏：
     lv_obj_t *toolbar = lv_obj_create(scr)
     lv_obj_set_size(toolbar, LV_PCT(100), BOTTOM_BAR_HEIGHT)
     lv_obj_align(toolbar, LV_ALIGN_BOTTOM_MID, 0, 0)
     设置 flex 水平等间距布局
  4. 在工具栏中创建按钮：
     text_button_create(toolbar, LV_SYMBOL_LEFT "返回", 80, 40)
       → 点击回调：page_manager_navigate("main")
     text_button_create(toolbar, LV_SYMBOL_MINUS "缩小", 80, 40)
       → 点击回调：manual_zoom_out(self)
     text_button_create(toolbar, LV_SYMBOL_PLUS "放大", 80, 40)
       → 点击回调：manual_zoom_in(self)
     text_button_create(toolbar, LV_SYMBOL_PREV "上一张", 80, 40)
       → 点击回调：manual_prev_image(self)
     text_button_create(toolbar, LV_SYMBOL_NEXT "下一张", 80, 40)
       → 点击回调：manual_next_image(self)
     text_button_create(toolbar, "▶ 幻灯", 80, 40)
       → 点击回调：启动幻灯片，page_manager_navigate("auto")
  5. 保存到 self->priv
  6. 返回 scr

manual_load_image(self, path):
  1. lv_image_set_src(self->priv->image_obj, path)
  2. 重置缩放：zoom_factor = 1.0
  3. lv_image_set_zoom(self->priv->image_obj, (int)(zoom_factor * 256))
  4. lv_obj_center(self->priv->image_obj)
  5. 更新 status_label 显示文件名
  6. 触发后台预加载下一张图片

manual_zoom_in(self):
  1. zoom_factor += zoom_step
  2. 限制最大 3.0
  3. lv_image_set_zoom(image_obj, (int)(zoom_factor * 256))
  4. lv_obj_center(image_obj)

manual_zoom_out(self):
  1. zoom_factor -= zoom_step
  2. 限制最小 0.5
  3. lv_image_set_zoom(image_obj, (int)(zoom_factor * 256))
  4. lv_obj_center(image_obj)

manual_next_image(self):
  1. current_index = (current_index + 1) % file_list.count
  2. manual_load_image(self, file_list.files[current_index])

manual_prev_image(self):
  1. current_index = (current_index - 1 + file_list.count) % file_list.count
  2. manual_load_image(self, file_list.files[current_index])
```

**预加载机制**（简化版）：
```
start_preload_thread(self):
  1. 如果 preload_index 已有效，取消当前预加载
  2. 启动新线程，preload_index = current_index + 1
  3. 在线程中调用 lv_image_set_src 加载图片到隐藏的 lv_image 对象
  4. 加载完成后设置标志位，主线程检测后直接显示
```

### 10.2 manual_screen 的 on_enter 回调

```
manual_on_enter(self, user_data):
  1. priv = self->priv
  2. 如果 user_data 包含目录路径：
     使用 file_service_get()->walk_dir() 收集该目录下所有图片文件
     存入 priv->file_list
     设置 priv->current_index = 0
  3. 如果 user_data 包含单个文件路径：
     解析其所在目录，遍历目录收集所有图片
     找到该文件在列表中的位置，设为 current_index
  4. manual_load_image(self, file_list.files[current_index])
```

### 10.3 第十阶段验证

- 进入图片浏览，显示图片
- 点击放大/缩小按钮，图片缩放正常
- 上一张/下一张切换正常
- 返回按钮返回主菜单

---

## 第十一阶段：文件浏览器页面

### 目标

实现文件浏览器页面（browse_screen），支持：
- 网格或列表显示目录和文件
- 点击目录进入子目录
- 点击文件根据类型打开（图片→manual_screen、音乐→music_screen、文本→text_screen）
- 上下翻页
- 返回上级目录
- 顶部路径栏显示当前路径

### 11.1 browse_screen.h / browse_screen.c

**布局**：
```
┌──────────────────────────────────────────┐
│ [/mnt/sdcard/photos/]              [🔍]  │  路径栏 + 搜索（可选）
├──────────────────────────────────────────┤
│ ┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐    │
│ │📁 dir│ │📁 dir│ │🖼️img │ │🖼️img │    │  文件网格
│ │name  │ │name  │ │.jpg  │ │.bmp  │    │  (cols × rows)
│ └──────┘ └──────┘ └──────┘ └──────┘    │
│ ┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐    │
│ │🎵mp3 │ │📄txt │ │ ...  │ │ ...  │    │
│ │.mp3  │ │.txt  │ │      │ │      │    │
│ └──────┘ └──────┘ └──────┘ └──────┘    │
├──────────────────────────────────────────┤
│  [上级] [选择]   ◀ 1/5 ▶   [返回]       │  底部菜单栏
└──────────────────────────────────────────┘
```

**数据结构**：
```c
typedef struct {
    file_browser_t *fb;           // 复用文件浏览器控件
    lv_obj_t *toolbar;            // 底部工具栏

    /* 当前选中的文件（用于 "选择" 按钮） */
    char selected_file[MAX_FILE_PATH];
} browse_screen_priv_t;
```

**核心逻辑**：
```
browse_screen_create(self):
  1. lv_obj_t *scr = lv_obj_create(NULL)
  2. 应用背景样式
  3. 创建 file_browser_t，4 列 × 3 行，整屏显示
  4. 创建底部工具栏，含按钮：
     - "上级" → file_browser_navigate(fb, parent_path)
     - "选择" → 根据选中文件类型跳转到对应页面
     - "上一页"/"下一页" → fb 内部处理
     - "返回" → page_manager_navigate("main")
  5. 为 file_browser 注册事件回调：
     FB_EVENT_DIR_SELECTED → 导航到选中目录
     FB_EVENT_FILE_SELECTED → 记录 selected_file，高亮显示
  6. 保存 self->priv
  7. 返回 scr

browse_on_enter(self, user_data):
  1. 如果 user_data 包含路径 → 导航到该路径
  2. 否则使用默认起始路径 "/"

// 文件选中后的"选择"操作
browse_on_select(self):
  1. 获取 selected_file
  2. 根据文件类型跳转：
     - 图片 → page_manager_navigate("manual", selected_file)
     - 音乐 → page_manager_navigate("music", selected_file)
     - 文本 → page_manager_navigate("text", selected_file)
```

### 11.2 第十一阶段验证

- 文件浏览器正常显示目录结构
- 点击目录可进入子目录
- 点击文件高亮选中
- 按"选择"按钮跳转到对应页面
- 翻页和返回上级目录正常工作

---

## 第十二阶段：幻灯片与设置页面

### 目标

实现自动播放页面（auto_screen）、设置页面（setting_screen）、间隔设置页面（interval_screen）。

### 12.1 auto_screen — 幻灯片播放

**功能**：
- 从配置的目录递归收集所有图片文件
- 按顺序每隔 N 秒自动切换一张图片
- 支持手动上一张/下一张
- 显示播放进度和当前图片名
- 支持暂停/继续

**布局**：
```
┌──────────────────────────────────────────┐
│                                          │
│                                          │
│          ┌───────────────────┐           │
│          │    全屏图片      │           │
│          └───────────────────┘           │
│                                          │
│  photo_001.jpg                    [3/50] │  底部状态栏
├──────────────────────────────────────────┤
│  [返回] [⏮上一张] [⏯暂停] [下一张⏭] [⚙设置] │
└──────────────────────────────────────────┘
```

**核心实现**：
```c
typedef struct {
    lv_obj_t *image_obj;
    lv_obj_t *status_label;       // "photo_001.jpg  3/50"
    lv_obj_t *toolbar;
    lv_timer_t *timer;           // LVGL 定时器（自动切换）
    file_list_t file_list;
    int current_index;
    int interval_seconds;        // 切换间隔（秒）
    bool paused;
} auto_screen_priv_t;
```

```
auto_screen_create(self):
  1. 创建全屏 image 对象
  2. 创建顶部浮动状态栏（半透明背景）
  3. 创建底部浮动工具栏
  4. 定义按钮回调：
     - "暂停/继续" → 切换 paused 状态，停止/重启 timer
     - "上一张/下一张" → 切换图片
     - "返回" → page_manager_go_back()
     - "设置" → page_manager_navigate("setting")

auto_slide_timer_cb(timer):
  1. 获取 auto_screen_priv_t
  2. if (paused) return
  3. current_index = (current_index + 1) % file_list.count
  4. 加载图片：lv_image_set_src(img, file_list.files[current_index])
  5. 更新状态栏文字

auto_on_enter(self, user_data):
  1. 如果没有文件列表，从默认目录或 user_data 指定的目录收集图片
  2. 显示第一张图片
  3. 启动定时器：lv_timer_create(auto_slide_timer_cb, interval_seconds * 1000, self)
```

### 12.2 setting_screen — 设置页面

**功能**：
- 选择幻灯片文件夹
- 设置幻灯片间隔时间
- 返回主菜单

**布局**：
```
┌──────────────────────────────────────────┐
│               ⚙ 设置                     │
│                                          │
│   ┌──────────────────────────────────┐   │
│   │  📁 选择文件夹   [/mnt/sdcard/]  │   │
│   └──────────────────────────────────┘   │
│                                          │
│   ┌──────────────────────────────────┐   │
│   │  ⏱ 切换间隔          [5秒]       │   │
│   └──────────────────────────────────┘   │
│                                          │
│   ┌──────────────────────────────────┐   │
│   │  ↩ 返回                          │   │
│   └──────────────────────────────────┘   │
│                                          │
└──────────────────────────────────────────┘
```

**核心逻辑**：
```
setting_screen_create(self):
  1. 创建标题 "⚙ 设置"
  2. 创建设置项按钮（lv_list 或独立 btn）：
     - "选择文件夹" → 打开简化版文件浏览器（仅目录选择模式），
        选定后将路径保存到全局配置，供 auto_screen 使用
     - "切换间隔" → page_manager_navigate("interval")
     - "返回" → page_manager_go_back()
```

### 12.3 interval_screen — 间隔时间设置

**功能**：
- 显示当前间隔秒数（大号数字）
- 通过 +/- 按钮或滚轮调整间隔（1~60 秒）
- 保存设置并返回

**布局**：
```
┌──────────────────────────────────────────┐
│              设置切换间隔                 │
│                                          │
│              ┌──────────┐                │
│     [-]      │    5     │     [+]        │  数字加减
│              │   秒     │                │
│              └──────────┘                │
│                                          │
│              [确定]  [取消]              │
└──────────────────────────────────────────┘
```

```
interval_on_adjust(self, delta):
  1. priv->interval += delta
  2. 钳制到 [1, 60]
  3. 更新数字 label 显示

interval_on_confirm(self):
  1. 将 priv->interval 保存到全局配置
  2. page_manager_go_back()
```

**全局配置存储**：使用简单的全局变量或 `config.h` 中的默认值：
```c
// 可放在 config/config.c 或 app/app.c 中
static int g_slide_interval = 5;
static char g_slide_folder[MAX_FILE_PATH] = "/mnt/sdcard";

void config_set_interval(int sec) { g_slide_interval = sec; }
int  config_get_interval(void)     { return g_slide_interval; }
void config_set_folder(const char *path) { strncpy(g_slide_folder, path, MAX_FILE_PATH); }
const char *config_get_folder(void) { return g_slide_folder; }
```

### 12.4 第十二阶段验证

- 设置页面三项按钮可见可点击
- 间隔设置页面可调整数字（1~60），确定后全局生效
- 幻灯片播放可按设定间隔自动切换
- 暂停/继续功能正常

---

## 第十三阶段：音乐播放子系统

### 目标

实现完整的音乐播放功能，包括 HAL 层的 ALSA 输出和 MP3 解码，服务层的播放状态机，以及 UI 层的音乐播放器页面。

### 13.1 hal/audio/audio_output.h — 音频输出虚基类

```c
#ifndef AUDIO_OUTPUT_H
#define AUDIO_OUTPUT_H

#include <stdint.h>

typedef struct audio_output {
    const char *name;

    /* 初始化音频输出设备
     * @param rate     采样率 (Hz)，如 44100
     * @param channels 声道数，1=单声道 2=立体声
     * @param bits     位深度，16
     * @return 0 成功，-1 失败
     */
    int (*init)(struct audio_output *self,
                int rate, int channels, int bits);

    /* 写入 PCM 数据
     * @param buf    16-bit 小端 PCM 数据
     * @param frames 帧数（每帧 = channels * bits/8 字节）
     * @return 实际写入帧数，<0 错误
     */
    int (*write)(struct audio_output *self,
                 const int16_t *buf, int frames);

    /* 等待所有数据播放完毕 */
    void (*drain)(struct audio_output *self);

    /* 关闭设备 */
    void (*deinit)(struct audio_output *self);

    void *priv;
} audio_output_t;

#endif
```

### 13.2 alsa_output.h / alsa_output.c — ALSA 实现

**功能**：
- 打开 ALSA PCM 设备（默认 "default" 或 "plughw:0,0"）
- 配置硬件参数（采样率、格式、声道数、访问模式）
- 提供阻塞式和非阻塞式写入

**私有数据**：
```c
typedef struct {
    snd_pcm_t *pcm;              // PCM 句柄
    snd_pcm_hw_params_t *hw_params;
    snd_ctl_t *ctl;              // 控制接口（音量调节）
    int rate;
    int channels;
    int bits;
    unsigned int buffer_size;    // 缓冲区大小（帧）
    unsigned int period_size;    // 周期大小（帧）
} alsa_priv_t;
```

**alsa_init 实现逻辑**：
```
alsa_init(self, rate, channels, bits):
  1. snd_pcm_open(&priv->pcm, "default", SND_PCM_STREAM_PLAYBACK, 0)
  2. snd_pcm_hw_params_malloc(&params)
  3. snd_pcm_hw_params_any(pcm, params)
  4. snd_pcm_hw_params_set_access(pcm, params,
       SND_PCM_ACCESS_RW_INTERLEAVED)
  5. snd_pcm_hw_params_set_format(pcm, params,
       bits == 16 ? SND_PCM_FORMAT_S16_LE : SND_PCM_FORMAT_S32_LE)
  6. snd_pcm_hw_params_set_channels(pcm, params, channels)
  7. snd_pcm_hw_params_set_rate_near(pcm, params, &rate, 0)
  8. snd_pcm_hw_params_set_buffer_size_near(pcm, params, &buffer_size)
  9. snd_pcm_hw_params(pcm, params)
  10. snd_pcm_hw_params_free(params)
  11. snd_pcm_prepare(pcm)
  12. 初始化 ctl 用于音量控制（可选）
  13. 返回 0
```

**alsa_write 实现逻辑**：
```
alsa_write(self, buf, frames):
  1. ret = snd_pcm_writei(pcm, buf, frames)
  2. if (ret == -EPIPE):  // 欠载（under-run）
       snd_pcm_prepare(pcm)
       重试写入
  3. 返回实际写入帧数
```

### 13.3 mp3_decoder.h / mp3_decoder.c — MP3 解码器

从原有 `music/mp3.c` 迁移 libmad 解码逻辑，抽取为独立模块。

```c
#ifndef MP3_DECODER_H
#define MP3_DECODER_H

#include <stdint.h>

typedef struct mp3_decoder {
    /* 初始化解码器 */
    int (*init)(struct mp3_decoder *self);

    /* 解码一段 MP3 数据为 PCM
     * @param data      输入 MP3 数据
     * @param len       输入数据长度
     * @param pcm_out   输出 PCM 数据（16-bit 立体声小端，调用方负责释放）
     * @param pcm_frames 输出 PCM 帧数
     * @return 消耗的输入字节数，<0 错误
     */
    int (*decode)(struct mp3_decoder *self,
                  const uint8_t *data, int len,
                  int16_t **pcm_out, int *pcm_frames);

    /* 重置解码器状态（用于 seek 后） */
    void (*reset)(struct mp3_decoder *self);

    /* 销毁解码器 */
    void (*deinit)(struct mp3_decoder *self);

    void *priv;
} mp3_decoder_t;

mp3_decoder_t *mp3_decoder_create(void);
void mp3_decoder_destroy(mp3_decoder_t *dec);
#endif
```

**decode 实现**（基于 libmad 回调模型）：
```
// 内部结构
typedef struct {
    struct mad_stream   stream;
    struct mad_frame    frame;
    struct mad_synth    synth;
    const uint8_t      *input_buf;
    int                 input_len;
    int                 consumed;
    int16_t            *pcm_buf;
    int                 pcm_capacity;
    int                 pcm_frames;
} mp3_priv_t;

// libmad 输入回调
mad_input_cb(data, stream):
  1. 从 priv->input_buf + priv->consumed 拷贝数据到 stream->buffer
  2. 更新 stream 的 bufend

// libmad 输出回调
mad_output_cb(data, header, pcm):
  1. 将 mad_fixed_to_sample(pcm->samples[i]) 转换为 int16_t
  2. 写入 priv->pcm_buf
  3. 更新 pcm_frames

mp3_decode(self, data, len, pcm_out, pcm_frames):
  1. priv->input_buf = data, priv->input_len = len
  2. mad_stream_buffer(&stream, data, len)
  3. while (mad_frame_decode(&frame, &stream) == 0):
       mad_synth_frame(&synth, &frame)
       将 synth.pcm 数据追加到输出缓冲区
  4. *pcm_out = pcm_buf
  5. *pcm_frames = total_frames
  6. 返回 stream.this_frame - data
```

**错误处理**：对 `MAD_ERROR_LOSTSYNC` 做自动重同步。

### 13.4 service/music_service.h / music_service.c — 音乐播放服务

**功能**：管理播放状态机，协调解码器和音频输出。

```c
#ifndef MUSIC_SERVICE_H
#define MUSIC_SERVICE_H

typedef enum {
    MUSIC_STATE_IDLE,           // 空闲
    MUSIC_STATE_PLAYING,        // 播放中
    MUSIC_STATE_PAUSED,         // 暂停
    MUSIC_STATE_STOPPED,        // 停止
} music_state_t;

typedef void (*music_callback_t)(music_state_t state, int progress,
                                  int total, void *user_data);

typedef struct music_service {
    int (*play)(const char *file_path);
    int (*pause)(void);
    int (*resume)(void);
    int (*stop)(void);

    int (*set_volume)(int volume);   // 0 ~ 100
    int (*get_volume)(void);

    music_state_t (*get_state)(void);
    int (*get_progress)(int *current_sec, int *total_sec);

    void (*register_callback)(music_callback_t cb, void *user_data);

    /* 内部读写 */
    void *priv;
} music_service_t;

music_service_t *music_service_get(void);

#endif
```

**music_service.c 核心实现**：

```
内部结构：
  static music_service_t *g_instance
  - audio_output_t *output           // ALSA 输出
  - mp3_decoder_t  *decoder          // MP3 解码器
  - file_map_t      current_file
  - music_state_t   state
  - int             volume
  - int64_t         current_sample   // 当前采样位置
  - int64_t         total_samples    // 总采样数
  - pthread_t       play_thread

music_play(path):
  1. file_service_get()->map_file(path, &file_map)
  2. 解析 ID3v2 标签，跳过标签数据
  3. 设置状态为 PLAYING
  4. 创建播放线程 music_play_thread()

music_play_thread(arg):
  1. 循环读取 MP3 数据块（每次 16KB）
  2. mp3_decoder->decode(chunk, &pcm, &frames)
  3. audio_output->write(pcm, frames)
  4. 更新 current_sample
  5. 如果 state == PAUSED → pthread_cond_wait（暂停等待）
  6. 如果 state == STOPPED → 退出线程

music_set_volume(vol):
  1. priv->volume = vol (0~100)
  2. 通过 ALSA ctl 接口设置音量：
     snd_ctl_elem_id_set_numid(id, VOLUME_NUMID)
     snd_ctl_elem_value_set_integer(ctl_val, 0, vol_to_alsa(vol))
     snd_ctl_elem_write(ctl, ctl_val)
```

### 13.5 music_screen — 音乐播放器页面

**布局**：
```
┌──────────────────────────────────────────┐
│          🎵 音乐播放器                    │
│                                          │
│          ┌──────────────────┐            │
│          │   song_name.mp3  │            │  歌曲名
│          └──────────────────┘            │
│                                          │
│   ───●───────────────────────────────    │  进度条 (lv_bar)
│        01:23 / 04:56                     │  时间显示
│                                          │
│      🔊 ───●────────────────            │  音量条 (lv_bar)
│                                          │
│   [⏮]  [⏯]  [⏹]  [⏭]                  │  播放控制
│                                          │
│   [🔊-]         [🔊+]         [↩返回]    │  上一行
└──────────────────────────────────────────┘
```

**核心实现**：
```c
typedef struct {
    lv_obj_t *title_label;        // 歌曲名
    lv_obj_t *progress_bar;       // 播放进度条
    lv_obj_t *time_label;         // "01:23 / 04:56"
    lv_obj_t *volume_bar;         // 音量条
    lv_obj_t *play_btn;           // 播放/暂停按钮
    lv_obj_t *stop_btn;
    lv_obj_t *prev_btn;
    lv_obj_t *next_btn;
    lv_timer_t *progress_timer;   // 50ms 更新进度条

    char current_file[MAX_FILE_PATH];
    file_list_t playlist;         // 当前目录的音乐文件列表
    int current_track_index;
} music_screen_priv_t;

// 进度更新定时器
music_progress_timer_cb(timer):
  1. music_service_get()->get_progress(&cur, &total)
  2. lv_bar_set_value(progress_bar, cur, LV_ANIM_OFF)
  3. lv_bar_set_range(progress_bar, 0, total)
  4. 更新 time_label 显示 "MM:SS / MM:SS"

// 播放/暂停按钮回调
music_play_pause_cb(e):
  1. state = music_service_get()->get_state()
  2. if (state == PLAYING):
       music_service_get()->pause()
       lv_label_set_text(play_btn, LV_SYMBOL_PLAY)
     else:
       music_service_get()->resume()
       lv_label_set_text(play_btn, LV_SYMBOL_PAUSE)

// 音量 +/- 回调
music_vol_up_cb(e):
  1. vol = music_service_get()->get_volume() + VOLUME_STEP
  2. 限制最大 100
  3. music_service_get()->set_volume(vol)
  4. lv_bar_set_value(volume_bar, vol, LV_ANIM_ON)
```

### 13.6 第十三阶段验证

- MP3 文件播放正常，有声音输出
- 进度条随播放实时更新
- 暂停/继续/停止功能正常
- 音量调节功能正常
- 上一首/下一首切换正常

---

## 第十四阶段：文本阅读器页面

### 目标

实现文本阅读器页面（text_screen），使用 LVGL 的 `lv_label` 或 `lv_textarea` 显示文本文件内容，支持翻页。

### 14.1 text_screen.h / text_screen.c

**功能**：
- 加载文本文件（UTF-8 编码）
- 自动分行显示
- 上一页/下一页
- 返回文件浏览器

**布局**：
```
┌──────────────────────────────────────────┐
│  📄 novel.txt                   第3/50页 │  标题栏
├──────────────────────────────────────────┤
│                                          │
│  正文内容正文内容正文内容正文内容正文内容   │
│  正文内容正文内容正文内容正文内容正文内容   │
│  正文内容正文内容正文内容正文内容正文内容   │
│  正文内容正文内容正文内容正文内容正文内容   │
│  ...                                     │  文本区域
│                                          │
├──────────────────────────────────────────┤
│  [返回]  [上一页]   ◀ 3/50 ▶  [下一页]   │  底部控制栏
└──────────────────────────────────────────┘
```

**实现方案**：使用 LVGL 的 `lv_label` 控件。LVGL 会自动处理文本换行。如果文本太长，使用 `lv_label_set_long_mode(LV_LABEL_LONG_WRAP)`。

**核心实现**：
```c
typedef struct {
    lv_obj_t *title_label;        // 文件名
    lv_obj_t *text_label;         // 文本内容（lv_label）
    lv_obj_t *page_label;         // 页码显示
    lv_obj_t *toolbar;

    file_map_t file_map;          // mmap 的文本文件
    char *text_buf;               // 当前页文本缓冲区
    int total_chars;              // 文件总字符数
    int current_pos;              // 当前显示起始位置
    int chars_per_page;           // 每页大约可显示字符数
    int current_page;
    int total_pages;
} text_screen_priv_t;
```

```
text_load_file(self, path):
  1. file_service_get()->map_file(path, &priv->file_map)
  2. priv->total_chars = file_map.file_size
  3. priv->current_pos = 0
  4. 估算每页字符数：根据 label 大小和字体高度计算
  5. priv->total_pages = total_chars / chars_per_page + 1
  6. text_show_current_page(self)

text_show_current_page(self):
  1. 计算当前页起始偏移：start = current_pos
  2. 计算当前页结束偏移：end = min(start + chars_per_page, total_chars)
  3. 从 file_map.mem + start 拷贝到 text_buf（确保末尾 '\0'）
  4. 处理换行：将 '\n' 替换为 LVGL 换行符
  5. lv_label_set_text(priv->text_label, text_buf)
  6. 更新 page_label 和 title_label

text_next_page(self):
  1. current_pos += chars_per_page
  2. if (current_pos >= total_chars) current_pos = total_chars - chars_per_page
  3. text_show_current_page(self)

text_prev_page(self):
  1. current_pos -= chars_per_page
  2. if (current_pos < 0) current_pos = 0
  3. text_show_current_page(self)
```

**注意**：
- 由于原始文本可能很大（如 19MB 的 `utf8_novel.txt`），不能一次性加载到 label 中。采用分页加载策略：每页只截取可显示部分的文字。
- UTF-8 字符边界处理：截取时确保不在多字节字符中间切断（检查前导字节）。

### 14.2 第十四阶段验证

- 文本文件正常打开和显示
- 翻页功能正常（上一页/下一页）
- 页码显示准确
- 中文 UTF-8 显示正常（依赖 FreeType 字体）
- 大文件（> 10MB）内存占用可控

---

## 第十五阶段：应用层集成与联调

### 目标

完成所有模块的集成，实现完整的启动序列，确保各页面之间数据传递和导航逻辑正确，进行端到端测试。

### 15.1 main.c 最终版

```c
#include "app/app.h"
#include "util/debug.h"

int main(int argc, char *argv[])
{
    if (app_init(argc, argv) != 0) {
        DBG_PRINTF(DEBUG_LEVEL_ERR, "Application init failed!");
        return -1;
    }

    DBG_PRINTF(DEBUG_LEVEL_INFO, "digitpic-lvgl started successfully");
    app_run();

    /* 正常情况下不会到达这里 */
    return 0;
}
```

### 15.2 跨页面数据传递约定

| 源页面 | 目标页面 | 传递数据 | 格式 |
|---|---|---|---|
| browse | manual | 图片文件路径 | `char *path` |
| browse | music | 音乐文件路径 | `char *path` |
| browse | text | 文本文件路径 | `char *path` |
| manual | auto | 图片目录路径 + 起始索引 | 全局配置 `g_slide_folder` |
| main | browse | 初始路径（可选） | `char *path`（NULL = 根目录） |
| main | auto | 无（使用全局配置的目录） | NULL |

数据传递方式：通过 `page_manager_navigate(name, user_data)` 的 `void *user_data` 参数。接收页在 `on_enter` 回调中处理。

```c
// 调用方
char *file_path = "/mnt/sdcard/photos/IMG_001.jpg";
page_manager_navigate("manual", file_path);

// 接收方
void manual_on_enter(base_screen_t *self, void *user_data) {
    if (user_data) {
        char *path = (char *)user_data;
        priv->current_file = strdup(path);
        // ... 加载图片
    }
}
```

### 15.3 完整导航流程图

```
                        ┌──────────┐
                        │  main    │ 主菜单
                        └────┬─────┘
               ┌─────────────┼─────────────┐
               v             v             v
         ┌─────────┐  ┌─────────┐  ┌──────────┐
         │ browse  │  │  auto   │  │ setting  │
         │(浏览器) │  │(幻灯片)  │  │(设置)    │
         └────┬────┘  └─────────┘  └────┬──────┘
              │                         │
    ┌─────────┼─────────┐               v
    v         v         v         ┌────────────┐
┌───────┐ ┌───────┐ ┌───────┐   │ interval   │
│manual │ │music  │ │ text  │   │(间隔设置)  │
│(图片) │ │(音乐) │ │(文本) │   └────────────┘
└───┬───┘ └───────┘ └───────┘
    │
    │ (幻灯片按钮)
    v
┌─────────┐
│  auto   │
└─────────┘
```

### 15.4 错误处理约定

| 场景 | 处理方式 |
|---|---|
| 设备文件 (`/dev/fb0`, `/dev/input/eventX`) 无法打开 | `dbg_print` 输出错误，`return -1`，跳过该设备 |
| 字体文件不存在 | 回退到 LVGL 内置字体 |
| 图片文件无法解码 | LVGL 显示默认"破损图片"占位符，不崩溃 |
| MP3 文件格式错误 | libmad 自动跳过损坏帧，服务层重试 3 次后停止播放 |
| 目录无权限 / 不存在 | 文件浏览器显示空目录，工具栏提示"无法访问" |
| 内存不足 | LVGL heap 满时 `lv_malloc` 返回 NULL，检查并降级 |

### 15.5 最终验收清单

| 编号 | 功能项 | 验收标准 |
|---|---|---|
| F1 | 主菜单 | 三个图标按钮可见，点击跳转正确 |
| F2 | 文件浏览器 | 显示目录树，点击目录进入，点击文件选中 |
| F3 | 图片查看 | 显示图片，放大/缩小/上一张/下一张，预加载正常 |
| F4 | 幻灯片播放 | 自动切换图片，间隔可配置，暂停/继续正常 |
| F5 | 设置页面 | 设置项可点击，间隔设置页面加减正常 |
| F6 | 文本阅读器 | 显示文本，翻页正常，中文不乱码 |
| F7 | 音乐播放器 | 播放/暂停/停止/音量调节/进度条更新正常 |
| F8 | 触摸交互 | 触摸屏点击、拖拽均有响应 |
| F9 | 鼠标交互 | 鼠标移动/点击/滚轮有响应，光标显示正常 |
| F10 | 全局返回 | 各页面返回按钮返回正确页面 |
| F11 | 内存稳定性 | 连续操作 30 分钟无内存泄漏、无崩溃 |
| F12 | 编译 | `make clean && make` 无警告无错误 |

### 15.6 第十五阶段验证

按照 F1~F12 逐项测试，记录问题并修复。全部通过后方可视为重构完成。

---

## 附录 A：依赖关系矩阵

下表中 `✓` 表示行模块依赖列模块：

| 模块 ↓ \ 依赖 → | lvgl | tick | debug | list | display | input | audio | file_svc | music_svc | image_svc | styles | screens | widgets |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| **util/tick** | | | | | | | | | | | | | |
| **util/debug** | | | | | | | | | | | | | |
| **util/list** | | | | | | | | | | | | | |
| **hal/display** | ✓ | | | | | | | | | | | | |
| **hal/input** | ✓ | | | | | | | | | | | | |
| **hal/audio** | | | | | | | | | | | | | |
| **service/file** | | | ✓ | ✓ | | | | | | | | | |
| **service/music** | | | ✓ | | | | ✓ | ✓ | | | | | |
| **service/image** | ✓ | | ✓ | | | | | ✓ | | | | | |
| **ui/styles** | ✓ | | | | | | | | | | | | |
| **ui/widgets** | ✓ | | | | | | | ✓ | | | ✓ | | |
| **ui/screens** | ✓ | | | | | | | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ |
| **app/** | ✓ | ✓ | ✓ | | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | |

## 附录 B：头文件包含规范

各层头文件只能包含以下来源：

| 层 | 可包含的头文件 |
|---|---|
| **HAL** | `<标准C库>`、`<linux/fb.h>`、`<alsa/asoundlib.h>`、`<tslib.h>`、`<mad.h>`、`lvgl.h`、`config.h` |
| **Service** | `<标准C库>`、`util/debug.h`、`util/list.h`、`hal/audio/*.h`（仅 music_service）、`config.h` |
| **UI** | `<标准C库>`、`lvgl.h`、`service/file_service.h`、`ui/styles.h`、`ui/widgets/*.h`、`config.h` |
| **App** | 所有层的公开头文件 |

**禁止**：
- UI 层直接包含 HAL 层头文件
- Service 层包含 UI 层头文件
- 各层的 `.c` 文件包含其他层的 `.c` 文件

## 附录 C：LVGL 对象层级结构（示例）

```
lv_screen_active()
  ├── lv_obj (背景容器, styles->bg)
  │   ├── lv_label (标题)
  │   ├── lv_obj (按钮行容器, flex 布局)
  │   │   ├── lv_btn → lv_label (浏览)
  │   │   ├── lv_btn → lv_label (自动播放)
  │   │   └── lv_btn → lv_label (设置)
  │   └── lv_obj (底部状态栏)
  │       └── lv_label (版本信息)
  │
  └── lv_obj (弹出层/对话框, 默认隐藏)
```
