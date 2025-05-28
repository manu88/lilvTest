#CXX := clang++

CPPFLAGS= -g `pkg-config --cflags lilv-0` `pkg-config --cflags gtk+-3.0`
LDFLAGS= `pkg-config --libs lilv-0` `pkg-config --libs gtk+-3.0`

UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        CCFLAGS += -D LINUX
    endif
    ifeq ($(UNAME_S),Darwin)
        CPPFLAGS+= -D MACOS
    endif
	
SRCS=src/main.c src/plugins.c

BIN_NAME=testUI
OBJS=$(subst .c,.o,$(SRCS))


all: $(BIN_NAME) 

$(BIN_NAME): $(OBJS)
	g++ $(OBJS) $(LDFLAGS) -o $(BIN_NAME) 



clean:
	rm -f $(OBJS)
	rm -rf $(BIN_NAME)

.PHONY: clean all

