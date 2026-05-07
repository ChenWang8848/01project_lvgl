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
	rm -f *.o *.a
