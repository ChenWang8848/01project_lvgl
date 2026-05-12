# ================================================================
# 顶层 Makefile — 单遍构建: 编译 LVGL + 全部应用层源码 → 最终可执行文件
# ================================================================
# 构建产物: build/digitpic
# 交叉编译: arm-linux-gnueabihf-gcc
#
# 架构说明:
#  本项目采用分层架构 (由上到下):
#    app/        — 应用控制器、页面管理器
#    ui/         — 页面 screens + 自定义控件 widgets + 全局样式 styles
#    service/    — 文件服务、音乐服务、图片服务
#    hal/        — 硬件抽象层 (display/input/audio)
#    util/       — 调试日志、链表工具
#    config/     — 全局配置宏
#
#  LVGL 源码通过 lvgl.mk / lv_drivers.mk 引入，
#  其 VPATH 机制自动在 lvgl/src/* 中查找 .c 文件。
# ================================================================

include common.mk

# --- LVGL 源码列表 (设置 CSRCS/ASRCS 和 VPATH) ---
LVGL_DIR_NAME       := lvgl
LV_DRIVERS_DIR_NAME := lv_drivers

include $(LVGL_DIR)/lvgl/lvgl.mk
include $(LVGL_DIR)/lv_drivers/lv_drivers.mk

# --- 构建输出路径 ---
BUILD_DIR := build
TARGET    := $(BUILD_DIR)/digitpic

# --- 本项目的分层源码 (相对于项目根目录) ---
OUR_SRCS := \
	util/debug.c util/list.c \
	hal/display/fb_driver.c hal/display/lvgl_display.c \
	hal/input/touchscreen.c hal/input/mouse.c hal/input/lvgl_input.c \
	hal/audio/alsa_output.c hal/audio/mp3_decoder.c \
	service/file_service.c service/music_service.c service/image_service.c \
	service/image_cache.c \
	ui/styles.c \
	ui/screens/main_screen.c ui/screens/browse_screen.c \
	ui/screens/manual_screen.c ui/screens/auto_screen.c \
	ui/screens/setting_screen.c ui/screens/interval_screen.c \
	ui/screens/text_screen.c ui/screens/music_screen.c \
	ui/widgets/icon_button.c ui/widgets/file_browser.c \
	app/app.c app/page_manager.c

OUR_OBJS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(OUR_SRCS))

# --- LVGL 对象文件 ---
LVGL_COBJS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(CSRCS))
LVGL_AOBJS := $(patsubst %.S,$(BUILD_DIR)/%.o,$(ASRCS))

# --- 主程序入口 ---
MAIN_OBJ := $(BUILD_DIR)/main.o

# --- 编译/链接选项 ---
CFLAGS  += -I$(TOP_DIR)
LDFLAGS := -lm -lpthread -L/usr/local/freetype-arm/lib -lfreetype


.PHONY: all clean

all: $(TARGET)

# --- 最终链接 ---
$(TARGET): $(MAIN_OBJ) $(OUR_OBJS) $(LVGL_COBJS) $(LVGL_AOBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
	@echo "===== 构建成功: $(TARGET) ====="

# --- 确保 build/ 目录存在 ---
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# --- 模式规则: .c → build/<路径>.o ---
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# --- 模式规则: .S (汇编) → build/<路径>.o ---
$(BUILD_DIR)/%.o: %.S | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# --- 清理 ---
clean:
	rm -rf $(BUILD_DIR)
