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

# Detect CPU architecture and OS for ONNX Runtime
UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

# Determine ONNX Runtime directory based on architecture and OS
ifeq ($(UNAME_S),Linux)
    ifeq ($(UNAME_M),x86_64)
        ONNX_DIR = onnxruntime-linux-x64-1.16.3
    else ifeq ($(UNAME_M),aarch64)
        ONNX_DIR = onnxruntime-linux-aarch64-1.16.3
    else ifeq ($(UNAME_M),arm64)
        ONNX_DIR = onnxruntime-linux-aarch64-1.16.3
    else
        $(error Unsupported Linux architecture: $(UNAME_M))
    endif
else ifeq ($(UNAME_S),Darwin)
    ifeq ($(UNAME_M),x86_64)
        ONNX_DIR = onnxruntime-osx-x64-1.16.3
    else ifeq ($(UNAME_M),arm64)
        ONNX_DIR = onnxruntime-osx-arm64-1.16.3
    else
        $(error Unsupported macOS architecture: $(UNAME_M))
    endif
else
    $(error Unsupported operating system: $(UNAME_S))
endif

# ONNX Runtime configuration
ONNX_CFLAGS = -I./$(ONNX_DIR)/include
ONNX_LIBS = -L./$(ONNX_DIR)/lib -lonnxruntime -Wl,-rpath,./$(ONNX_DIR)/lib

# Print detected configuration
$(info Detected OS: $(UNAME_S))
$(info Detected Architecture: $(UNAME_M))
$(info Using ONNX Runtime: $(ONNX_DIR))

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
	@echo "Clean complete"

cleanall: clean
	@echo "Cleaning all generated files..."
	rm -f $(OBJECTS) $(TARGET) lvgl
	@echo "All clean complete"

distclean: clean
	@echo "Cleaning LVGL build..."
	rm -rf lvgl/build lvgl/lib
	@echo "Deep clean complete"

run: $(TARGET)
	@echo "Running $(TARGET)..."
	./$(TARGET)

.PHONY: all clean distclean run
