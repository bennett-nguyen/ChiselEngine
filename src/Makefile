MAKEFLAGS += --no-builtin-rules

# Compiler Options
CC        := gcc
CXX       := g++

# Include and source code directories 
INCDIRS             := include util 
INC_DIRS_NO_WARNING := lib/include/imgui lib/include/stb
CODEDIRS            := util lib/imgui .

# binary and build directories
BINDIR     := bin
RELEASEDIR := release
DEBUGDIR   := debug
BUILDDIR   := build
TARGET     := game

# external libraries
LIB        := -lSDL2 -lGLEW -lGL

# Default compilation set to release if invoked with only "make"
CFG ?= release

# compiler config
VERSION       := --std=c++17
DEPFLAGS      := -MP -MMD
FEATURES      := -pedantic-errors
WARNINGS      := -Wall -Wextra -Wconversion -Wsign-conversion
RELEASEFLAGS  := -O2 -DNDEBUG
DEBUGFLAGS    := -g3 -O0
CXXFLAGS      := $(VERSION) $(WARNINGS) $(FEATURES) $(foreach ENTRY, $(INC_DIRS_NO_WARNING), -isystem $(ENTRY)) $(foreach ENTRY, $(INCDIRS), -I$(ENTRY)) $(DEPFLAGS)

ifeq ($(CFG), debug)
	CXXFLAGS  += $(DEBUGFLAGS)
	TARGETDIR := $(BINDIR)/$(DEBUGDIR)
else
	CXXFLAGS  += $(RELEASEFLAGS)
	TARGETDIR := $(BINDIR)/$(RELEASEDIR)
endif

# source files to compile, object files and dependency files
CXXFILES  := $(foreach ENTRY, $(CODEDIRS), $(wildcard $(ENTRY)/*.cpp))
OBJECTS   := $(patsubst %.cpp, $(BUILDDIR)/%.o, $(CXXFILES))
DEPFILES  := $(patsubst %.cpp, $(BUILDDIR)/%.d, $(CXXFILES))
OBJDIRS   := $(foreach ENTRY, $(CODEDIRS), $(BUILDDIR)/$(ENTRY))

# default make rule
.PHONY: all create_build_dirs
all: create_build_dirs | $(TARGETDIR)/$(TARGET)

create_build_dirs:
	@mkdir -pv $(TARGETDIR)
	@mkdir -pv $(BUILDDIR)
	@mkdir -pv $(OBJDIRS)

$(TARGETDIR)/$(TARGET): $(OBJECTS)
	$(CXX) -o $@ $^ $(LIB)

$(BUILDDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(LIB)

.PHONY: run
run:
	@__NV_PRIME_RENDER_OFFLOAD=1 __GLX_VENDOR_LIBRARY_NAME=nvidia $(TARGETDIR)/$(TARGET)

.PHONY: clean
clean:
	@rm -rfv $(BINDIR) $(BUILDDIR)

-include $(DEPFILES)