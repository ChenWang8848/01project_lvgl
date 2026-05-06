#
# Makefile
#
ARCH = arm
CC = arm-linux-gnueabihf-gcc
LVGL_DIR_NAME ?= lvgl
LVGL_DIR ?= ${shell pwd}

# 定义输出目录
BUILD_DIR = build

CFLAGS ?= -O3 -g0 -I$(LVGL_DIR)/ #-Wall -Wshadow -Wundef -Wmissing-prototypes -Wno-discarded-qualifiers -Wall -Wextra -Wno-unused-function -Wno-error=strict-prototypes -Wpointer-arith -fno-strict-aliasing -Wno-error=cpp -Wuninitialized -Wmaybe-uninitialized -Wno-unused-parameter -Wno-missing-field-initializers -Wtype-limits -Wsizeof-pointer-memaccess -Wno-format-nonliteral -Wno-cast-qual -Wunreachable-code -Wno-switch-default -Wreturn-type -Wmultichar -Wformat-security -Wno-ignored-qualifiers -Wno-error=pedantic -Wno-sign-compare -Wno-error=missing-prototypes -Wdouble-promotion -Wclobbered -Wdeprecated -Wempty-body -Wtype-limits -Wshift-negative-value -Wstack-usage=2048 -Wno-unused-value -Wno-unused-parameter -Wno-missing-field-initializers -Wuninitialized -Wmaybe-uninitialized -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers -Wtype-limits -Wsizeof-pointer-memaccess -Wno-format-nonliteral -Wpointer-arith -Wno-cast-qual -Wmissing-prototypes -Wunreachable-code -Wno-switch-default -Wreturn-type -Wmultichar -Wno-discarded-qualifiers -Wformat-security -Wno-ignored-qualifiers -Wno-sign-compare
LDFLAGS ?= -lm
BIN = $(BUILD_DIR)/demo

MAINSRC = ./main.c

include $(LVGL_DIR)/lvgl/lvgl.mk
include $(LVGL_DIR)/lv_drivers/lv_drivers.mk

#CSRCS +=$(LVGL_DIR)/mouse_cursor_icon.c 

OBJEXT ?= .o

# 将所有对象文件路径指向 build 目录
# 修复：使用相对路径或简单文件名，避免绝对路径导致的问题
AOBJS = $(ASRCS:.S=$(OBJEXT))
COBJS = $(CSRCS:.c=$(OBJEXT))
MAINOBJ = $(MAINSRC:.c=$(OBJEXT))

# 将对象文件重定向到 build 目录，并保持目录结构
AOBJS := $(patsubst %,$(BUILD_DIR)/%,$(AOBJS))
COBJS := $(patsubst %,$(BUILD_DIR)/%,$(COBJS))
MAINOBJ := $(patsubst %,$(BUILD_DIR)/%,$(MAINOBJ))

SRCS = $(ASRCS) $(CSRCS) $(MAINSRC)
OBJS = $(AOBJS) $(COBJS) $(MAINOBJ)

## MAINOBJ -> OBJFILES

all: default

# 创建 build 目录
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# 修改编译规则，输出到 build 目录
# 修复：确保目标文件的目录存在
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -std=c99 -o $@
	@echo "CC $<"

$(BUILD_DIR)/%.o: %.S | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "AS $<"
    
default: $(OBJS)
	$(CC) -o $(BIN) $(OBJS) $(LDFLAGS)
	@echo "Linking complete: $(BIN)"

clean: 
	rm -rf $(BUILD_DIR)