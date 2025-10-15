# Makefile for Korean QWERTY Input System on MiniGUI

# Program name
TARGET = qwerty

# Source files
SOURCES = main.c qwerty-korean.c
OBJECTS = $(SOURCES:.c=.o)

# Compiler and flags
CC = gcc
CFLAGS = -Wall -O2 -g
LDFLAGS =

# MiniGUI configuration
MINIGUI_CFLAGS = $(shell pkg-config --cflags minigui)
MINIGUI_LIBS = $(shell pkg-config --libs minigui)

# Include paths
INCLUDES = -I. $(MINIGUI_CFLAGS)

# Libraries
LIBS = $(MINIGUI_LIBS) -lm

# Font directory (for installation)
FONT_DIR = /usr/local/share/minigui/fonts
ASSET_DIR = assets

# Default target
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJECTS)
	@echo "Linking $(TARGET)..."
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)
	@echo "Build complete: $(TARGET)"

# Compile source files
%.o: %.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	rm -f $(OBJECTS) $(TARGET)
	@echo "Clean complete"

# Install fonts to MiniGUI font directory
install-fonts:
	@echo "Installing Korean fonts..."
	@if [ ! -d "$(FONT_DIR)" ]; then \
		echo "Creating font directory: $(FONT_DIR)"; \
		sudo mkdir -p $(FONT_DIR); \
	fi
	@if [ -d "$(ASSET_DIR)" ]; then \
		echo "Copying fonts from $(ASSET_DIR)/ to $(FONT_DIR)/"; \
		sudo cp -v $(ASSET_DIR)/*.ttf $(FONT_DIR)/ 2>/dev/null || echo "No TTF fonts found in $(ASSET_DIR)"; \
	else \
		echo "Warning: $(ASSET_DIR) directory not found"; \
	fi
	@echo "Font installation complete"

# Run the program
run: $(TARGET)
	@echo "Running $(TARGET)..."
	./$(TARGET)

# Show MiniGUI configuration
show-config:
	@echo "=== MiniGUI Configuration ==="
	@echo "CFLAGS: $(MINIGUI_CFLAGS)"
	@echo "LIBS: $(MINIGUI_LIBS)"
	@pkg-config --modversion minigui 2>/dev/null || echo "MiniGUI not found via pkg-config"

# Help target
help:
	@echo "Available targets:"
	@echo "  all            - Build the program (default)"
	@echo "  clean          - Remove build artifacts"
	@echo "  run            - Build and run the program"
	@echo "  install-fonts  - Install Korean fonts to MiniGUI font directory"
	@echo "  show-config    - Show MiniGUI configuration"
	@echo "  help           - Show this help message"

# Phony targets
.PHONY: all clean run install-fonts show-config help

# Dependencies
main.o: main.c qwerty.h
qwerty-korean.o: qwerty-korean.c qwerty.h
