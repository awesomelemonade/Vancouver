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

PYBIND11_LIBRARY := $(shell python3 -m pybind11 --includes)
PYBIND11_SUFFIX := $(shell python3-config --extension-suffix)

main: main.cpp
#g++ -O3 -Wall -shared -std=c++11 -fPIC $(PYBIND11_LIBRARY) main.cpp -o main$(PYBIND11_SUFFIX)
	$(CXX) -Wall -shared -O3 --fast-math -g -I./include -fPIC $(PYBIND11_LIBRARY) $< $(LFLAGS) -o $@$(PYBIND11_SUFFIX) $(LEAP_LIBRARY)
clean:
	rm main$(PYBIND11_SUFFIX)
