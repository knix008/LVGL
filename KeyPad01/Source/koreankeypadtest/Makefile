# Makefile for Korean Keypad Test Program
# Builds the koreankeypadtest program in the local directory

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
LDFLAGS = 

# Source files
SOURCES = korean_test.c korean_hangul.c
OBJECTS = $(SOURCES:.c=.o)

# Target executable
TARGET = koreankeypadtest

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(OBJECTS)
	@echo "Linking $(TARGET)..."
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "Build successful! Executable: $(TARGET)"

# Compile source files
%.o: %.c korean_hangul.h
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	@rm -f $(OBJECTS) $(TARGET)
	@rm -rf build/
	@echo "Clean completed!"

# Install (copy to /usr/local/bin if needed)
install: $(TARGET)
	@echo "Installing $(TARGET) to /usr/local/bin..."
	sudo cp $(TARGET) /usr/local/bin/
	@echo "Installation completed!"

# Uninstall
uninstall:
	@echo "Uninstalling $(TARGET) from /usr/local/bin..."
	sudo rm -f /usr/local/bin/$(TARGET)
	@echo "Uninstallation completed!"

# Run the program
run: $(TARGET)
	@echo "Running $(TARGET)..."
	./$(TARGET)

# Build and test
test: $(TARGET)
	@echo "Running quick test..."
	@echo "rk" | timeout 5s ./$(TARGET) > /dev/null 2>&1
	@if [ $$? -eq 0 ]; then \
		echo "Test passed!"; \
	else \
		echo "Test failed or timed out"; \
	fi

# Show help
help:
	@echo "Korean Keypad Test Program Makefile"
	@echo "==================================="
	@echo ""
	@echo "Available targets:"
	@echo "  all         - Build the koreankeypadtest program (default)"
	@echo "  clean       - Remove build artifacts"
	@echo "  install     - Install to /usr/local/bin"
	@echo "  uninstall   - Remove from /usr/local/bin"
	@echo "  run         - Build and run the program"
	@echo "  test        - Build and run a quick test"
	@echo "  help        - Show this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make        # Build the program"
	@echo "  make clean  # Clean build artifacts"
	@echo "  make run    # Build and run"
	@echo "  make test   # Build and test"

# Phony targets
.PHONY: all clean install uninstall run test help 