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
	rm -f *.o *.a
