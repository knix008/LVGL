# Japanese Input Method - Makefile
# Builds the Japanese character input application with LVGL

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99
INCLUDES = -I. -Ilvgl $(shell pkg-config --cflags freetype2 sdl2)
LIBS = -Llvgl/lib -llvgl $(shell pkg-config --libs freetype2 sdl2) -lm -lpthread

# Source files
SOURCES = main.c lvgl_init.c gui_app.c japanese_input.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = japanese_input

# Default target
all: $(TARGET)

# Build the main executable
$(TARGET): $(OBJECTS) lvgl/lib/liblvgl.a
	@echo "Linking $@..."
	$(CC) $(OBJECTS) -o $@ $(LIBS)
	@echo "Build complete! Run with: ./$(TARGET)"

# Build object files
%.o: %.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Build LVGL library if it doesn't exist
lvgl/lib/liblvgl.a:
	@echo "Building LVGL library..."
	@mkdir -p lvgl/build lvgl/lib
	@find lvgl/src -name "*.c" | while read src; do \
		obj="lvgl/build/$$(basename $${src%.c}.o)"; \
		if [ ! -f "$$obj" ] || [ "$$src" -nt "$$obj" ]; then \
			echo "  Compiling $$src"; \
			$(CC) $(CFLAGS) -I. -Ilvgl $(shell pkg-config --cflags freetype2) -c "$$src" -o "$$obj"; \
		fi; \
	done
	@echo "  Creating static library..."
	@ar rcs lvgl/lib/liblvgl.a lvgl/build/*.o
	@echo "LVGL library built: lvgl/lib/liblvgl.a"

# Run the application
run: $(TARGET)
	./$(TARGET)

# Clean build artifacts
clean:
	@echo "Cleaning..."
	rm -f $(OBJECTS) $(TARGET)
	rm -rf lvgl/build/*.o
	@echo "Clean complete."

# Deep clean (including LVGL library)
distclean: clean
	rm -f lvgl/lib/liblvgl.a
	@echo "Deep clean complete."

# Install dependencies (Ubuntu/Debian)
install-deps:
	@echo "Installing dependencies..."
	sudo apt-get update
	sudo apt-get install -y build-essential libsdl2-dev libfreetype6-dev pkg-config
	@echo "Dependencies installed."

# Check for required fonts
check-fonts:
	@echo "Checking for Japanese fonts..."
	@if [ -f "assets/NotoSansCJK-Regular.ttf" ]; then \
		echo "✓ NotoSansCJK-Regular.ttf found"; \
	else \
		echo "⚠ NotoSansCJK-Regular.ttf not found - will use built-in fonts"; \
	fi

# Help target
help:
	@echo "Japanese Input Method - Build System"
	@echo ""
	@echo "Targets:"
	@echo "  all          - Build the application (default)"
	@echo "  run          - Build and run the application"
	@echo "  clean        - Remove build artifacts"
	@echo "  distclean    - Remove all build files including LVGL library"
	@echo "  install-deps - Install system dependencies (Ubuntu/Debian)"
	@echo "  check-fonts  - Check for Japanese font files"
	@echo "  help         - Show this help message"
	@echo ""
	@echo "Features:"
	@echo "  • Japanese character input (Hiragana, Katakana)"
	@echo "  • English input (lowercase/uppercase)"
	@echo "  • Numbers and special characters"
	@echo "  • Mobile phone keypad layout (320x640)"
	@echo "  • LVGL-based GUI with SDL2"

.PHONY: all run clean distclean install-deps check-fonts help
