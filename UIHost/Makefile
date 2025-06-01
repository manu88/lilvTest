CXXFLAGS= -g -stdlib=libc++ `pkg-config --cflags lilv-0` `pkg-config --cflags suil-0` `pkg-config --cflags gtk+-2.0` `pkg-config --cflags serd-0`
CXXFLAGS += -Iinclude/
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	CXXFLAGS += -D LINUX
endif
ifeq ($(UNAME_S),Darwin)
	CXXFLAGS+= -D MACOS
endif

LDFLAGS= `pkg-config --libs lilv-0` `pkg-config --libs gtk+-2.0` `pkg-config --libs suil-0` `pkg-config --libs serd-0`

SOURCES=$(wildcard src/*.cpp)
ifeq ($(UNAME_S),Darwin)
	SOURCES+=$(wildcard src/*.m)
endif
	OBJECTS=$(filter %.o,$(SOURCES:.cpp=.o))

ifeq ($(UNAME_S),Darwin)
OBJECTS+=$(filter %.o,$(SOURCES:.m=.o))
endif

EXECUTABLE= UIHost

all: $(SOURCES) $(EXECUTABLE)

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

ifeq ($(UNAME_S),Darwin)
%.o: %.m
	$(CXX) -c $(CXXFLAGS) $< -o $@
endif

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) $(CXXFLAGS) $(LDFLAGS) -o $(EXECUTABLE)

clean:
	rm -f $(OBJECTS)
	rm -f $(EXECUTABLE)

.PHONY: clean all