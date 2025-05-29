CCFLAGS= -g `pkg-config --cflags lilv-0` `pkg-config --cflags suil-0` `pkg-config --cflags gtk+-2.0` `pkg-config --cflags serd-0`

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	CCFLAGS += -D LINUX
endif
ifeq ($(UNAME_S),Darwin)
	CCFLAGS+= -D MACOS
endif

LDFLAGS= `pkg-config --libs lilv-0` `pkg-config --libs gtk+-2.0` `pkg-config --libs suil-0` `pkg-config --libs serd-0`

SOURCES=$(wildcard src/*.c)
ifeq ($(UNAME_S),Darwin)
	SOURCES+=$(wildcard src/*.m)
endif
	OBJECTS=$(filter %.o,$(SOURCES:.c=.o))

ifeq ($(UNAME_S),Darwin)
OBJECTS+=$(filter %.o,$(SOURCES:.m=.o))
endif

EXECUTABLE= testUI
TEST_EXEC = UnitTests
TEST_CPP = decompiler

all: $(SOURCES) $(EXECUTABLE)

%.o: %.c
	$(CC) -c $(CCFLAGS) $< -o $@

ifeq ($(UNAME_S),Darwin)
%.o: %.m
	$(CC) -c $(CCFLAGS) $< -o $@
endif

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(CCFLAGS) $(LDFLAGS) -o $(EXECUTABLE)

clean:
	rm -f $(OBJECTS)
	rm -f $(EXECUTABLE)

.PHONY: clean all