MAKEFLAGS += --no-builtin-rules

# Compiler Options
CC        := gcc
CXX       := g++

# Include and source code directories 
CODEDIRS                  := util voxel entities .
IMGUI_CODEDIR             := lib/imgui

IMGUI_INCDIRS             := lib/include/imgui
HEADERS_ONLY_LIB_INCDIRCS := lib/include/stb
INCDIRS                   := include voxel util entities $(IMGUI_INCDIRS) $(HEADERS_ONLY_LIB_INCDIRCS)

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
CXXFLAGS      := $(VERSION) $(WARNINGS) $(FEATURES) $(foreach ENTRY, $(INCDIRS), -I$(ENTRY)) $(DEPFLAGS)
IMGUIFLAGS    := $(VERSION) $(foreach ENTRY, $(IMGUI_INCDIRS), -I$(ENTRY)) $(DEPFLAGS) $(RELEASEFLAGS)

ifeq ($(CFG), debug)
	CXXFLAGS   += $(DEBUGFLAGS)
	TARGETDIR  := $(BINDIR)/$(DEBUGDIR)
else
	CXXFLAGS  += $(RELEASEFLAGS)
	TARGETDIR := $(BINDIR)/$(RELEASEDIR)
endif

# source files to compile, object files and dependency files
CXXFILES  := $(foreach ENTRY, $(CODEDIRS), $(wildcard $(ENTRY)/*.cpp))
OBJECTS   := $(patsubst %.cpp, $(BUILDDIR)/%.o, $(CXXFILES))
DEPFILES  := $(patsubst %.cpp, $(BUILDDIR)/%.d, $(CXXFILES))
OBJDIRS   := $(foreach ENTRY, $(CODEDIRS), $(BUILDDIR)/$(ENTRY))

IMGUI_CXXFILES := $(wildcard lib/imgui/*.cpp)
IMGUI_OBJECTS  := $(patsubst %.cpp, $(BUILDDIR)/%.o, $(IMGUI_CXXFILES))
IMGUI_DEPFILES := $(patsubst %.cpp, $(BUILDDIR)/%.d, $(IMGUI_CXXFILES))
IMGUI_OBJDIR   := $(BUILDDIR)/lib/imgui

# default make rule
.PHONY: all create_build_dirs
all: create_build_dirs | $(TARGETDIR)/$(TARGET)

create_build_dirs:
	@mkdir -pv $(TARGETDIR)
	@mkdir -pv $(BUILDDIR)
	@mkdir -pv $(OBJDIRS)
	@mkdir -pv $(IMGUI_OBJDIR)

$(TARGETDIR)/$(TARGET): $(OBJECTS) $(IMGUI_OBJECTS)
	$(CXX) -o $@ $^ $(LIB)

$(BUILDDIR)/util/%.o: util/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(LIB)

$(BUILDDIR)/voxel/%.o: voxel/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(LIB)


$(BUILDDIR)/entities/%.o: entities/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(LIB)

$(BUILDDIR)/./main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(LIB)

$(IMGUI_OBJDIR)/%.o: lib/imgui/%.cpp
	$(CXX) $(IMGUIFLAGS) -c -o $@ $<

.PHONY: run
run:
	@__NV_PRIME_RENDER_OFFLOAD=1 __GLX_VENDOR_LIBRARY_NAME=nvidia $(TARGETDIR)/$(TARGET)

.PHONY: clean
clean:
	@rm -rfv $(BINDIR) $(BUILDDIR)

-include $(DEPFILES)
-include $(IMGUI_DEPFILES)