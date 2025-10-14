# Makefile for Chunjiin Korean Input Method with MiniGUI

CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99 -I.
LDFLAGS = -lminigui_ths -lpthread -lfreetype -ljpeg -lpng -lz -lm -ldl

# Try to use pkg-config if available
PKG_CONFIG_MINIGUI = $(shell pkg-config --exists minigui && echo "yes")
PKG_CONFIG_FREETYPE = $(shell pkg-config --exists freetype2 && echo "yes")

ifeq ($(PKG_CONFIG_MINIGUI),yes)
    CFLAGS += $(shell pkg-config --cflags minigui)
    LDFLAGS = $(shell pkg-config --libs minigui)
else
    # Fallback flags
    CFLAGS += -I/usr/local/include -I/usr/include
    LDFLAGS = -L/usr/local/lib $(LDFLAGS)
endif

ifeq ($(PKG_CONFIG_FREETYPE),yes)
    CFLAGS += $(shell pkg-config --cflags freetype2)
    LDFLAGS += $(shell pkg-config --libs freetype2)
endif

TARGET = chunjiin
SOURCES = main.c chunjiin.c chunjiin_hangul.c
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all clean install run setup help debug release

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

main.o: main.c chunjiin.h
	$(CC) $(CFLAGS) -c main.c -o main.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Debug build
debug: CFLAGS += -g -O0 -DDEBUG
debug: $(TARGET)

# Release build  
release: CFLAGS += -O2 -DNDEBUG
release: $(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)
	rm -f *.log

install: $(TARGET)
	@echo "Installing $(TARGET)..."
	sudo mkdir -p /usr/local/bin
	sudo cp $(TARGET) /usr/local/bin/
	sudo chmod +x /usr/local/bin/$(TARGET)
	@if [ -d "assets" ]; then \
		sudo mkdir -p /usr/local/share/$(TARGET); \
		sudo cp -r assets /usr/local/share/$(TARGET)/; \
	fi
	@echo "Installation complete!"
	@echo "Run with: $(TARGET)"

uninstall:
	@echo "Uninstalling $(TARGET)..."
	sudo rm -f /usr/local/bin/$(TARGET)
	sudo rm -rf /usr/local/share/$(TARGET)
	@echo "Uninstallation complete!"

run: $(TARGET)
	@if [ -z "$$DISPLAY" ]; then \
		echo "Warning: No X11 display found. Make sure X11 is running."; \
	fi
	./$(TARGET)

# Setup using the installation script
setup:
	@echo "Setting up MiniGUI environment..."
	@if [ -f "install_minigui.sh" ]; then \
		./install_minigui.sh; \
	else \
		echo "install_minigui.sh not found. Please run the installation script first."; \
		exit 1; \
	fi

help:
	@echo "Chunjiin Korean Input Method - MiniGUI Build System"
	@echo ""
	@echo "Available targets:"
	@echo "  all       - Build the application (default)"
	@echo "  debug     - Build with debug symbols"
	@echo "  release   - Build optimized release version"
	@echo "  clean     - Remove build artifacts"
	@echo "  install   - Install the application system-wide"
	@echo "  uninstall - Remove installed application"
	@echo "  run       - Build and run the application"
	@echo "  setup     - Run MiniGUI installation script"
	@echo "  help      - Show this help message"
	@echo ""
	@echo "Prerequisites:"
	@echo "  1. Run 'make setup' to install MiniGUI"
	@echo "  2. Run 'make' to build the application"
	@echo "  3. Run 'make run' to start the application"
	@echo ""
	@echo "Configuration:"
	@echo "  - MiniGUI.cfg contains the MiniGUI configuration"
	@echo "  - Korean fonts are loaded from assets/ directory"
	@echo "  - X11 display server is required for GUI"

# Show build information
info:
	@echo "Build Information:"
	@echo "  Target: $(TARGET)"
	@echo "  Compiler: $(CC)"
	@echo "  CFLAGS: $(CFLAGS)"
	@echo "  LDFLAGS: $(LDFLAGS)"
	@echo "  Sources: $(SOURCES)"
	@echo ""
	@echo "System Information:"
	@echo "  OS: $(shell uname -s)"
	@echo "  Architecture: $(shell uname -m)"
	@echo "  Display: $(DISPLAY)"
	@echo ""
	@echo "Dependencies:"
	@if command -v pkg-config >/dev/null 2>&1; then \
		echo "  pkg-config: Available"; \
		if pkg-config --exists minigui; then \
			echo "  MiniGUI: $(shell pkg-config --modversion minigui)"; \
		else \
			echo "  MiniGUI: Not found via pkg-config"; \
		fi; \
		if pkg-config --exists freetype2; then \
			echo "  FreeType: $(shell pkg-config --modversion freetype2)"; \
		else \
			echo "  FreeType: Not found via pkg-config"; \
		fi; \
	else \
		echo "  pkg-config: Not available"; \
	fi
	@if ldconfig -p | grep -q libminigui; then \
		echo "  libminigui: Found in system"; \
	else \
		echo "  libminigui: Not found in system"; \
	fi