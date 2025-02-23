#
# 'make'        build executable file 'main'
# 'make clean'  removes all .o and executable files
#

# define the Cpp compiler to use
CXX = g++

# define any compile-time flags
CXXFLAGS := -std=c++17 -Wall -Wextra -g -D_REENTRANT -I/usr/include/opencv4
# -I/usr/include

# define output directory
OUTPUT := output

# define source directory
SRC := src

# define include directory
INCLUDE := include

# define lib directory
LIB := lib

# 检查操作系统类型
ifeq ($(OS),Windows_NT)
    LIBRARIES := -lws2_32 -lmgl
else
    LIBRARIES := -lmgl 
endif

ifeq ($(OS),Windows_NT)
	MAIN := main.exe
	SOURCEDIRS := $(SRC)
	INCLUDEDIRS := $(INCLUDE)
	LIBDIRS := $(LIB)
	FIXPATH = $(subst \,/,$1)
	RM := del /q /f
	MD := mkdir
	RUN_CMD := cmd /c
else
	MAIN := main
	SOURCEDIRS := $(shell find $(SRC) -type d)
	INCLUDEDIRS := $(shell find $(INCLUDE) -type d)
	LIBDIRS := $(shell find $(LIB) -type d)
	FIXPATH = $1
	RM := rm -f
	MD := mkdir -p
	RUN_CMD := ./  
endif

# define any directories containing header files other than /usr/include
INCLUDES := $(patsubst %,-I%, $(INCLUDEDIRS:%/=%))

# define the C libs
LIBS := $(patsubst %,-L%, $(LIBDIRS:%/=%))

# define the C source files
SOURCES := $(wildcard $(patsubst %,%/*.cpp, $(SOURCEDIRS)))

# define the C object files 
OBJECTS := $(SOURCES:.cpp=.o)

# output executable path
OUTPUTMAIN := $(call FIXPATH,$(OUTPUT)/$(MAIN))

all: $(OUTPUT) $(MAIN)
	@echo Executing 'all' complete!

$(OUTPUT):
	$(MD) $(OUTPUT)

$(MAIN): $(OBJECTS) 
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(OUTPUTMAIN) $(OBJECTS) $(LFLAGS) $(LIBS) $(LIBRARIES)

# this is a suffix replacement rule for building .o's from .cpp's
.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

.PHONY: clean
clean:
	$(RM) $(OUTPUTMAIN)
	$(RM) $(call FIXPATH,$(OBJECTS))
	@echo Cleanup complete!

run: all
	$(RUN_CMD) $(OUTPUTMAIN)
	@echo Executing 'run' complete!
