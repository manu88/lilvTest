CC := clang++

CPPFLAGS= -g `pkg-config --cflags lilv-0` `pkg-config --cflags suil-0` `pkg-config --cflags gtk+-2.0` `pkg-config --cflags serd-0`
LDFLAGS= `pkg-config --libs lilv-0` `pkg-config --libs gtk+-2.0` `pkg-config --libs suil-0` `pkg-config --libs serd-0`

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	CPPFLAGS+= -D LINUX
endif
ifeq ($(UNAME_S),Darwin)
	CPPFLAGS+= -D MACOS
endif

SRCS=src/main.c src/plugins.c

BIN_NAME=testUI
OBJS=$(subst .c,.o,$(SRCS))
ifeq ($(UNAME_S),Darwin)
	OBJS+=$(subst .m,.o,$(SRCS))
endif

all: $(BIN_NAME) 

$(BIN_NAME): $(OBJS)
	CC $(OBJS) $(LDFLAGS) -o $(BIN_NAME) 

clean:
	rm -f $(OBJS)
	rm -rf $(BIN_NAME)

.PHONY: clean all

