# Makefile for Chunjiin Korean Input Method

CC = gcc
CFLAGS = -Wall -Wextra -O2 $(shell pkg-config --cflags gtk+-3.0)
LIBS = $(shell pkg-config --libs gtk+-3.0) -lfontconfig
TARGET = chunjiin
SOURCES = main.c chunjiin.c chunjiin_hangul.c
OBJECTS = $(SOURCES:.c=.o)

# Required packages
REQUIRED_PKGS = gtk+-3.0
REQUIRED_LIBS = fontconfig

.PHONY: all clean install run check-deps help

all: check-deps $(TARGET)

# Check for required dependencies
check-deps:
	@echo "Checking dependencies..."
	@command -v pkg-config >/dev/null 2>&1 || { \
		echo "ERROR: pkg-config is not installed."; \
		echo "Install with: sudo apt-get install pkg-config"; \
		exit 1; \
	}
	@for pkg in $(REQUIRED_PKGS); do \
		pkg-config --exists $$pkg || { \
			echo "ERROR: $$pkg is not installed."; \
			echo "Install with: sudo apt-get install lib$${pkg%-*}-dev"; \
			exit 1; \
		}; \
	done
	@for lib in $(REQUIRED_LIBS); do \
		ldconfig -p | grep -q lib$$lib || { \
			echo "ERROR: lib$$lib is not installed."; \
			echo "Install with: sudo apt-get install lib$$lib-dev"; \
			exit 1; \
		}; \
	done
	@echo "All dependencies are satisfied!"

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LIBS)

%.o: %.c chunjiin.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET) $(TARGET).exe

install: $(TARGET)
	@echo "Installing $(TARGET)..."
	cp $(TARGET) /usr/local/bin/ || cp $(TARGET).exe /usr/local/bin/
	@echo "Installation complete!"

run: $(TARGET)
	./$(TARGET)

help:
	@echo "Chunjiin Korean Input Method - Makefile"
	@echo ""
	@echo "Available targets:"
	@echo "  make check-deps  - Check if all dependencies are installed"
	@echo "  make             - Build the application (checks deps first)"
	@echo "  make clean       - Remove build files"
	@echo "  make run         - Build and run the application"
	@echo "  make install     - Install to /usr/local/bin"
	@echo "  make deps-info   - Show detailed dependency installation info"
	@echo "  make help        - Show this help message"
	@echo ""

deps-info:
	@echo "Required Dependencies for Chunjiin:"
	@echo ""
	@echo "1. Build essentials:"
	@echo "   sudo apt-get install build-essential"
	@echo ""
	@echo "2. pkg-config:"
	@echo "   sudo apt-get install pkg-config"
	@echo ""
	@echo "3. GTK+ 3.0 development libraries:"
	@echo "   sudo apt-get install libgtk-3-dev"
	@echo ""
	@echo "4. Fontconfig development libraries:"
	@echo "   sudo apt-get install libfontconfig1-dev"
	@echo ""
	@echo "Install all at once:"
	@echo "   sudo apt-get install build-essential pkg-config libgtk-3-dev libfontconfig1-dev"
	@echo ""
