OS := $(shell uname)
ARCH := $(shell uname -m)

ifeq ($(OS), Linux)
  ifeq ($(ARCH), x86_64)
    LEAP_LIBRARY := ./lib/x64/libLeap.so -Wl,-rpath,./lib/x64
  else
    LEAP_LIBRARY := ./lib/x86/libLeap.so -Wl,-rpath,./lib/x86
  endif
  LFLAGS = -lGL -lGLEW -lSDL2
endif

Combined: combined.cpp
	$(CXX) -Wall -O3 --fast-math -g -I./include $< $(LFLAGS) -o $@ $(LEAP_LIBRARY)

clean:
	rm Combined
