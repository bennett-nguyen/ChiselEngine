# Compiler Options
CC        := gcc
CXX       := g++

# Include and source code directories 
INCDIRS   := ./include
CODEDIRS  := ./loader ./player .

# binary and build directories
BINDIR    := ../bin
BUILDDIR  := ../build
TARGET    := game

# external libraries
LIB       := -lSDL2 -lGLEW -lGL

# compiler config
VERSION   := --std=c++17
DEPFLAGS  := -MP -MMD
WARNINGS  := -Wall -Wextra
CXXFLAGS  := $(VERSION) $(WARNINGS) -g $(foreach ENTRY, $(INCDIRS), -I$(ENTRY)) $(DEPFLAGS)

# source files to compile, object files and dependency files
CXXFILES  := $(foreach ENTRY, $(CODEDIRS), $(wildcard $(ENTRY)/*.cpp))
OBJECTS   := $(patsubst %.cpp, %.o, $(CXXFILES))
DEPFILES  := $(patsubst %.cpp, %.d, $(CXXFILES))

# default make rule
.PHONY: all
all: ${BINDIR}/$(TARGET)

${BINDIR}/$(TARGET): $(OBJECTS)
	mkdir -p ${BINDIR}
	$(CXX) -o $@ $^ $(LIB)

%.o: %.c
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(LIB)

.PHONY: run
run:
	${BINDIR}/${TARGET}

.PHONY: clean
clean:
	rm -rfv $(BINARY) $(OBJECTS) $(DEPFILES)

-include $(DEPFILES)
