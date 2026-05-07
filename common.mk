# common.mk — shared by all subdirectory Makefiles
# Compute TOP_DIR relative to this file's own location

CROSS_COMPILE := arm-linux-gnueabihf-
CC  := $(CROSS_COMPILE)gcc
AR  := $(CROSS_COMPILE)ar

TOP_DIR := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))

# LVGL_DIR must point to the project root for lvgl.mk path resolution.
# LVGL_DIR_NAME is the subdirectory name containing lvgl.
# Together: $(LVGL_DIR)/$(LVGL_DIR_NAME)/ → project_root/lvgl/
LVGL_DIR     := $(TOP_DIR)
LVGL_DIR_NAME := lvgl

INCLUDES := \
	-I$(TOP_DIR)/config \
	-I$(TOP_DIR)/app \
	-I$(TOP_DIR)/ui -I$(TOP_DIR)/ui/screens -I$(TOP_DIR)/ui/widgets \
	-I$(TOP_DIR)/service \
	-I$(TOP_DIR)/hal/display -I$(TOP_DIR)/hal/input -I$(TOP_DIR)/hal/audio \
	-I$(TOP_DIR)/util \
	-I$(TOP_DIR) \
	-I$(LVGL_DIR)/lvgl \
	-I$(LVGL_DIR)/lv_drivers

CFLAGS := -Wall -O2 -g -std=gnu99 $(INCLUDES)
