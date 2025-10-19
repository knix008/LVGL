# Video Player Makefile

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2 -D_DEFAULT_SOURCE
INCLUDES = -I. -Ilvgl -Ilvgl/src
LIBS = -lm -lpthread

# SDL2, FreeType, and FFmpeg flags
SDL_CFLAGS = $(shell pkg-config --cflags sdl2 2>/dev/null || echo "-I/usr/include/SDL2")
FREETYPE_CFLAGS = $(shell pkg-config --cflags freetype2 2>/dev/null || echo "-I/usr/include/freetype2")
FFMPEG_CFLAGS = $(shell pkg-config --cflags libavcodec libavformat libavutil libswscale libswresample 2>/dev/null || echo "")
SDL_LIBS = $(shell pkg-config --libs sdl2 2>/dev/null || echo "-lSDL2")
FREETYPE_LIBS = $(shell pkg-config --libs freetype2 2>/dev/null || echo "-lfreetype")
FFMPEG_LIBS = $(shell pkg-config --libs libavcodec libavformat libavutil libswscale libswresample 2>/dev/null || echo "-lavcodec -lavformat -lavutil -lswscale -lswresample")

# Combine all flags
ALL_CFLAGS = $(CFLAGS) $(INCLUDES) $(SDL_CFLAGS) $(FREETYPE_CFLAGS) $(FFMPEG_CFLAGS)
ALL_LIBS = $(LIBS) $(SDL_LIBS) $(FREETYPE_LIBS) $(FFMPEG_LIBS)

# Source files
SOURCES = main.c video_player.c korean_fonts.c
OBJECTS = $(SOURCES:.c=.o)

# LVGL library
LVGL_LIB = lvgl/build/liblvgl.a

# Target executable
TARGET = main

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(OBJECTS) $(LVGL_LIB)
	@echo "Linking $(TARGET)..."
	$(CC) $(OBJECTS) $(LVGL_LIB) -o $(TARGET) $(ALL_LIBS)
	@echo "Build complete! Run with: ./$(TARGET)"

# Compile source files
%.o: %.c
	@echo "Compiling $<..."
	$(CC) $(ALL_CFLAGS) -c $< -o $@

# Check if LVGL library exists
$(LVGL_LIB):
	@echo "LVGL library not found. Building LVGL..."
	@if [ ! -f "lvgl/build/liblvgl.a" ]; then \
		echo "Building LVGL library..."; \
		cd lvgl && \
		mkdir -p build && \
		find src -name "*.c" | while read src_file; do \
			obj_file="build/$$(echo $$src_file | sed 's/\.c$$/.o/' | sed 's/src\///')"; \
			obj_dir=$$(dirname "$$obj_file"); \
			mkdir -p "$$obj_dir"; \
			if [ ! -f "$$obj_file" ] || [ "$$src_file" -nt "$$obj_file" ]; then \
				echo "  Compiling $$src_file..."; \
				gcc -Wall -Wextra -std=c11 -O2 -D_DEFAULT_SOURCE \
					-I.. -I. $(SDL_CFLAGS) $(FREETYPE_CFLAGS) \
					-c "$$src_file" -o "$$obj_file"; \
			fi; \
		done && \
		find build -name "*.o" | xargs ar rcs build/liblvgl.a && \
		ranlib build/liblvgl.a && \
		cd ..; \
	fi

# Clean build files
clean:
	@echo "Cleaning build files..."
	rm -f $(OBJECTS) $(TARGET)
	@echo "Clean complete!"

# Clean everything including LVGL
distclean: clean
	@echo "Cleaning LVGL build..."
	rm -rf lvgl/build
	@echo "Distclean complete!"

# Install dependencies
deps:
	@echo "Installing dependencies..."
	@if ! pkg-config --exists sdl2; then \
		echo "Installing SDL2..."; \
		sudo apt-get update && sudo apt-get install -y libsdl2-dev; \
	fi
	@if ! pkg-config --exists freetype2; then \
		echo "Installing FreeType..."; \
		sudo apt-get install -y libfreetype6-dev; \
	fi
	@echo "Dependencies installed!"

# Generate fonts from TTF files
fonts:
	@echo "Generating fonts from TTF files..."
	@if command -v lv_font_conv >/dev/null 2>&1; then \
		lv_font_conv --font font/NanumGothicCoding.ttf --size 16 --format lvgl --output nanum_gothic_16.c; \
		lv_font_conv --font font/NanumGothicCoding-Bold.ttf --size 16 --format lvgl --output nanum_gothic_bold_16.c; \
		echo "Fonts generated successfully!"; \
	else \
		echo "lv_font_conv not found. Please install it first:"; \
		echo "npm install -g lv_font_conv"; \
	fi

# Run the application
run: $(TARGET)
	@echo "Running video player..."
	./$(TARGET)

# Help target
help:
	@echo "Available targets:"
	@echo "  all       - Build the video player (default)"
	@echo "  clean     - Remove build files"
	@echo "  distclean - Remove all build files including LVGL"
	@echo "  deps      - Install required dependencies"
	@echo "  fonts     - Generate fonts from TTF files"
	@echo "  run       - Build and run the video player"
	@echo "  help      - Show this help message"

# Phony targets
.PHONY: all clean distclean deps fonts run help
