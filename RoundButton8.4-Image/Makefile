# Makefile for Round Button LVGL Application
# Target: Linux with SDL2
# Note: LVGL library should be built separately using setup.sh

# Compiler and flags
CC = gcc
CFLAGS = -O2 -Wall -Wextra -Wno-unused-parameter -Wno-unused-function
CFLAGS += -I. -I./lvgl
CFLAGS += -DLV_CONF_INCLUDE_SIMPLE

# SDL2 flags
SDL_CFLAGS := $(shell pkg-config --cflags sdl2)
SDL_LDFLAGS := $(shell pkg-config --libs sdl2)

# FreeType flags (if using TrueType fonts)
FREETYPE_CFLAGS := $(shell pkg-config --cflags freetype2)
FREETYPE_LDFLAGS := $(shell pkg-config --libs freetype2)

# Combine flags
CFLAGS += $(SDL_CFLAGS) $(FREETYPE_CFLAGS)
LDFLAGS = $(SDL_LDFLAGS) $(FREETYPE_LDFLAGS) -lm -lpthread

# Target executable
TARGET = button

# Application source files (only our code, not LVGL)
APP_SRC = main.c
APP_OBJ = main.o

# LVGL directory
LVGL_DIR = lvgl

# LVGL pre-built library (built by setup.sh)
LVGL_LIB = $(LVGL_DIR)/lib/liblvgl.a

# Check if LVGL library exists
ifeq (,$(wildcard $(LVGL_LIB)))
    $(warning LVGL library not found at $(LVGL_LIB))
    $(warning Please run ./setup.sh first to build LVGL)
endif

# Build rules
all: check-lvgl $(TARGET)

check-lvgl:
	@if [ ! -f "$(LVGL_LIB)" ]; then \
		echo "Error: LVGL library not found!"; \
		echo "Please run: ./setup.sh"; \
		exit 1; \
	fi

$(TARGET): $(APP_OBJ) $(LVGL_LIB)
	@echo "Linking $(TARGET)..."
	@$(CC) -o $@ $(APP_OBJ) $(LVGL_LIB) $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

$(APP_OBJ): $(APP_SRC)
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Cleaning application build files..."
	@rm -f $(APP_OBJ) $(TARGET)
	@echo "Clean complete"

clean-all: clean
	@echo "Cleaning LVGL library..."
	@rm -rf $(LVGL_DIR)/lib $(LVGL_DIR)/build
	@find $(LVGL_DIR) -name "*.o" -delete
	@echo "Clean all complete"

run: $(TARGET)
	@echo "Running $(TARGET)..."
	@./$(TARGET)

rebuild: clean all

help:
	@echo "Makefile for Round Button LVGL Application"
	@echo ""
	@echo "Targets:"
	@echo "  all       - Build the application (default)"
	@echo "  clean     - Remove application build files"
	@echo "  clean-all - Remove application and LVGL build files"
	@echo "  run       - Build and run the application"
	@echo "  rebuild   - Clean and build application"
	@echo "  help      - Show this help message"
	@echo ""
	@echo "Note: Run ./setup.sh first to build LVGL library"

.PHONY: all check-lvgl clean clean-all run rebuild help
