CPPFLAGS=$(shell sdl2-config --cflags) $(EXTRA_CPPFLAGS)
LDLIBS=$(shell sdl2-config --libs) -lGLEW $(EXTRA_LDLIBS)
EXTRA_LDLIBS?=-lGL

all: cube

clean:
	rm -f *.o cube

.PHONY: all clean

cube: ../../common/shader_utils.o
