# Makefile for Chunjiin Korean Input Method with LVGL

CC = gcc
CFLAGS = -Wall -Wextra -O2 -I. -Ilvgl $(shell pkg-config --cflags freetype2)
LDFLAGS = -lSDL2 -lpthread -lm $(shell pkg-config --libs freetype2)
TARGET = chunjiin
SOURCES = main.c chunjiin.c chunjiin_hangul.c

# LVGL source files
LVGL_DIR = lvgl

# Core LVGL sources (includes built-in SDL driver)
LVGL_SOURCES = $(shell find $(LVGL_DIR)/src -name "*.c")

# All sources
ALL_SOURCES = $(SOURCES) $(LVGL_SOURCES)
OBJECTS = $(ALL_SOURCES:.c=.o)

.PHONY: all clean install run setup help

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET) $(TARGET).exe
	find . -name "*.o" -type f -delete

install: $(TARGET)
	@echo "Installing $(TARGET)..."
	cp $(TARGET) /usr/local/bin/ || cp $(TARGET).exe /usr/local/bin/
	@echo "Installation complete!"

run: $(TARGET)
	./$(TARGET)

# Setup LVGL
setup:
	@echo "Setting up LVGL..."
	@if [ ! -d "lvgl" ]; then \
		echo "Cloning LVGL repository..."; \
		git clone --depth 1 --branch release/v9.2 https://github.com/lvgl/lvgl.git; \
	else \
		echo "LVGL directory already exists"; \
	fi
	@echo "Setup complete!"
	@echo ""
	@echo "Next steps:"
	@echo "1. Make sure lv_conf.h is properly configured"
	@echo "2. Make sure SDL2 is installed: sudo apt-get install libsdl2-dev"
	@echo "3. Run 'make' to build the application"

help:
	@echo "Chunjiin Korean Input Method - Makefile (LVGL)"
	@echo ""
	@echo "Available targets:"
	@echo "  make setup    - Clone LVGL repository"
	@echo "  make          - Build the application"
	@echo "  make clean    - Remove build files"
	@echo "  make run      - Build and run the application"
	@echo "  make install  - Install to /usr/local/bin"
	@echo "  make help     - Show this help message"
	@echo ""
	@echo "Requirements:"
	@echo "  - SDL2 development libraries (libsdl2-dev)"
	@echo "  - LVGL v9.2+ with built-in SDL driver (will be cloned by 'make setup')"
