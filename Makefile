#CXX := clang++

CPPFLAGS= -g -std=c++17
CPPFLAGS+=-I/usr/include/lilv-0/

LDFLAGS= -llilv-0
# liblilv-0 is in /usr/lib/aarch64-linux-gnu/ 

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

