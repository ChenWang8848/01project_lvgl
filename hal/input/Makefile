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
	rm -f *.o *.a
