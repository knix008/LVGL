# MiniGUI ChunJiIn Korean Input Application Makefile

# Compiler and flags
CC = gcc
CFLAGS = -Wall -g -O2 -Wno-unused-parameter
LDFLAGS = -Wl,--as-needed -Wl,-rpath,'$$ORIGIN/install/lib'

# Local MiniGUI installation paths
MINIGUI_CFLAGS = -I./install/include
MINIGUI_LIBS = -L./install/lib -lminigui_sa -ljpeg -lpng16 -lz -ldl -lharfbuzz -lfreetype -lm -lpthread -linput -ldrm -ludev

# Target executable
TARGET = chunjiin

# Source files (modular structure)
INPUT_SOURCES = input/input.c input/chunjiin.c input/english.c input/number.c
GUI_SOURCES = gui/gui.c
MAIN_SOURCES = main.c

SOURCES = $(MAIN_SOURCES) $(INPUT_SOURCES) $(GUI_SOURCES)
HEADERS = input/chunjiin.h input/input.h input/english.h input/number.h gui/gui.h

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Font directory
FONT_DIR = ./install/share/fonts
ASSETS_DIR = assets

# Default target
all: copy-fonts $(TARGET)
	@echo ""
	@echo "✓ Build complete!"
	@echo "Run with: ./run.sh or make run"

# Copy Korean fonts to MiniGUI font directory
copy-fonts:
	@echo "Copying Korean fonts to MiniGUI font directory..."
	@mkdir -p $(FONT_DIR)
	@cp $(ASSETS_DIR)/*.ttf $(FONT_DIR)/ 2>/dev/null || echo "Warning: Could not copy fonts (assets directory may not exist)"
	@echo "✓ Korean fonts copied to $(FONT_DIR)/"

# Build the executable
$(TARGET): $(OBJECTS)
	@echo "Linking $(TARGET)..."
	$(CC) $(OBJECTS) -o $(TARGET) $(MINIGUI_LIBS) $(LDFLAGS)

# Compile source files with header dependencies
%.o: %.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) $(MINIGUI_CFLAGS) -c $< -o $@

# Rebuild everything from scratch
rebuild: clean all

# Run the application
run: $(TARGET)
	@echo "Starting ChunJiIn Korean Input Application..."
	@./run.sh

# Clean build files
clean:
	@echo "Cleaning build files..."
	@rm -f $(OBJECTS)
	@rm -f $(TARGET)
	@rm -f $(FONT_DIR)/*.ttf
	@echo "✓ Clean complete"

# Deep clean including MiniGUI installation
distclean: clean
	@echo "Removing MiniGUI installation..."
	@rm -rf ./install
	@rm -rf ./minigui
	@echo "✓ Deep clean complete"

# Check if MiniGUI is installed locally
check-minigui:
	@echo "Checking local MiniGUI installation..."
	@if [ -f "./install/lib/libminigui_sa.so" ]; then \
		echo "✓ MiniGUI found locally"; \
	else \
		echo "✗ MiniGUI not found locally - run ./build.sh to build it"; \
		exit 1; \
	fi

# Install dependencies (run build script)
install-deps:
	@echo "Building MiniGUI locally..."
	@./build.sh

# Show build information
info:
	@echo "ChunJiIn Korean Input Application (Modular Architecture)"
	@echo "========================================================"
	@echo "Compiler:       $(CC)"
	@echo "CFLAGS:         $(CFLAGS)"
	@echo "Target:         $(TARGET)"
	@echo ""
	@echo "Modules:"
	@echo "  Main:         $(MAIN_SOURCES)"
	@echo "  Input:        $(INPUT_SOURCES)"
	@echo "  GUI:          $(GUI_SOURCES)"
	@echo ""
	@echo "Font directory: $(FONT_DIR)"
	@echo ""
	@echo "MiniGUI Status:"
	@if [ -f "./install/lib/libminigui_sa.so" ]; then \
		echo "  ✓ MiniGUI installed locally"; \
	else \
		echo "  ✗ MiniGUI not installed - run 'make install-deps'"; \
	fi

# Help target
help:
	@echo "ChunJiIn Korean Input Application - Makefile Help"
	@echo "=================================================="
	@echo ""
	@echo "Available targets:"
	@echo "  all           - Build ChunJiIn application (default)"
	@echo "  rebuild       - Clean and rebuild everything"
	@echo "  run           - Build and run the application"
	@echo "  clean         - Remove build files and fonts"
	@echo "  distclean     - Remove all build files and MiniGUI installation"
	@echo "  check-minigui - Check if MiniGUI is installed locally"
	@echo "  install-deps  - Download and build MiniGUI locally"
	@echo "  info          - Show build configuration information"
	@echo "  help          - Show this help message"
	@echo ""
	@echo "Quick Start:"
	@echo "  1. make install-deps  # Download and build MiniGUI (first time only)"
	@echo "  2. make               # Build the application"
	@echo "  3. make run           # Run the application"
	@echo ""
	@echo "Application Features:"
	@echo "  - ChunJiIn Korean input method (천지인 입력기)"
	@echo "  - Multi-line text display with auto-wrap"
	@echo "  - Vertical and horizontal scrollbars"
	@echo "  - Center-aligned window on startup"

.PHONY: all rebuild run clean distclean check-minigui install-deps info help copy-fonts