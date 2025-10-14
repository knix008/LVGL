# MiniGUI Application Makefile

# Compiler and flags
CC = gcc
CFLAGS = -Wall -g -O2 -Wno-unused-parameter
LDFLAGS = -Wl,--as-needed

# Local MiniGUI installation paths
MINIGUI_CFLAGS = -I./local/install/include
MINIGUI_LIBS = -L./local/install/lib -lminigui_sa -ljpeg -lpng16 -lz -ldl -lharfbuzz -lfreetype -lm -lpthread -linput -ldrm -ludev

# Target executable
TARGET = helloworld

# Source files
SOURCES = main.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(MINIGUI_LIBS) $(LDFLAGS)

# Compile source files
%.o: %.c
	$(CC) $(CFLAGS) $(MINIGUI_CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJECTS) $(TARGET)

# Check if MiniGUI is installed locally
check-minigui:
	@echo "Checking local MiniGUI installation..."
	@if [ -f "./local/install/lib/libminigui_sa.so" ]; then \
		echo "✓ MiniGUI found locally"; \
	else \
		echo "✗ MiniGUI not found locally - run ./run.sh to build it"; \
		exit 1; \
	fi

# Help target
help:
	@echo "Available targets:"
	@echo "  all          - Build the application (default)"
	@echo "  clean        - Remove build files"
	@echo "  check-minigui - Check if MiniGUI is installed locally"
	@echo "  help         - Show this help"
	@echo ""
	@echo "Note: Run './run.sh' first to download and build MiniGUI locally"

.PHONY: all clean check-minigui help