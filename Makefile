# =========================
# Compiler
# =========================
CXX = g++

CXXFLAGS := -std=c++17 -Wall -Wextra -g -march=x86-64 -mtune=generic
LFLAGS =

# =========================
# SDL flags
# =========================
SDL_CFLAGS := $(shell sdl-config --cflags)
SDL_LIBS   := $(shell sdl-config --libs)

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
INCLUDES := -I$(INCLUDE) $(SDL_CFLAGS)

LIBS := \
	-lncurses \
	-lSDL_mixer \
	$(SDL_LIBS) \
	-lpthread \


# =========================
# Sources / Objects
# =========================
SOURCES := $(wildcard $(SRC)/*.cpp)
OBJECTS := $(patsubst $(SRC)/%.cpp,$(BUILD)/%.o,$(SOURCES))
DEPS    := $(OBJECTS:.o=.d)

TARGET := $(OUTPUT)/main

# =========================
# Rules
# =========================
all: $(BUILD) $(OUTPUT) $(TARGET)
	@echo Build complete

$(BUILD):
	mkdir -p $(BUILD)

$(OUTPUT):
	mkdir -p $(OUTPUT)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LFLAGS) $(LIBS)

$(BUILD)/%.o: $(SRC)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -c $< -o $@

-include $(DEPS)

.PHONY: clean run

clean:
	rm -f $(BUILD)/*.o $(BUILD)/*.d $(TARGET)
	@echo Clean complete

run: all
	./$(TARGET)
