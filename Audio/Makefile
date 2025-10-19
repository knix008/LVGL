# Makefile for Audio Player with LVGL

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2 -D_DEFAULT_SOURCE
LDFLAGS = -lm -lpthread

# LVGL directory and library
LVGL_DIR = ./lvgl
LVGL_LIB = $(LVGL_DIR)/build/liblvgl.a

# SDL2 for display driver
SDL_CFLAGS = $(shell pkg-config --cflags sdl2 2>/dev/null || echo "-I/usr/include/SDL2")
SDL_LIBS = $(shell pkg-config --libs sdl2 2>/dev/null || echo "-lSDL2")

# SDL2_mixer for audio playback
SDL_MIXER_CFLAGS = $(shell pkg-config --cflags SDL2_mixer 2>/dev/null || echo "-I/usr/include/SDL2")
SDL_MIXER_LIBS = $(shell pkg-config --libs SDL2_mixer 2>/dev/null || echo "-lSDL2_mixer")

# FreeType for TrueType font rendering
FREETYPE_CFLAGS = $(shell pkg-config --cflags freetype2 2>/dev/null || echo "-I/usr/include/freetype2")
FREETYPE_LIBS = $(shell pkg-config --libs freetype2 2>/dev/null || echo "-lfreetype")

# Include paths - lv_conf.h is in current directory
INCLUDES = -I. -I$(LVGL_DIR)

# Combine all flags
ALL_CFLAGS = $(CFLAGS) $(INCLUDES) $(SDL_CFLAGS) $(SDL_MIXER_CFLAGS) $(FREETYPE_CFLAGS)
ALL_LIBS = $(LVGL_LIB) $(SDL_LIBS) $(SDL_MIXER_LIBS) $(FREETYPE_LIBS) $(LDFLAGS)

TARGET = audio_player
OBJS = main.o audio_player.o audio_gui.o

all: check_setup $(TARGET)

check_setup:
	@if [ ! -f "lv_conf.h" ]; then \
		echo "Error: lv_conf.h not found! Run ./setup.sh first."; \
		exit 1; \
	fi
	@if [ ! -f "$(LVGL_LIB)" ]; then \
		echo "Error: LVGL library not found! Run ./setup.sh first."; \
		exit 1; \
	fi
	@echo "Checking for SDL2_mixer..."
	@if ! pkg-config --exists SDL2_mixer 2>/dev/null; then \
		echo "Warning: SDL2_mixer not found via pkg-config."; \
		echo "Please install SDL2_mixer:"; \
		echo "  Ubuntu/Debian: sudo apt-get install libsdl2-mixer-dev"; \
		echo "  Fedora: sudo dnf install SDL2_mixer-devel"; \
		echo "  macOS: brew install sdl2_mixer"; \
	fi

$(TARGET): $(OBJS)
	@echo "Linking $@..."
	$(CC) -o $(TARGET) $(OBJS) $(ALL_LIBS)
	@echo "Build complete!"
	@echo "Run with: ./$(TARGET)"

main.o: main.c audio_player.h audio_gui.h lv_conf.h
	@echo "Compiling $<..."
	$(CC) $(ALL_CFLAGS) -c main.c

audio_player.o: audio_player.c audio_player.h audio_gui.h
	@echo "Compiling $<..."
	$(CC) $(ALL_CFLAGS) -c audio_player.c

audio_gui.o: audio_gui.c audio_gui.h audio_player.h
	@echo "Compiling $<..."
	$(CC) $(ALL_CFLAGS) -c audio_gui.c

clean:
	rm -f $(TARGET) $(OBJS)

clean-lvgl:
	@echo "Cleaning LVGL build directory..."
	@rm -rf $(LVGL_DIR)/build
	@echo "LVGL build directory cleaned. Run ./setup.sh to rebuild."

run: $(TARGET)
	./$(TARGET)

# Create assets directory structure
setup-assets:
	@echo "Creating assets directory structure..."
	@mkdir -p assets/music
	@echo "Place your MP3 and WAV files in assets/music/"
	@echo "Example files:"
	@echo "  assets/music/example1.mp3"
	@echo "  assets/music/example2.wav"

# Help target to show configuration
help:
	@echo "Audio Player Makefile"
	@echo "===================="
	@echo ""
	@echo "Targets:"
	@echo "  all           - Build the application (default)"
	@echo "  clean         - Remove built files"
	@echo "  clean-lvgl    - Remove LVGL build directory (run ./setup.sh to rebuild)"
	@echo "  run           - Build and run the application"
	@echo "  setup-assets  - Create assets directory structure"
	@echo "  help          - Show this help message"
	@echo ""
	@echo "Dependencies:"
	@echo "  - LVGL (Light and Versatile Graphics Library)"
	@echo "  - SDL2 (for display driver)"
	@echo "  - SDL2_mixer (for audio playback - MP3, WAV)"
	@echo "  - FreeType (for font rendering)"
	@echo ""
	@echo "Install dependencies:"
	@echo "  Ubuntu/Debian:"
	@echo "    sudo apt-get install libsdl2-dev libsdl2-mixer-dev libfreetype6-dev"
	@echo "  Fedora:"
	@echo "    sudo dnf install SDL2-devel SDL2_mixer-devel freetype-devel"
	@echo "  macOS:"
	@echo "    brew install sdl2 sdl2_mixer freetype"
	@echo ""
	@echo "Current configuration:"
	@echo "  LVGL_DIR: $(LVGL_DIR)"
	@echo "  LVGL_LIB: $(LVGL_LIB)"
	@echo "  CFLAGS: $(ALL_CFLAGS)"
	@echo "  LIBS: $(ALL_LIBS)"
	@echo ""
	@echo "Display: 320x640 resolution"
	@echo ""
	@echo "Controls:"
	@echo "  - Mouse: Click on buttons and sliders"
	@echo "  - Keyboard: Q or ESC to quit, SPACE to play/pause"

.PHONY: all clean clean-lvgl run setup-assets help check_setup
