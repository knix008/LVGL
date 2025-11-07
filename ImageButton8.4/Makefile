# Makefile for LVGL 8.4 Button Demo Application

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2
CFLAGS += -I. -Ilvgl
CFLAGS += -DLV_CONF_INCLUDE_SIMPLE

# SDL2 flags
SDL2_CFLAGS = $(shell pkg-config --cflags sdl2)
SDL2_LIBS = $(shell pkg-config --libs sdl2)

# FreeType flags (for font loading)
FREETYPE_CFLAGS = $(shell pkg-config --cflags freetype2)
FREETYPE_LIBS = $(shell pkg-config --libs freetype2)

# PNG flags (for image decoding)
PNG_LIBS = $(shell pkg-config --libs libpng 2>/dev/null || echo "-lpng16")

# JPEG flags (for JPG image decoding)
JPEG_LIBS = $(shell pkg-config --libs libjpeg 2>/dev/null || echo "-ljpeg")

# Combine all flags
CFLAGS += $(SDL2_CFLAGS) $(FREETYPE_CFLAGS)
LDFLAGS = $(SDL2_LIBS) $(FREETYPE_LIBS) $(PNG_LIBS) $(JPEG_LIBS)

# Source files
SOURCES = main.c
LVGL_LIB = lvgl/lib/liblvgl.a

# Output executable
TARGET = button

# Default target
all: $(TARGET)

# Link the executable
$(TARGET): $(SOURCES:.c=.o) $(LVGL_LIB)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Compile source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Check if LVGL library exists
$(LVGL_LIB):
	@echo "Error: LVGL library not found at $(LVGL_LIB)"
	@echo "Please run './setup.sh' first"
	@exit 1

# Clean build artifacts
clean:
	rm -f $(SOURCES:.c=.o) $(TARGET)
	@echo "Clean complete"

# Clean everything including LVGL
distclean: clean
	rm -rf lvgl/build lvgl/lib
	@echo "Distclean complete"

# Run the application
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean distclean run
