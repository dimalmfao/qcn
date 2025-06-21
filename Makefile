.PHONY: all build clean

all: build

build:
	cmake -B build
	cmake --build build

clean:
	rm -rf build
