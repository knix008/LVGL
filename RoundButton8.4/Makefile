# Makefile for Round Button LVGL Application
# Target: Linux with SDL2

# Compiler and flags
CC = gcc
CFLAGS = -O2 -Wall -Wextra -Wno-unused-parameter -Wno-unused-function
CFLAGS += -I. -I./lvgl
CFLAGS += -DLV_CONF_INCLUDE_SIMPLE

# SDL2 flags
SDL_CFLAGS := $(shell pkg-config --cflags sdl2)
SDL_LDFLAGS := $(shell pkg-config --libs sdl2)

# FreeType flags (if using TrueType fonts)
FREETYPE_CFLAGS := $(shell pkg-config --cflags freetype2)
FREETYPE_LDFLAGS := $(shell pkg-config --libs freetype2)

# Combine flags
CFLAGS += $(SDL_CFLAGS) $(FREETYPE_CFLAGS)
LDFLAGS = $(SDL_LDFLAGS) $(FREETYPE_LDFLAGS) -lm -lpthread

# Target executable
TARGET = button

# Source files
MAIN_SRC = main.c

# LVGL source files
LVGL_DIR = lvgl
LVGL_SRC = $(shell find $(LVGL_DIR)/src -name "*.c")
LVGL_DEMOS_SRC = $(shell find $(LVGL_DIR)/demos -name "*.c" 2>/dev/null || true)

# All source files
SOURCES = $(MAIN_SRC) $(LVGL_SRC) $(LVGL_DEMOS_SRC)

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Build rules
all: $(TARGET)

$(TARGET): $(OBJECTS)
	@echo "Linking $(TARGET)..."
	@$(CC) -o $@ $^ $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

%.o: %.c
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Cleaning build files..."
	@rm -f $(OBJECTS) $(TARGET)
	@echo "Clean complete"

run: $(TARGET)
	@echo "Running $(TARGET)..."
	@./$(TARGET)

rebuild: clean all

help:
	@echo "Makefile for Round Button LVGL Application"
	@echo ""
	@echo "Targets:"
	@echo "  all      - Build the application (default)"
	@echo "  clean    - Remove build files"
	@echo "  run      - Build and run the application"
	@echo "  rebuild  - Clean and build"
	@echo "  help     - Show this help message"

.PHONY: all clean run rebuild help
