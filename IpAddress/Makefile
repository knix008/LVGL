# Makefile for IP Address Input GUI using LVGL

# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -O2 -g
LDFLAGS = 

# Directories
LVGL_DIR = lvgl
SRC_DIR = .
BUILD_DIR = build

# Source files
SOURCES = main.c ip_address_gui.c ip_address.c

# Object files
OBJECTS = $(SOURCES:%.c=$(BUILD_DIR)/%.o)

# LVGL library
LVGL_LIB = $(LVGL_DIR)/build/liblvgl.a

# Target executable
TARGET = ip_address

# Include directories
INCLUDES = -I$(LVGL_DIR) -I$(SRC_DIR)

# Libraries
LIBS = -lm -lpthread

# SDL2 for simulation (if available)
ifeq ($(shell pkg-config --exists sdl2 && echo yes), yes)
    SDL_CFLAGS = $(shell pkg-config --cflags sdl2)
    SDL_LIBS = $(shell pkg-config --libs sdl2)
    CFLAGS += -DUSE_SDL=1 $(SDL_CFLAGS)
    LIBS += $(SDL_LIBS)
endif

# FreeType library (if available)
ifeq ($(shell pkg-config --exists freetype2 && echo yes), yes)
    FREETYPE_CFLAGS = $(shell pkg-config --cflags freetype2)
    FREETYPE_LIBS = $(shell pkg-config --libs freetype2)
    CFLAGS += $(FREETYPE_CFLAGS)
    LIBS += $(FREETYPE_LIBS)
endif

# Default target
all: $(TARGET)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build target
$(TARGET): $(BUILD_DIR) $(OBJECTS) $(LVGL_LIB)
	$(CC) $(OBJECTS) $(LVGL_LIB) -o $@ $(LDFLAGS) $(LIBS)

# Build application objects
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# LVGL library check
$(LVGL_LIB):
	@echo "Error: LVGL library not found. Please build LVGL first."
	@exit 1

# Clean build files
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Install dependencies (Ubuntu/Debian)
install-deps:
	sudo apt-get update
	sudo apt-get install -y build-essential libsdl2-dev pkg-config

# Run the application
run: $(TARGET)
	./$(TARGET)

# Debug build
debug: CFLAGS += -DDEBUG -g3
debug: $(TARGET)

# Release build
release: CFLAGS += -DNDEBUG -O3
release: $(TARGET)

# Help target
help:
	@echo "Available targets:"
	@echo "  all          - Build the application (default)"
	@echo "  clean        - Remove build files"
	@echo "  run          - Build and run the application"
	@echo "  debug        - Build with debug flags"
	@echo "  release      - Build with release flags"
	@echo "  install-deps - Install build dependencies"
	@echo "  help         - Show this help"

.PHONY: all clean run debug release install-deps help
