# Makefile for Japanese QWERTY Input Method

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2 -I. -Ilvgl -DLV_CONF_INCLUDE_SIMPLE

# Get FreeType flags
FREETYPE_CFLAGS = $(shell pkg-config --cflags freetype2)
FREETYPE_LIBS = $(shell pkg-config --libs freetype2)

# Libraries
LIBS = -lSDL2 -lm -lpthread $(FREETYPE_LIBS)

# LVGL library
LVGL_LIB = lvgl/lib/liblvgl.a

# Source files
SOURCES = main.c japanese_qwerty.c japanese_gui.c
OBJECTS = $(SOURCES:.c=.o)

# Target executable
TARGET = japanese_input

# Default target
all: $(TARGET)

$(TARGET): $(OBJECTS) $(LVGL_LIB)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LVGL_LIB) $(LIBS)
	@echo "Build complete! Run with: ./$(TARGET)"

# Compile source files
%.o: %.c
	$(CC) $(CFLAGS) $(FREETYPE_CFLAGS) -c $< -o $@

# Main dependencies
main.o: main.c japanese_qwerty.h japanese_gui.h
japanese_qwerty.o: japanese_qwerty.c japanese_qwerty.h
japanese_gui.o: japanese_gui.c japanese_gui.h japanese_qwerty.h

# Build LVGL library if needed
$(LVGL_LIB):
	@echo "Building LVGL library..."
	@echo "Error: LVGL library not found. Please run ./setup.sh first"
	@exit 1

# Clean targets
clean:
	rm -f $(OBJECTS) $(TARGET)
	cd tests && make clean
	@echo "Clean complete!"

clean-all: clean
	rm -rf lvgl/build lvgl/lib
	@echo "Deep clean complete!"

# Run target
run: $(TARGET)
	./$(TARGET)

# Phony targets
.PHONY: all clean clean-all run

# Help target
help:
	@echo "Japanese QWERTY Input Method - Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all       - Build the application (default)"
	@echo "  run       - Build and run the application"
	@echo "  clean     - Remove object files and executable"
	@echo "  clean-all - Remove all build files including LVGL"
	@echo "  help      - Show this help message"
	@echo ""
	@echo "Note: Fonts are loaded at runtime from assets/NotoSansCJK.ttc"

