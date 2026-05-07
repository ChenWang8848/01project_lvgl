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
	rm -f *.o *.a
