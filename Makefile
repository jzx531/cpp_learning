#
# 'make'        build executable file 'main'
# 'make clean'  removes all .o, .d and executable files
#

# define the Cpp compiler to use
CXX = g++

# define any compile-time flags
CXXFLAGS := -std=c++17 -Wall -Wextra -g 

# define linker flags
LFLAGS =

# define directories
OUTPUT  := output
SRC     := src
INCLUDE := include
LIB     := lib
BUILD   := build

# OS-specific settings
ifeq ($(OS),Windows_NT)
SHELL = cmd.exe
MAIN    := main.exe
SOURCEDIRS  := $(SRC)
INCLUDEDIRS := $(INCLUDE)
LIBDIRS     := $(LIB) 
FIXPATH = $(subst /,\,$1)
RM      := del /q /f
MD      := mkdir
else
MAIN    := main
SOURCEDIRS  := $(shell find $(SRC) -type d)
INCLUDEDIRS := $(shell find $(INCLUDE) -type d)
LIBDIRS     := $(shell find $(LIB) -type d)
FIXPATH = $1
RM = rm -f
MD := mkdir -p
endif

# include flags
INCLUDES := $(patsubst %,-I%, $(INCLUDEDIRS:%/=%))

# library flags
# LIBS := $(patsubst %,-L%, $(LIBDIRS:%/=%))
LIBS := -L$(LIB)  -lpdcurses -luser32 -lgdi32
# source files
SOURCES := $(wildcard $(patsubst %,%/*.cpp, $(SOURCEDIRS)))

# object and dependency files in BUILD directory
OBJECTS := $(SOURCES:$(SRC)/%.cpp=$(BUILD)/%.o)
DEPS    := $(OBJECTS:.o=.d)

# final executable path
OUTPUTMAIN := $(call FIXPATH,$(OUTPUT)/$(MAIN))

# default target
all: $(OUTPUT) $(OUTPUTMAIN)
	@echo Executing 'all' complete!

# create output directory
$(OUTPUT):
	$(MD) $(OUTPUT)

# create build directory (needed before compiling objects)
$(BUILD):
	$(MD) $(BUILD)

# link executable
$(OUTPUTMAIN): $(BUILD) $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $(OBJECTS) $(LFLAGS) $(LIBS)

# compile .cpp -> .o in build/ directory
$(BUILD)/%.o: $(SRC)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

# include dependencies
-include $(DEPS)

.PHONY: clean run

clean:
	-$(RM) $(OUTPUTMAIN)
	-$(RM) $(call FIXPATH,$(OBJECTS))
	-$(RM) $(call FIXPATH,$(DEPS))
	@echo Cleanup complete!

run: all
	./$(OUTPUTMAIN)
	@echo Executing 'run: all' complete!