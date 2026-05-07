# hal/Makefile — recursive into subdirectories
SUB_DIRS := display input audio

.PHONY: all clean $(SUB_DIRS)

all: $(SUB_DIRS)

$(SUB_DIRS):
	$(MAKE) -C $@

clean:
	@for dir in $(SUB_DIRS); do $(MAKE) -C $$dir clean; done
