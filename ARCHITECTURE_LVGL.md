# digitpic LVGL 重构架构设计文档

## 一、分层架构总览

```
┌──────────────────────────────────────────────────────────────────┐
│                        Application Layer                         │
│   app/ : 应用控制器、页面管理器、主事件循环                         │
├──────────────────────────────────────────────────────────────────┤
│                       UI Layer (Presentation)                    │
│   ui/screens/ : 各功能页面（LVGL screen）                         │
│   ui/widgets/ : 自定义可复用控件                                  │
│   ui/styles   : 全局样式/主题定义                                  │
├──────────────────────────────────────────────────────────────────┤
│                     Service Layer (Business Logic)               │
│   service/ : 文件服务、音乐服务、图片服务                           │
├──────────────────────────────────────────────────────────────────┤
│                   HAL (Hardware Abstraction Layer)                │
│   hal/display/ : LVGL 显示驱动（framebuffer 抽象）                 │
│   hal/input/   : LVGL 输入设备驱动（触摸屏/鼠标）                   │
│   hal/audio/   : 音频输出抽象层（ALSA + libmad 解码）              │
├──────────────────────────────────────────────────────────────────┤
│                         Utility Layer                            │
│   util/  : 调试日志、链表工具                                     │
│   config/: 全局配置宏、颜色定义                                    │
└──────────────────────────────────────────────────────────────────┘
```

**依赖规则**：上层依赖下层，下层不依赖上层；同层之间通过接口通信。

---

## 二、目录结构

项目需要新建 **10 个文件夹**，每个文件夹均有自己的 Makefile：

```
digitpic-lvgl/
│
├── main.c                              # 程序入口
├── Makefile                            # 顶层 Makefile
│
├── app/                                # 应用层 (1 个文件夹)
│   ├── app.h                           #   应用控制器接口
│   ├── app.c                           #   初始化序列、主循环
│   ├── page_manager.h                  #   页面管理器接口
│   ├── page_manager.c                  #   页面注册、切换
│   └── Makefile
│
├── ui/                                 # UI 层 (3 个文件夹)
│   ├── styles.h                        #   全局样式声明
│   ├── styles.c                        #   样式初始化（颜色、字体、按钮等）
│   ├── Makefile                        #   递归进入 screens/ 和 widgets/
│   │
│   ├── screens/                        #   页面 = LVGL screen
│   │   ├── base_screen.h               #     页面基类（虚表）
│   │   ├── main_screen.h / .c          #     主菜单
│   │   ├── browse_screen.h / .c        #     文件浏览器
│   │   ├── manual_screen.h / .c        #     图片查看器
│   │   ├── auto_screen.h / .c          #     幻灯片播放
│   │   ├── setting_screen.h / .c       #     设置菜单
│   │   ├── interval_screen.h / .c      #     间隔设置
│   │   ├── text_screen.h / .c          #     文本阅读器
│   │   ├── music_screen.h / .c         #     音乐播放器
│   │   └── Makefile
│   │
│   └── widgets/                        #   自定义 LVGL 控件
│       ├── icon_button.h / .c          #     图标+文字按钮
│       ├── file_browser.h / .c         #     文件浏览器网格控件
│       └── Makefile
│
├── service/                            # 服务层 (1 个文件夹)
│   ├── file_service.h                  #   文件服务接口
│   ├── file_service.c                  #   mmap 文件读写、目录遍历
│   ├── music_service.h                 #   音乐服务接口
│   ├── music_service.c                 #   播放控制状态机
│   ├── image_service.h                 #   图片服务接口
│   ├── image_service.c                 #   图片预加载、缓存管理
│   └── Makefile
│
├── hal/                                # 硬件抽象层 (4 个文件夹)
│   ├── Makefile                        #   递归进入子目录
│   │
│   ├── display/                        #   显示抽象
│   │   ├── display_driver.h            #     显示驱动虚基类
│   │   ├── fb_driver.h / .c            #     Linux framebuffer 实现
│   │   ├── lvgl_display.h / .c         #     LVGL 显示驱动适配（flush 回调）
│   │   └── Makefile
│   │
│   ├── input/                          #   输入抽象
│   │   ├── input_driver.h              #     输入设备虚基类
│   │   ├── touchscreen.h / .c          #     tslib 触摸屏驱动
│   │   ├── mouse.h / .c                #     USB 鼠标驱动
│   │   ├── lvgl_input.h / .c           #     LVGL 输入设备适配（read 回调）
│   │   └── Makefile
│   │
│   └── audio/                          #   音频抽象
│       ├── audio_output.h              #     音频输出虚基类
│       ├── alsa_output.h / .c          #     ALSA PCM 输出实现
│       ├── mp3_decoder.h / .c          #     libmad MP3 解码器
│       └── Makefile
│
├── util/                               # 工具层 (1 个文件夹)
│   ├── debug.h / .c                    #   调试日志（stdout + UDP）
│   ├── list.h / .c                     #   通用双向链表
│   └── Makefile
│
├── config/                             # 配置 (1 个文件夹)
│   ├── config.h                        #   全局宏、颜色、尺寸常量
│   └── Makefile                        #   （空操作，仅头文件）
│
├── lib/                                # 第三方库（不参与内部构建）
│   ├── lvgl/                           #   LVGL v8.x / v9.x
│   ├── lv_drivers/                     #   LVGL 驱动（可选）
│   ├── freetype-2.14.3/                #   FreeType
│   ├── tslib/                          #   tslib（触摸屏）
│   ├── libjpeg-turbo-1.2.1/            #   libjpeg-turbo
│   ├── libmad-0.15.1b/                 #   libmad（MP3 解码）
│   └── alsa-lib-1.2.7/                 #   ALSA-lib
│
└── assets/                             # 资源文件
    ├── fonts/                          #   TTF 字体文件
    └── icons/                          #   图标（PNG/BMP）
```

---

## 三、面向对象类设计

### 3.1 设计原则

- **封装**：每个模块只暴露其 `.h` 中的公开接口（函数指针、创建/销毁函数），内部实现细节在 `.c` 中隐藏。
- **继承**：使用结构体嵌套实现"父类"→"子类"继承。子类第一个成员为父类结构体。
- **多态**：通过函数指针表（vtable）实现运行时多态。各子类填充自己的函数实现。
- **单例**：服务层对象使用全局单例模式，避免重复创建。

### 3.2 HAL 层类设计

```
┌───────────────────────────────────────────────────────────┐
│  display_driver_t (虚基类)                                │
│  ├── name: const char*                                    │
│  ├── init(self) -> int              (纯虚)                │
│  ├── get_resolution(self, *w, *h)   (纯虚)                │
│  ├── get_bpp(self) -> int           (纯虚)                │
│  └── flush(self, disp, area, px)    (纯虚) LVGL 回调      │
│       ↓ 继承                                              │
│  fb_driver_t : display_driver_t                           │
│       ├── fd: int                      (/dev/fb0)         │
│       ├── fb_mmap: void*               (mmap 指针)        │
│       ├── fb_info: struct fb_var_screeninfo                │
│       └── 实现所有虚函数                                   │
├───────────────────────────────────────────────────────────┤
│  input_driver_t (虚基类)                                  │
│  ├── name: const char*                                    │
│  ├── init(self) -> int              (纯虚)                │
│  ├── read(self, *data) -> bool       (纯虚) LVGL 回调     │
│  └── deinit(self)                   (纯虚)                │
│       ↓ 继承                                              │
│  touchscreen_t : input_driver_t                           │
│       ├── ts_dev: struct tsdev*                           │
│       └── 实现所有虚函数                                   │
│  mouse_t : input_driver_t                                 │
│       ├── fd: int                      (/dev/input/eventX)│
│       └── 实现所有虚函数                                   │
├───────────────────────────────────────────────────────────┤
│  audio_output_t (虚基类)                                  │
│  ├── name: const char*                                    │
│  ├── init(self, rate, ch, bits) -> int                    │
│  ├── write(self, buf, frames) -> int                      │
│  ├── drain(self) -> int                                   │
│  └── deinit(self)                                         │
│       ↓ 继承                                              │
│  alsa_output_t : audio_output_t                           │
│       ├── pcm: snd_pcm_t*                                 │
│       ├── ctl: snd_ctl_t*                                 │
│       └── 实现所有虚函数                                   │
├───────────────────────────────────────────────────────────┤
│  mp3_decoder_t (独立类)                                   │
│  ├── init(self) -> int                                    │
│  ├── decode(self, data, len, **pcm, *pcm_len) -> int      │
│  ├── seek(self, pos) -> int                               │
│  └── deinit(self)                                         │
└───────────────────────────────────────────────────────────┘
```

### 3.3 Service 层类设计

```
┌───────────────────────────────────────────────────────────┐
│  file_service_t (单例)                                    │
│  ├── map_file(path) -> file_map_t*                        │
│  ├── unmap_file(map)                                      │
│  ├── list_dir(path) -> dir_entry_t[]                      │
│  ├── walk_dir(path, depth) -> file_list_t                 │
│  └── get_file_ext(path) -> char*                          │
├───────────────────────────────────────────────────────────┤
│  music_service_t (单例)                                   │
│  ├── play(path) -> int                                    │
│  ├── pause() / resume() / stop()                          │
│  ├── set_volume(vol)                                      │
│  ├── get_state() -> music_state_t                         │
│  ├── get_progress(*cur, *total)                           │
│  └── register_callback(event, cb)                         │
├───────────────────────────────────────────────────────────┤
│  image_service_t (单例)                                   │
│  ├── load(path) -> lv_image_dsc_t*                        │
│  ├── preload(path)   (后台线程预加载)                      │
│  ├── cancel_preload(path)                                 │
│  └── get_cache_info(*count, *mem)                         │
└───────────────────────────────────────────────────────────┘
```

### 3.4 UI 层类设计

```
┌───────────────────────────────────────────────────────────┐
│  base_screen_t (页面虚基类)                                │
│  ├── name: const char*                                    │
│  ├── screen: lv_obj_t*         (LVGL screen 对象)         │
│  ├── create(self) -> lv_obj_t* (纯虚，构建 widget 树)     │
│  ├── on_enter(self, *data)     (可选，页面进入回调)       │
│  ├── on_exit(self)             (可选，页面退出回调)       │
│  └── destroy(self)             (可选)                     │
│       ↓ 继承                                              │
│  main_screen_t : base_screen_t                            │
│  browse_screen_t : base_screen_t                          │
│  manual_screen_t : base_screen_t                          │
│  ... (共 8 个 screen 子类)                                │
└───────────────────────────────────────────────────────────┘
```

### 3.5 App 层类设计

```
┌───────────────────────────────────────────────────────────┐
│  page_manager_t (单例)                                    │
│  ├── register(screen: base_screen_t*)                     │
│  ├── navigate(name, *user_data)    (页面切换)             │
│  ├── get_current() -> base_screen_t*                      │
│  └── go_back()                                            │
├───────────────────────────────────────────────────────────┤
│  app_controller_t (单例)                                  │
│  ├── init(argc, argv)              (总初始化)             │
│  ├── run()                         (进入 LVGL 主循环)     │
│  └── cleanup()                     (资源清理)             │
└───────────────────────────────────────────────────────────┘
```

---

## 四、模块替换对照表

| 原有模块 | 新模块 | 说明 |
|---|---|---|
| `display/fb.c`、`disp_manager.c` | `hal/display/fb_driver.c`、`lvgl_display.c` | framebuffer 操作保留，VideoMem 池废弃，改用 LVGL 的 draw buffer |
| `input/touchscreen.c`、`mouse.c`、`input_manager.c` | `hal/input/touchscreen.c`、`mouse.c`、`lvgl_input.c` | 设备读取保留，环形缓冲区和条件变量废弃，改用 LVGL indev |
| `fonts/freetype.c`、`fonts_manager.c` | LVGL FreeType 集成 | LVGL 内置 FreeType 支持，自定义字体管理代码全部废弃 |
| `encoding/utf-8.c`、`encoding_manager.c` | LVGL UTF-8 原生支持 | LVGL 已内置 UTF-8 处理 |
| `render/format/bmp.c`、`jpg.c`、`picfmt_manager.c` | LVGL 内置图像解码 | LVGL 支持 BMP/PNG/JPG 解码，自定义解析器废弃 |
| `render/operation/zoom.c`、`merge.c` | LVGL 图像缩放/混合 | 使用 `lv_image_set_zoom()` / `lv_canvas` |
| `render/render.c` | LVGL 绘制引擎 | 按钮反转、文本合并、矩形填充等全部由 LVGL 替代 |
| `page/` 下全部 8 个页面 | `ui/screens/` 下 8 个 screen | 手动布局计算 → LVGL 声明式布局 (flex/grid) |
| `page/page_manager.c` | `app/page_manager.c` | 页面注册/切换用 `lv_screen_load()` |
| `draw/draw.c` | `ui/screens/text_screen.c` | 文字排版引擎全部由 `lv_label` 替代 |
| `music/music_manager.c`、`mp3.c` | `service/music_service.c`、`hal/audio/mp3_decoder.c`、`alsa_output.c` | 功能保留，结构拆分：解码/输出/控制分离 |
| `file/file.c` | `service/file_service.c` | 功能保留，接口封装 |
| `debug/` | `util/debug.c` | 保留，精简整合 |
| `system/common_st.c` | `util/list.c` | 保留 |

### 完全废弃的模块

- `display/disp_manager.c` — VideoMem 池管理（LVGL draw buffer 替代）
- `input/input_manager.c` — 环形缓冲区 + 条件变量（LVGL indev 替代）
- `fonts/fonts_manager.c` — 字体插件注册表（LVGL 字体管理替代）
- `encoding/encoding_manager.c` — 编码检测注册表（LVGL 内置 UTF-8 替代）
- `render/format/picfmt_manager.c` — 图片格式注册表（LVGL 内置解码替代）
- `render/render.c` — 手动像素绘制（LVGL 绘制引擎替代）
- `render/operation/` — 手动缩放/混合（LVGL 图像引擎替代）
- `draw/draw.c` — 文字排版引擎（LVGL `lv_label` 替代）
- `jpg2rgb.c`、`numid.c`、`netprint_client.c` — 独立调试工具，保留不参与构建

---

## 五、数据流图

```
                    ┌──────────────┐
                    │   main.c     │
                    └──────┬───────┘
                           │ app_controller_init()
                           v
              ┌─────────────────────────┐
              │    app_controller_t     │
              │  ┌───────────────────┐  │
              │  │ 1. 初始化 HAL 层  │  │
              │  │ 2. 初始化服务层   │  │
              │  │ 3. 初始化 UI 层   │  │
              │  │ 4. 注册所有页面   │  │
              │  │ 5. 进入主循环     │  │
              │  └───────────────────┘  │
              └─────────┬───────────────┘
                        │
          ┌─────────────┼─────────────┐
          v             v             v
    ┌──────────┐ ┌──────────┐ ┌──────────┐
    │   HAL    │ │ Service  │ │    UI    │
    │  display │ │  file    │ │ screens  │
    │  input   │ │  music   │ │ widgets  │
    │  audio   │ │  image   │ │ styles   │
    └──────────┘ └──────────┘ └──────────┘
          │             │             │
          └─────────────┼─────────────┘
                        │ 事件驱动
                        v
              ┌─────────────────┐
              │  LVGL 主循环    │
              │  while(1) {     │
              │    lv_timer_handler(); │
              │    usleep(5000);│
              │  }              │
              └─────────────────┘
```

**事件流**：
```
硬件输入 ──→ HAL/input (tslib/mouse 读取)
                 │ lv_indev_read_callback()
                 v
           LVGL indev ──→ LVGL 事件系统 ──→ Screen 事件回调
                                                  │
                                                  v
                                          Service 层调用
                                          (播放音乐 / 加载文件 / ...)
                                                  │
                                                  v
                                          HAL/audio 或 HAL/display
```

---

## 六、各文件夹 Makefile

### 6.1 顶层 Makefile

```makefile
# digitpic-lvgl/Makefile
# 顶层构建 — 编译 main.c 并链接所有子目录的静态库和第三方库

CROSS_COMPILE := arm-linux-gnueabihf-
CC  := $(CROSS_COMPILE)gcc
AR  := $(CROSS_COMPILE)ar

# 第三方库路径
LVGL_DIR      := lib/lvgl
FREETYPE_DIR  := lib/freetype-2.14.3
TSLIB_DIR     := lib/tslib
JPEG_DIR      := lib/libjpeg-turbo-1.2.1
MAD_DIR       := lib/libmad-0.15.1b
ALSA_DIR      := lib/alsa-lib-1.2.7

# 头文件搜索路径
INCLUDES := \
	-Iconfig \
	-Iapp \
	-Iui -Iui/screens -Iui/widgets \
	-Iservice \
	-Ihal/display -Ihal/input -Ihal/audio \
	-Iutil \
	-I$(LVGL_DIR) \
	-I$(FREETYPE_DIR)/include \
	-I$(TSLIB_DIR)/include \
	-I$(JPEG_DIR) \
	-I$(MAD_DIR) \
	-I$(ALSA_DIR)/include

# 编译选项
CFLAGS   := -Wall -O2 -g -std=gnu99 $(INCLUDES)
LDFLAGS  := -L$(FREETYPE_DIR) -L$(TSLIB_DIR)/lib -L$(JPEG_DIR) -L$(MAD_DIR) -L$(ALSA_DIR)/lib
LDLIBS   := -lfreetype -lts -ljpeg -lmad -lasound -lm -lpthread

# 需要构建子目录 Makefile 的文件夹（按依赖顺序）
SUB_DIRS := util hal/display hal/input hal/audio service ui/screens ui/widgets ui app

# 收集所有子目录生成的静态库
SUB_LIBS := $(patsubst %,%/built-in.a,$(SUB_DIRS))

TARGET := digitpic

.PHONY: all clean $(SUB_DIRS)

all: $(TARGET)

# 构建 LVGL 库
$(LVGL_DIR)/liblvgl.a:
	$(MAKE) -C $(LVGL_DIR)

# 递归构建各子目录
$(SUB_DIRS):
	$(MAKE) -C $@

# 各子目录的静态库依赖其子目录构建
define sub_lib_rule
$(1)/built-in.a: $(1)
endef
$(foreach dir,$(SUB_DIRS),$(eval $(call sub_lib_rule,$(dir))))

# 最终链接
$(TARGET): main.o $(LVGL_DIR)/liblvgl.a $(SUB_LIBS)
	$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)

main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@for dir in $(SUB_DIRS); do \
		$(MAKE) -C $$dir clean; \
	done
	rm -f main.o $(TARGET)
```

### 6.2 config/Makefile

```makefile
# digitpic-lvgl/config/Makefile
# config/ 仅包含头文件，无编译目标

.PHONY: all clean

all:
	@:

clean:
	@:
```

### 6.3 util/Makefile

```makefile
# digitpic-lvgl/util/Makefile

include ../common.mk

SRCS := debug.c list.c
OBJS := $(SRCS:.c=.o)
TARGET := built-in.a

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(AR) rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o *.a *.d
```

### 6.4 hal/Makefile（容器，仅递归）

```makefile
# digitpic-lvgl/hal/Makefile
# 递归进入下级子目录

SUB_DIRS := display input audio

.PHONY: all clean $(SUB_DIRS)

all: $(SUB_DIRS)

$(SUB_DIRS):
	$(MAKE) -C $@

clean:
	@for dir in $(SUB_DIRS); do \
		$(MAKE) -C $$dir clean; \
	done
```

### 6.5 hal/display/Makefile

```makefile
# digitpic-lvgl/hal/display/Makefile

include ../../common.mk

SRCS := fb_driver.c lvgl_display.c
OBJS := $(SRCS:.c=.o)
TARGET := built-in.a

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(AR) rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o *.a *.d
```

### 6.6 hal/input/Makefile

```makefile
# digitpic-lvgl/hal/input/Makefile

include ../../common.mk

SRCS := touchscreen.c mouse.c lvgl_input.c
OBJS := $(SRCS:.c=.o)
TARGET := built-in.a

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(AR) rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o *.a *.d
```

### 6.7 hal/audio/Makefile

```makefile
# digitpic-lvgl/hal/audio/Makefile

include ../../common.mk

SRCS := alsa_output.c mp3_decoder.c
OBJS := $(SRCS:.c=.o)
TARGET := built-in.a

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(AR) rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o *.a *.d
```

### 6.8 service/Makefile

```makefile
# digitpic-lvgl/service/Makefile

include ../common.mk

SRCS := file_service.c music_service.c image_service.c
OBJS := $(SRCS:.c=.o)
TARGET := built-in.a

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(AR) rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o *.a *.d
```

### 6.9 ui/Makefile（容器，递归 + 本层源文件）

```makefile
# digitpic-lvgl/ui/Makefile
# 构建本层的 styles.c，并递归构建子目录

include ../common.mk

SUB_DIRS := screens widgets
SRCS := styles.c
OBJS := $(SRCS:.c=.o)
TARGET := built-in.a

.PHONY: all clean $(SUB_DIRS)

all: $(TARGET)

$(SUB_DIRS):
	$(MAKE) -C $@

# 将本层 .o 与子目录的 .a 合并为一个 archive
$(TARGET): $(OBJS) $(SUB_DIRS)
	$(AR) rcs $@ $(OBJS) $$(find $(SUB_DIRS) -name '*.o')

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o *.a *.d
	@for dir in $(SUB_DIRS); do \
		$(MAKE) -C $$dir clean; \
	done
```

### 6.10 ui/screens/Makefile

```makefile
# digitpic-lvgl/ui/screens/Makefile

include ../../common.mk

SRCS := main_screen.c browse_screen.c manual_screen.c auto_screen.c \
        setting_screen.c interval_screen.c text_screen.c music_screen.c
OBJS := $(SRCS:.c=.o)
TARGET := built-in.a

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(AR) rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o *.a *.d
```

### 6.11 ui/widgets/Makefile

```makefile
# digitpic-lvgl/ui/widgets/Makefile

include ../../common.mk

SRCS := icon_button.c file_browser.c
OBJS := $(SRCS:.c=.o)
TARGET := built-in.a

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(AR) rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o *.a *.d
```

### 6.12 app/Makefile

```makefile
# digitpic-lvgl/app/Makefile

include ../common.mk

SRCS := app.c page_manager.c
OBJS := $(SRCS:.c=.o)
TARGET := built-in.a

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(AR) rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o *.a *.d
```

### 6.13 common.mk（公共编译变量）

```makefile
# digitpic-lvgl/common.mk
# 被各子目录 Makefile include 使用
# 注意：此路径相对于各子目录的位置不同，各 Makefile 使用相对路径 include

# 交叉编译工具链
CROSS_COMPILE := arm-linux-gnueabihf-
CC  := $(CROSS_COMPILE)gcc
AR  := $(CROSS_COMPILE)ar

# 第三方库路径（相对于项目根目录）
TOP_DIR := $(realpath $(dir $(lastword $(MAKEFILE_LIST)))/..)
LVGL_DIR     := $(TOP_DIR)/lib/lvgl
FREETYPE_DIR := $(TOP_DIR)/lib/freetype-2.14.3
TSLIB_DIR    := $(TOP_DIR)/lib/tslib
JPEG_DIR     := $(TOP_DIR)/lib/libjpeg-turbo-1.2.1
MAD_DIR      := $(TOP_DIR)/lib/libmad-0.15.1b
ALSA_DIR     := $(TOP_DIR)/lib/alsa-lib-1.2.7

# 头文件搜索路径
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
```

---

## 七、main.c 伪代码

```c
#include "app/app.h"

int main(int argc, char *argv[])
{
    app_controller_t app;

    /* 1. 创建应用控制器（解析命令行参数、加载字体路径等） */
    if (app_controller_init(&app, argc, argv) != 0) {
        return -1;
    }

    /* 2. 初始化序列（在 app_controller_init 内部完成）:
     *
     *   hal_init():
     *     display_init()   → 打开 /dev/fb0，获取分辨率/BPP
     *                       → 创建 LVGL display，绑定 flush 回调
     *     input_init()     → 打开触摸屏 + 鼠标
     *                       → 创建 LVGL indev，绑定 read 回调
     *     audio_init()     → 打开 ALSA PCM，初始化 libmad
     *
     *   service_init():
     *     file_service     → 单例，无状态
     *     music_service    → 传入 audio_output + mp3_decoder 实例
     *     image_service    → 初始化预加载线程
     *
     *   ui_init():
     *     styles_init()    → 创建全局 LVGL style 对象
     *     widgets_init()   → （自定义控件注册）
     *
     *   app_init():
     *     page_manager     → 注册全部 8 个 screen
     *     page_manager_navigate("main", NULL)  → 加载主菜单
     */

    /* 3. 进入 LVGL 事件主循环 */
    app_controller_run(&app);   /* while(1) { lv_timer_handler(); usleep(5000); } */

    /* 4. 清理（实际上不会到达这里，除非实现退出机制） */
    app_controller_cleanup(&app);
    return 0;
}
```

---

## 八、迁移建议

### 8.1 迁移顺序（由底层到上层）

| 阶段 | 内容 | 预计工作量 |
|---|---|---|
| **Phase 1** | 搭建目录结构 + common.mk + 所有 Makefile，确保空壳可编译 | 0.5 天 |
| **Phase 2** | 集成 LVGL 库，实现 `hal/display/` (fb_driver + lvgl_display)，跑通 LVGL "hello world" | 1 天 |
| **Phase 3** | 实现 `hal/input/` (touchscreen + mouse → LVGL indev)，实现触摸交互 | 1 天 |
| **Phase 4** | 实现 `util/` (debug + list)、`service/file_service` — 从旧代码迁移 | 0.5 天 |
| **Phase 5** | 实现 `ui/styles`、`ui/widgets/` — 定义全局主题和复用控件 | 1 天 |
| **Phase 6** | 逐个实现 `ui/screens/`（从 main_screen 开始） | 3 天 |
| **Phase 7** | 实现 `service/music_service`、`hal/audio/` — 音乐播放 | 1.5 天 |
| **Phase 8** | 实现 `service/image_service` — 图片预加载 | 0.5 天 |
| **Phase 9** | `app/` 集成、整体联调、边界测试 | 1.5 天 |

**总计预估：约 10 人天**

### 8.2 可复用的旧代码

| 文件 | 复用方式 |
|---|---|
| `file/file.c` | 几乎完整迁移到 `service/file_service.c`，仅调整接口命名 |
| `music/mp3.c` | 拆分为 `hal/audio/mp3_decoder.c`（解码）+ `hal/audio/alsa_output.c`（输出），逻辑可大量复用 |
| `debug/debug_manager.c`、`stdout.c`、`netprint.c` | 合并精简到 `util/debug.c` |
| `system/common_st.c` | 直接迁移到 `util/list.c` |
| `config.h` | 迁移到 `config/config.h`，删除 LVGL 已覆盖的颜色/尺寸宏 |
| `input/touchscreen.c` | 保留 tslib 原始读取逻辑，去掉环形缓冲区和 `SlipGetInputEvent` |
| `input/mouse.c` | 保留原始读取逻辑，去掉软件光标（LVGL 原生支持） |
| `display/fb.c` | 保留 framebuffer open/ioctl/mmap 逻辑，封装到 `fb_driver.c` |
| 页面布局常量 | 作为 LVGL 控件的坐标/size 参考值使用 |

### 8.3 关键注意事项

1. **LVGL tick 定时器**：需要 Linux `timerfd` 或 `setitimer` 提供 1ms tick，供 LVGL 内部计时。
2. **双缓冲**：若屏幕刷新时有撕裂，可启用 LVGL 双缓冲（需要 2 个 framebuffer 大小的内存块）。
3. **旋转**：若原项目有横竖屏切换需求，LVGL 支持软件旋转（`lv_display_set_rotation()`）。
4. **字体**：LVGL 的 FreeType 集成需要先调用 `lv_freetype_init()`，然后通过 `lv_freetype_font_create()` 加载 TTF 文件。
5. **内存预算**：嵌入式设备内存有限，注意 LVGL heap (`lv_conf.h` 中 `LV_MEM_SIZE`) 和 draw buffer 大小的配置。

---

## 九、文件数量统计

| 目录 | 文件数 (.c + .h) | 新增/迁移 |
|---|---|---|
| `app/` | 4 (2c + 2h) | 新增 |
| `ui/screens/` | 17 (8c + 9h，含 base_screen.h) | 新增 |
| `ui/widgets/` | 4 (2c + 2h) | 新增 |
| `ui/` | 2 (styles.c + styles.h) | 新增 |
| `service/` | 6 (3c + 3h) | 2 迁移 + 2 新增 |
| `hal/display/` | 4 (2c + 2h) | 1 迁移 + 1 新增 |
| `hal/input/` | 4 (2c + 2h) | 1 迁移 + 1 新增 |
| `hal/audio/` | 4 (2c + 2h) | 迁移 |
| `util/` | 4 (2c + 2h) | 迁移 |
| `config/` | 1 (config.h) | 迁移 |
| **合计** | **50** | |

**Makefile 数量**：12 个（顶层 1 + 子目录 11）
