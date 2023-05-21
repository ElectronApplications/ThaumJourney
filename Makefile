CXX ?= g++
CXXFLAGS ?= -O2 -std=c++20

CXXFLAGS := ${CXXFLAGS} -pthread
ifeq ($(OS),Windows_NT)
    LIBS := -Llib -mwindows -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic
else
    LIBS := -lSDL2 -lSDL2_image -lSDL2_ttf
endif

all:
	${CXX} src/*.cpp -Iinclude/ -o bin/thaumjourney ${LIBS} ${CXXFLAGS}
clean:
	rm -f bin/thaumjourney
