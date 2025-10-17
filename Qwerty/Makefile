# Makefile for Korean/English QWERTY Keypad with LVGL

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2 -D_DEFAULT_SOURCE
LDFLAGS = -lm -lpthread

# LVGL directory - check local first, then system
LVGL_DIR_LOCAL = ./lvgl
LVGL_DIR ?= $(shell [ -d "$(LVGL_DIR_LOCAL)" ] && echo "$(LVGL_DIR_LOCAL)" || pkg-config --variable=includedir lvgl 2>/dev/null || echo "$(LVGL_DIR_LOCAL)")

# SDL2 for display driver (typical for Linux desktop)
SDL_CFLAGS = $(shell pkg-config --cflags sdl2 2>/dev/null || echo "-I/usr/include/SDL2")
SDL_LIBS = $(shell pkg-config --libs sdl2 2>/dev/null || echo "-lSDL2")

# FreeType for TrueType font rendering
FREETYPE_CFLAGS = $(shell pkg-config --cflags freetype2 2>/dev/null || echo "-I/usr/include/freetype2")
FREETYPE_LIBS = $(shell pkg-config --libs freetype2 2>/dev/null || echo "-lfreetype")

# Include paths - lv_conf.h is in current directory
INCLUDES = -I. -I$(LVGL_DIR)

# Determine if we need to compile LVGL sources
ifeq ($(shell [ -d "$(LVGL_DIR_LOCAL)" ] && echo "yes"), yes)
    # Use local LVGL - compile all source files
    LVGL_CFLAGS = $(INCLUDES)
    LVGL_SRC = $(shell find $(LVGL_DIR_LOCAL)/src -name "*.c")
    LVGL_OBJS = $(LVGL_SRC:.c=.o)
    LVGL_LIBS = 
    USE_LOCAL_LVGL = 1
else
    # Use system LVGL
    LVGL_CFLAGS = $(shell pkg-config --cflags lvgl 2>/dev/null || echo "$(INCLUDES)")
    LVGL_SRC = 
    LVGL_OBJS = 
    LVGL_LIBS = $(shell pkg-config --libs lvgl 2>/dev/null || echo "")
    USE_LOCAL_LVGL = 0
endif

# Combine all flags
ALL_CFLAGS = $(CFLAGS) $(LVGL_CFLAGS) $(SDL_CFLAGS) $(FREETYPE_CFLAGS)
ALL_LIBS = $(LVGL_LIBS) $(SDL_LIBS) $(FREETYPE_LIBS) $(LDFLAGS)

TARGET = qwerty
OBJS = main.o qwerty.o $(LVGL_OBJS)

all: check_setup $(TARGET)

check_setup:
	@if [ ! -f "lv_conf.h" ]; then \
		echo "Error: lv_conf.h not found! Run ./setup.sh first."; \
		exit 1; \
	fi
	@if [ ! -d "$(LVGL_DIR)" ]; then \
		echo "Error: LVGL not found! Run ./setup.sh first."; \
		exit 1; \
	fi

$(TARGET): $(OBJS)
	@echo "Linking $@..."
	$(CC) -o $(TARGET) $(OBJS) $(ALL_LIBS)

main.o: main.c qwerty.h lv_conf.h
	@echo "Compiling $<..."
	$(CC) $(ALL_CFLAGS) -c main.c

qwerty.o: qwerty.c qwerty.h
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c qwerty.c

# Compile LVGL source files
ifeq ($(USE_LOCAL_LVGL), 1)
$(LVGL_DIR_LOCAL)/src/%.o: $(LVGL_DIR_LOCAL)/src/%.c
	@echo "Compiling $<..."
	@mkdir -p $(dir $@)
	@$(CC) $(ALL_CFLAGS) -c $< -o $@
endif

clean:
	rm -f $(TARGET) main.o qwerty.o
ifeq ($(USE_LOCAL_LVGL), 1)
	@find $(LVGL_DIR_LOCAL)/src -name "*.o" -delete 2>/dev/null || true
endif

run: $(TARGET)
	./$(TARGET)

# Help target to show configuration
help:
	@echo "Korean/English QWERTY Keypad Makefile"
	@echo "======================================"
	@echo ""
	@echo "Targets:"
	@echo "  all     - Build the application (default)"
	@echo "  clean   - Remove built files"
	@echo "  run     - Build and run the application"
	@echo "  help    - Show this help message"
	@echo ""
	@echo "Dependencies:"
	@echo "  - LVGL (Light and Versatile Graphics Library)"
	@echo "  - SDL2 (for display driver)"
	@echo ""
	@echo "Current configuration:"
	@echo "  LVGL_DIR: $(LVGL_DIR)"
	@echo "  USE_LOCAL_LVGL: $(USE_LOCAL_LVGL)"
	@echo "  CFLAGS: $(ALL_CFLAGS)"
	@echo "  LIBS: $(ALL_LIBS)"
	@echo ""
	@echo "To install dependencies on Ubuntu/Debian:"
	@echo "  ./setup.sh"
	@echo ""
	@echo "Or manually:"
	@echo "  sudo apt-get install build-essential libsdl2-dev"
	@echo "  git clone https://github.com/lvgl/lvgl.git"

.PHONY: all clean run help check_setup
