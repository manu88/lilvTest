#CXX := clang++

CPPFLAGS= -g `pkg-config --cflags lilv-0`
#CPPFLAGS+=-I/usr/local/include/lilv-0/

LDFLAGS= `pkg-config --libs lilv-0`
# liblilv-0 is in /usr/lib/aarch64-linux-gnu/ 

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

