#
# make        : build executable
# make clean  : remove build and output files
#

CXX = gcc

# CXXFLAGS := -std=c++20 -Wall -Wextra -g -march=x86-64 -mtune=generic  -fcoroutines
CXXFLAGS := -Wall -ggdb -W -O

LFLAGS =

ifeq ($(OS),Windows_NT)
	LFLAGS += -static-libstdc++ -static-libgcc
	MAIN := main.exe
	RM := del /q /f
	MD := mkdir
	FIXPATH = $(subst /,\,$1)
else
	MAIN := main
	RM := rm -f
	MD := mkdir -p
	FIXPATH = $1
endif

# =========================
# Directories
# =========================
SRC     := src
INCLUDE := include 
LIB     := lib
BUILD   := build
OUTPUT  := output

# =========================
# Include / Lib flags
# =========================
INCLUDES := -I$(INCLUDE) -I/usr/include/tirpc
LIBS := -L$(LIB) -lpthread -ltirpc

# =========================
# Sources / Objects
# =========================
SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c,$(BUILD)/%.o,$(SOURCES))
DEPS    := $(OBJECTS:.o=.d)

TARGET := $(call FIXPATH,$(OUTPUT)/$(MAIN))

# =========================
# Rules
# =========================
all: $(BUILD) $(OUTPUT) $(TARGET)
	@echo Build complete

$(BUILD):
	$(MD) $(BUILD)

$(OUTPUT):
	$(MD) $(OUTPUT)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LFLAGS) $(LIBS)

$(BUILD)/%.o: $(SRC)/%.c
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -c $< -o $@

-include $(DEPS)

.PHONY: clean run

clean:
	$(RM) $(call FIXPATH,$(BUILD)/*.o)
	$(RM) $(call FIXPATH,$(BUILD)/*.d)
	$(RM) $(call FIXPATH,$(TARGET))
	@echo Clean complete

run: all
	./$(TARGET)