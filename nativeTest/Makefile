#CXX := clang++

CPPFLAGS= -g `pkg-config --cflags lilv-0` `pkg-config --cflags gtk+-3.0`
LDFLAGS= `pkg-config --libs lilv-0` `pkg-config --libs gtk+-3.0`

SRCS=src/main.cpp

BIN_NAME=testUI
OBJS=$(subst .cpp,.o,$(SRCS))


all: $(BIN_NAME) 

$(BIN_NAME): $(OBJS)
	g++ $(OBJS) $(LDFLAGS) -o $(BIN_NAME) 


clean:
	rm -f $(OBJS)
	rm -rf $(BIN_NAME)

.PHONY: clean all

