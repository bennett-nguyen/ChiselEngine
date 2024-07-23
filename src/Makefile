MAKEFLAGS += --no-builtin-rules

# Compiler Options
CC        := gcc
CXX       := g++

# Include and source code directories 
INCDIRS   := include
CODEDIRS  := loader player .

# binary and build directories
BINDIR    := bin
BUILDDIR  := build
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
OBJDIRS   := $(foreach ENTRY, $(CODEDIRS), $(BUILDDIR)/$(ENTRY))

# default make rule
.PHONY: all create_build_dirs
all: create_build_dirs | $(BINDIR)/$(TARGET)

create_build_dirs:
	@mkdir -p $(BINDIR)
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(OBJDIRS)

$(BINDIR)/$(TARGET): $(OBJECTS)
	$(CXX) -o $@ $(foreach ENTRY, $^, $(BUILDDIR)/$(ENTRY)) $(LIB)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $(BUILDDIR)/$@ $< $(LIB)

.PHONY: run
run:
	@$(BINDIR)/$(TARGET)

.PHONY: clean
clean:
	@rm -rfv $(BINDIR) $(BUILDDIR)

-include $(DEPFILES)
