include ../common.mk

SUB_DIRS := screens widgets

# Styles compiles at this level
SRCS := styles.c
OBJS := $(SRCS:.c=.o)
TARGET := built-in.a

.PHONY: all clean $(SUB_DIRS)

all: $(TARGET)

$(SUB_DIRS):
	$(MAKE) -C $@

$(TARGET): $(OBJS) $(SUB_DIRS)
	$(AR) rcs $@ $(OBJS) $$(find $(SUB_DIRS) -name '*.o')

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o *.a
	@for dir in $(SUB_DIRS); do $(MAKE) -C $$dir clean; done
