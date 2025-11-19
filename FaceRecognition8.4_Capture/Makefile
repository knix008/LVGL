# Makefile for Face Recognition Application with LVGL
# Target: Linux with SDL2, OpenCV, and FreeType

# Application name
TARGET = face_recognition

# Compiler and flags
CXX = g++
CC = gcc
CXXFLAGS = -Wall -Wextra -O2 -std=c++17 -I./include -I./lvgl/src -I.
CFLAGS = -Wall -Wextra -O2 -I./include -I./lvgl/src -I.
LDFLAGS = -lpthread -lm -ljpeg -lsqlite3

# SDL2 configuration
SDL_CFLAGS = $(shell pkg-config --cflags sdl2)
SDL_LIBS = $(shell pkg-config --libs sdl2) -lSDL2_mixer

# FreeType configuration
FREETYPE_CFLAGS = $(shell pkg-config --cflags freetype2)
FREETYPE_LIBS = $(shell pkg-config --libs freetype2)

# OpenCV configuration
OPENCV_CFLAGS = $(shell pkg-config --cflags opencv4)
OPENCV_LIBS = $(shell pkg-config --libs opencv4)

# Source directories
SRC_DIR = src
INCLUDE_DIR = include
OBJ_DIR = build

# Source and object files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

# LVGL library
LVGL_LIB = lvgl/lib/liblvgl.a

# Build rules
all: $(TARGET)

$(TARGET): $(OBJ_DIR) $(OBJECTS) $(LVGL_LIB)
	@echo "Linking $(TARGET)..."
	$(CXX) $(OBJECTS) $(LVGL_LIB) $(SDL_LIBS) $(FREETYPE_LIBS) $(OPENCV_LIBS) $(LDFLAGS) -o $(TARGET)
	@echo "Build complete: $(TARGET)"

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(SDL_CFLAGS) $(FREETYPE_CFLAGS) $(OPENCV_CFLAGS) -c $< -o $@

$(LVGL_LIB):
	@echo "Error: LVGL library not found at $(LVGL_LIB)"
	@echo "Please run ./setup.sh first to build LVGL"
	@exit 1

clean:
	@echo "Cleaning build files..."
	rm -rf $(OBJ_DIR) $(TARGET)
	@echo "Clean complete"

cleanall: clean
	@echo "Cleaning all generated files..."
	rm -rf $(OBJ_DIR) $(TARGET)
	@echo "All clean complete"

distclean: clean
	@echo "Cleaning LVGL build..."
	rm -rf lvgl/build lvgl/lib
	@echo "Deep clean complete"

run: $(TARGET)
	@echo "Running $(TARGET)..."
	./$(TARGET)

help:
	@echo "Face Recognition Application - Build Instructions"
	@echo ""
	@echo "Available targets:"
	@echo "  make            - Build the application"
	@echo "  make clean      - Remove build files"
	@echo "  make cleanall   - Remove all generated files"
	@echo "  make distclean  - Deep clean (removes LVGL build)"
	@echo "  make run        - Build and run the application"
	@echo "  make help       - Show this help message"
	@echo ""
	@echo "Prerequisites:"
	@echo "  - Run './setup.sh' first to build LVGL and install dependencies"
	@echo "  - OpenCV4 development libraries"
	@echo "  - SDL2 development libraries"
	@echo "  - FreeType2 development libraries"

.PHONY: all clean cleanall distclean run help
