# Makefile for LVGL Image Button Application
# Builds an image button demonstration with PNG, JPG, GIF, BMP support

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2 -I. -Ilvgl -Ilvgl/src/drivers/sdl -Ilvgl/src/draw/sdl
LDFLAGS = -Llvgl/lib

# Libraries
LIBS = -llvgl -lm -lpthread
SDL_LIBS = $(shell pkg-config --libs sdl2)
FREETYPE_LIBS = $(shell pkg-config --libs freetype2)

# Include flags
SDL_CFLAGS = $(shell pkg-config --cflags sdl2)
FREETYPE_CFLAGS = $(shell pkg-config --cflags freetype2)

# Source and object files
SRC = main.c image_button_app.c
OBJ = $(SRC:.c=.o)

# No need to compile SDL drivers separately - they're already in liblvgl.a

TARGET = button

# Default target
all: check_lvgl $(TARGET)
	@echo ""
	@echo "========================================="
	@echo "✓ Build successful!"
	@echo "========================================="
	@echo "Run the application with:"
	@echo "  ./$(TARGET)"
	@echo ""

# Check if LVGL library exists
check_lvgl:
	@if [ ! -f lvgl/lib/liblvgl.a ]; then \
		echo ""; \
		echo "========================================="; \
		echo "ERROR: LVGL library not found!"; \
		echo "========================================="; \
		echo "Please run the setup script first:"; \
		echo "  bash setup.sh"; \
		echo ""; \
		exit 1; \
	fi

# Link the final executable
$(TARGET): $(OBJ)
	@echo "Linking $@..."
	$(CC) $(OBJ) $(LDFLAGS) $(LIBS) $(SDL_LIBS) $(FREETYPE_LIBS) -o $@

# Compile source files
%.o: %.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) $(SDL_CFLAGS) $(FREETYPE_CFLAGS) -DLV_CONF_INCLUDE_SIMPLE -c $< -o $@

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	rm -f $(OBJ) $(TARGET)
	@echo "✓ Clean complete"

# Clean everything including LVGL
distclean: clean
	@echo "Cleaning LVGL build files..."
	rm -rf lvgl/build lvgl/lib
	@echo "✓ Deep clean complete"

# Run the application
run: all
	@echo "Running $(TARGET)..."
	@./$(TARGET)

# Install dependencies (Ubuntu/Debian)
install-deps:
	@echo "Installing dependencies..."
	sudo apt-get update
	sudo apt-get install -y build-essential git libsdl2-dev libfreetype6-dev pkg-config python3-pil
	@echo "✓ Dependencies installed"

# Help target
help:
	@echo "Available targets:"
	@echo "  all           - Build the application (default)"
	@echo "  clean         - Remove build artifacts"
	@echo "  distclean     - Remove all build files including LVGL"
	@echo "  run           - Build and run the application"
	@echo "  install-deps  - Install required system dependencies"
	@echo "  help          - Show this help message"
	@echo ""
	@echo "Before building:"
	@echo "  1. Run 'bash setup.sh' to set up LVGL"
	@echo "  2. Ensure sample images are in assets/images/"
	@echo ""

# Phony targets
.PHONY: all clean distclean run install-deps help check_lvgl