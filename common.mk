# ================================================================
# common.mk — 公共编译变量，被所有子目录 Makefile 和顶层 Makefile include
# ================================================================
# 职责:
#   1. 指定交叉编译工具链 (arm-linux-gnueabihf-)
#   2. 计算项目根目录 TOP_DIR (基于本文件自身位置)
#   3. 汇总所有模块的头文件搜索路径
#   4. 设置统一的 CFLAGS
# ================================================================

# --- 交叉编译工具链 ---
CROSS_COMPILE := arm-linux-gnueabihf-
CC  := $(CROSS_COMPILE)gcc
AR  := $(CROSS_COMPILE)ar

# --- 项目根目录 (common.mk 所在目录) ---
TOP_DIR := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))

# --- LVGL 路径配置 ---
# LVGL .mk 文件内部使用 $(LVGL_DIR)/$(LVGL_DIR_NAME)/... 拼接路径
# LVGL_DIR 必须指向项目根目录, LVGL_DIR_NAME = lvgl
# 最终效果: $(LVGL_DIR)/$(LVGL_DIR_NAME)/ → <项目根>/lvgl/
LVGL_DIR      := $(TOP_DIR)
LVGL_DIR_NAME := lvgl

# --- 头文件搜索路径 (分层架构各模块) ---
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

# --- 编译选项 ---
# -std=c99: 使用 C99 标准 (兼容 LVGL 源码)
CFLAGS := -Wall -O2 -g -std=c99 $(INCLUDES)
