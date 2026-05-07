# digitpic-lvgl top-level Makefile
# Builds LVGL library + layered application sub-libraries → final binary

include common.mk

# LVGL source file lists (sets CSRCS, ASRCS).
# Requires: LVGL_DIR (= TOP_DIR), LVGL_DIR_NAME (= lvgl),
#           LV_DRIVERS_DIR_NAME (= lv_drivers)
LVGL_DIR_NAME      := lvgl
LV_DRIVERS_DIR_NAME := lv_drivers

include $(LVGL_DIR)/lvgl/lvgl.mk
include $(LVGL_DIR)/lv_drivers/lv_drivers.mk

# Subdirectories producing built-in.a, in dependency order.
# Note: ui/ merges screens/ and widgets/ into its own archive,
# so we do NOT list ui/screens and ui/widgets separately.
SUB_DIRS := util hal/display hal/input hal/audio service ui app
SUB_LIBS := $(patsubst %,%/built-in.a,$(SUB_DIRS))

BUILD_DIR := build
TARGET   := $(BUILD_DIR)/digitpic

CFLAGS  += -I$(TOP_DIR)
LDFLAGS := -lm -lpthread

.PHONY: all clean $(SUB_DIRS)

all: $(TARGET)

# Recursively build each subdirectory
$(SUB_DIRS):
	$(MAKE) -C $@

define sub_lib_rule
$(1)/built-in.a: $(1)
endef
$(foreach dir,$(SUB_DIRS),$(eval $(call sub_lib_rule,$(dir))))

# Ensure build/ exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/lvgl/src
	mkdir -p $(BUILD_DIR)/lv_drivers

# main.c → build/main.o
$(BUILD_DIR)/main.o: main.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -std=c99 -c $< -o $@

# LVGL sources → build/<path>.o
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -std=c99 -c $< -o $@

# LVGL assembly sources
$(BUILD_DIR)/%.o: %.S | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Build lists
LVGL_COBJS  := $(patsubst %.c,$(BUILD_DIR)/%.o,$(CSRCS))
LVGL_AOBJS  := $(patsubst %.S,$(BUILD_DIR)/%.o,$(ASRCS))
MAIN_OBJ    := $(BUILD_DIR)/main.o

# Final link
$(TARGET): $(MAIN_OBJ) $(LVGL_COBJS) $(LVGL_AOBJS) $(SUB_LIBS)
	$(CC) -o $@ $^ $(LDFLAGS)
	@echo "===== BUILD SUCCESS: $(TARGET) ====="

clean:
	@for dir in $(SUB_DIRS); do \
		$(MAKE) -C $$dir clean; \
	done
	rm -rf $(BUILD_DIR)
