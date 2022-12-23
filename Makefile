# A simple Makefile for compiling small SDL projects

# set the compiler
#CC := clang

# set the compiler flags
#CFLAGS := `sdl2-config --libs --cflags ` -ggdb3 -O0 -Wall -lSDL2_image -lm -std=c++14 -vector
# add header files here

CXX ?= -static g++
CXXFLAGS ?= --std=c++11 `sdl2-config --libs --cflags ` `pkg-config sdl2_image --cflags --libs` `pkg-config sdl2_ttf --cflags --libs` `pkg-config sdl2_mixer --cflags --libs` -O0 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lm -O2


# Compile the tests you wrote for buggy_flip
tree-notation-I.app: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o tree-notation-I.app
	
tree-notation-I.exe: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o tree-notation-I.exe

# Remove automatically generated files
clean :
	rm -rvf *.exe *~ *.out *.dSYM *.stackdump

.PHONY: all clean
