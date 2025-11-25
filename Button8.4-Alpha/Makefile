# Makefile using LVGL 8.4

# Compiler and flags
CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -O2 -g
LDFLAGS = -lm -lpthread

# Directories
SRC_DIR = .
LVGL_DIR = lvgl
LIB_DIR = $(LVGL_DIR)/lib

# Source files
MAIN_SRC = main.c
LVGL_LIB = $(LIB_DIR)/liblvgl.a

# Target executable
TARGET = test

# SDL2 flags (adjust paths as needed for your system)
SDL2_CFLAGS = $(shell pkg-config --cflags sdl2)
SDL2_LIBS = $(shell pkg-config --libs sdl2)

# LVGL include paths
LVGL_INCLUDES = -I$(LVGL_DIR)/src -I$(SRC_DIR)

# All includes
INCLUDES = $(LVGL_INCLUDES) $(SDL2_CFLAGS)

# All libraries
LIBS = $(LVGL_LIB) $(SDL2_LIBS) -lfreetype -lpng16 $(LDFLAGS)

# Compile flags
ALL_CFLAGS = $(CFLAGS) $(INCLUDES) -DLV_CONF_INCLUDE_SIMPLE

# Default target
all: $(TARGET)

# Build LVGL library if it doesn't exist
$(LVGL_LIB):
	@echo "LVGL library already exists, skipping build..."

# Build the main executable
$(TARGET): $(MAIN_SRC) $(LVGL_LIB)
	@echo "Building the system..."
	$(CC) $(ALL_CFLAGS) -o $@ $(MAIN_SRC) $(LIBS)

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	rm -rf $(TARGET)

# Clean everything including LVGL
distclean: clean
	@echo "Cleaning everything..."
	rm -f $(LVGL_LIB)

# Install (optional)
install: $(TARGET)
	@echo "Installing the system..."
	cp $(TARGET) /usr/local/bin/

# Run the application
run: $(TARGET)
	@echo "Running the system..."
	$(TARGET)

# Debug build
debug: CFLAGS += -DDEBUG -g3
debug: $(TARGET)

# Release build
release: CFLAGS += -O3 -DNDEBUG
release: $(TARGET)

# Show help
help:
	@echo "Available targets:"
	@echo "  all      - Build the appliation (default)"
	@echo "  clean    - Remove build artifacts"
	@echo "  distclean- Remove all build artifacts including LVGL"
	@echo "  install  - Install the executable to /usr/local/bin"
	@echo "  run      - Build and run the application"
	@echo "  debug    - Build with debug symbols"
	@echo "  release  - Build optimized release version"
	@echo "  help     - Show this help message"

# Phony targets
.PHONY: all clean distclean install run debug release help

# Dependencies
$(TARGET): $(MAIN_SRC) $(LVGL_LIB)
