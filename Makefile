# digitpic-lvgl top-level Makefile
# Single-pass build: compiles LVGL + all layered application sources → final binary

include common.mk

# For LVGL .mk files: LVGL_DIR must be project root, LVGL_DIR_NAME = lvgl
LVGL_DIR_NAME       := lvgl
LV_DRIVERS_DIR_NAME := lv_drivers

include $(LVGL_DIR)/lvgl/lvgl.mk
include $(LVGL_DIR)/lv_drivers/lv_drivers.mk

BUILD_DIR := build
TARGET    := $(BUILD_DIR)/digitpic

# Our layered source files (relative to project root)
OUR_SRCS := \
	util/debug.c util/list.c \
	hal/display/fb_driver.c hal/display/lvgl_display.c \
	hal/input/touchscreen.c hal/input/mouse.c hal/input/lvgl_input.c \
	hal/audio/alsa_output.c hal/audio/mp3_decoder.c \
	service/file_service.c service/music_service.c service/image_service.c \
	ui/styles.c \
	ui/screens/main_screen.c ui/screens/browse_screen.c \
	ui/screens/manual_screen.c ui/screens/auto_screen.c \
	ui/screens/setting_screen.c ui/screens/interval_screen.c \
	ui/screens/text_screen.c ui/screens/music_screen.c \
	ui/widgets/icon_button.c ui/widgets/file_browser.c \
	app/app.c app/page_manager.c

OUR_OBJS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(OUR_SRCS))

# LVGL objects (CSRCS/ASRCS set by lvgl.mk / lv_drivers.mk)
LVGL_COBJS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(CSRCS))
LVGL_AOBJS := $(patsubst %.S,$(BUILD_DIR)/%.o,$(ASRCS))

MAIN_OBJ := $(BUILD_DIR)/main.o

# Additional include paths from LVGL .mk files may be quoted; ensure TOP_DIR is in path
CFLAGS  += -I$(TOP_DIR)
LDFLAGS := -lm -lpthread

.PHONY: all clean

all: $(TARGET)

# Link
$(TARGET): $(MAIN_OBJ) $(OUR_OBJS) $(LVGL_COBJS) $(LVGL_AOBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
	@echo "===== BUILD SUCCESS: $(TARGET) ====="

# Ensure build/ exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Pattern rule: any .c → build/<path>.o
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Pattern rule: assembly → build/<path>.o
$(BUILD_DIR)/%.o: %.S | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
