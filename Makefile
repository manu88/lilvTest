#CXX := clang++

CPPFLAGS= -g -std=c++17
#CPPFLAGS+=-I/opt/homebrew/include/ -Isrc/

LDFLAGS=
#-L/opt/homebrew/lib/ -lSDL2 -lSDL2_ttf -lncurses
LDLIBS=

SRCS=src/main.cpp




BIN_NAME=testLV2
OBJS=$(subst .cpp,.o,$(SRCS))


all: $(BIN_NAME) 

$(BIN_NAME): $(OBJS)
	g++ $(LDFLAGS) -o $(BIN_NAME) $(OBJS) $(LDLIBS)


clean:
	rm -f $(OBJS)
	rm -rf $(BIN_NAME)

.PHONY: clean all

