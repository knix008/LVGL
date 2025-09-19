# MiniGUI Application Makefile

# Compiler and flags
CC = gcc
CFLAGS = -Wall -g -O2
LDFLAGS = 

# MiniGUI specific flags
MINIGUI_CFLAGS = -I/usr/local/include/minigui
MINIGUI_LIBS = -L/usr/local/lib -lminigui_ths -ljpeg -lpng16 -lz -ldl -lharfbuzz -lfreetype -lpixman-1 -lwebp -lm -lpthread -linput -ldrm -ludev -lpciaccess

# Alternative paths for manual installation
# MINIGUI_CFLAGS = -I/usr/local/include/minigui
# MINIGUI_LIBS = -L/usr/local/lib -lminigui

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

# Install (optional)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

# Uninstall (optional)
uninstall:
	rm -f /usr/local/bin/$(TARGET)

# Check if MiniGUI is installed
check-minigui:
	@echo "Checking MiniGUI installation..."
	@pkg-config --exists minigui && echo "MiniGUI found" || echo "MiniGUI not found - please install libminigui-dev"

# Help target
help:
	@echo "Available targets:"
	@echo "  all          - Build the application (default)"
	@echo "  clean        - Remove build files"
	@echo "  install      - Install to /usr/local/bin"
	@echo "  uninstall    - Remove from /usr/local/bin"
	@echo "  check-minigui - Check if MiniGUI is installed"
	@echo "  help         - Show this help"

.PHONY: all clean install uninstall check-minigui help
