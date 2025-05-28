#CXX := clang++

CPPFLAGS= -g -std=c++17 `pkg-config --cflags lilv-0`

UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        CCFLAGS += -D LINUX
    endif
    ifeq ($(UNAME_S),Darwin)
        CPPFLAGS+= -D MACOS
    endif

LDFLAGS= `pkg-config --libs lilv-0` 

SRCS=src/main.cpp

BIN_NAME=testLV2
OBJS=$(subst .cpp,.o,$(SRCS))


all: $(BIN_NAME) 

$(BIN_NAME): $(OBJS)
	g++ $(OBJS) $(LDFLAGS) -o $(BIN_NAME) 


clean:
	rm -f $(OBJS)
	rm -rf $(BIN_NAME)

.PHONY: clean all

