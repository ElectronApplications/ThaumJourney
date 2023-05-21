CXX ?= g++
CXXFLAGS ?= -O2 -std=c++20

CXXFLAGS := ${CXXFLAGS} -pthread
ifeq ($(OS),Windows_NT)
    LIBS := -Llib -mwindows -lmingw32 -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic
else
    LIBS :=
endif

all:
	${CXX} *.cpp -o structure_generator ${LIBS} ${CXXFLAGS}
clean:
	rm -f structure_generator
