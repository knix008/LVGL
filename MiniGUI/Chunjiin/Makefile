# MiniGUI Application Makefile

# Compiler and flags
CC = gcc
CFLAGS = -Wall -g -O2 -Wno-unused-parameter
LDFLAGS = -Wl,--as-needed

# Local MiniGUI installation paths
MINIGUI_CFLAGS = -I./install/include
MINIGUI_LIBS = -L./install/lib -lminigui_sa -ljpeg -lpng16 -lz -ldl -lharfbuzz -lfreetype -lm -lpthread -linput -ldrm -ludev

# Target executable
TARGET = chunjiin_app

# Source files
SOURCES = main.c chunjiin.c chunjiin_hangul.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Default target
all: copy-fonts $(TARGET)

# Copy Korean fonts to MiniGUI font directory
copy-fonts:
	@echo "Copying Korean fonts to MiniGUI font directory..."
	@mkdir -p ./install/share/fonts
	@cp assets/*.ttf ./install/share/fonts/ 2>/dev/null || echo "Warning: Could not copy fonts (assets directory may not exist)"
	@echo "✓ Korean fonts copied to ./install/share/fonts/"

# Build the executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(MINIGUI_LIBS) $(LDFLAGS)

# Compile source files
%.o: %.c
	$(CC) $(CFLAGS) $(MINIGUI_CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJECTS)
	rm -f $(TARGET)
	rm -f ./install/share/fonts/*.ttf

# Check if MiniGUI is installed locally
check-minigui:
	@echo "Checking local MiniGUI installation..."
	@if [ -f "./install/lib/libminigui_sa.so" ]; then \
		echo "✓ MiniGUI found locally"; \
	else \
		echo "✗ MiniGUI not found locally - run ./build.sh to build it"; \
		exit 1; \
	fi

# Help target
help:
	@echo "Available targets:"
	@echo "  all          - Build ChunJiIn application (default)"
	@echo "  clean        - Remove build files"
	@echo "  check-minigui - Check if MiniGUI is installed locally"
	@echo "  help         - Show this help"
	@echo ""
	@echo "Application:"
	@echo "  ./chunjiin_app  - ChunJiIn Korean input system"
	@echo ""
	@echo "Note: Run './build.sh' first to download and build MiniGUI locally"

.PHONY: all clean check-minigui help