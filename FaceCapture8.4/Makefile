# Makefile for Webcam Capture Application with LVGL
# Target: Linux with SDL2 and V4L2

# Application name
TARGET = camera

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2 -I. -Ilvgl
LDFLAGS = -lpthread -lm -ljpeg

# SDL2 configuration
SDL_CFLAGS = $(shell pkg-config --cflags sdl2)
SDL_LIBS = $(shell pkg-config --libs sdl2 SDL2_mixer)

# FreeType configuration
FREETYPE_CFLAGS = $(shell pkg-config --cflags freetype2)
FREETYPE_LIBS = $(shell pkg-config --libs freetype2)

# FFmpeg configuration
FFMPEG_CFLAGS = $(shell pkg-config --cflags libavformat libavcodec libavdevice libswscale libavutil)
FFMPEG_LIBS = $(shell pkg-config --libs libavformat libavcodec libavdevice libswscale libavutil)

# ONNX Runtime configuration
ONNX_CFLAGS = -I./onnxruntime-linux-x64-1.16.3/include
ONNX_LIBS = -L./onnxruntime-linux-x64-1.16.3/lib -lonnxruntime -Wl,-rpath,./onnxruntime-linux-x64-1.16.3/lib

# Source files
SOURCES = main.c camera.c gui.c face_detection.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# LVGL library
LVGL_LIB = lvgl/lib/liblvgl.a

# Build rules
all: $(TARGET)

$(TARGET): $(OBJECTS) $(LVGL_LIB)
	@echo "Linking $(TARGET)..."
	$(CC) $(OBJECTS) $(LVGL_LIB) $(SDL_LIBS) $(FREETYPE_LIBS) $(FFMPEG_LIBS) $(ONNX_LIBS) $(LDFLAGS) -o $(TARGET)
	@echo "Build complete: $(TARGET)"

%.o: %.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) $(SDL_CFLAGS) $(FREETYPE_CFLAGS) $(FFMPEG_CFLAGS) $(ONNX_CFLAGS) -c $< -o $@

$(LVGL_LIB):
	@echo "Error: LVGL library not found at $(LVGL_LIB)"
	@echo "Please run ./setup.sh first to build LVGL"
	@exit 1

clean:
	@echo "Cleaning build files..."
	rm -f $(OBJECTS) $(TARGET)
	make -C test clean
	@echo "Clean complete"

cleanall: clean
	@echo "Cleaning all generated files..."
	rm -f $(OBJECTS) $(TARGET) lvgl
	make -C test clean
	@echo "All clean complete"

distclean: clean
	@echo "Cleaning LVGL build..."
	rm -rf lvgl/build lvgl/lib
	@echo "Deep clean complete"

run: $(TARGET)
	@echo "Running $(TARGET)..."
	./$(TARGET)

.PHONY: all clean distclean run
