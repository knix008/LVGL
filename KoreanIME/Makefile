# Makefile for Korean IME
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
TARGET = korean
SOURCES = main.c korean.c
HEADER = korean.h

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(SOURCES) $(HEADER)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

# Clean build artifacts
clean:
	rm -f $(TARGET) test_korean test_simple

# Install to system (optional - requires sudo)
install: $(TARGET)
	sudo cp $(TARGET) /usr/local/bin/

# Uninstall from system (optional - requires sudo)
uninstall:
	sudo rm -f /usr/local/bin/$(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Debug build
debug: CFLAGS += -g -DDEBUG
debug: $(TARGET)

# Release build
release: CFLAGS += -DNDEBUG
release: $(TARGET)

# Build and run comprehensive tests
test: test_korean.c korean.c $(HEADER)
	$(CC) $(CFLAGS) -o test_korean test_korean.c korean.c
	./test_korean

# Build and run simple tests
test-simple: test_simple.c korean.c $(HEADER)
	$(CC) $(CFLAGS) -o test_simple test_simple.c korean.c
	./test_simple

# Run all tests
test-all: test test-simple
	@echo ""
	@echo "🎉 All tests completed successfully!"

# Show help
help:
	@echo "Available targets:"
	@echo "  all      - Build the Korean IME (default)"
	@echo "  clean    - Remove build artifacts"
	@echo "  install  - Install to /usr/local/bin (requires sudo)"
	@echo "  uninstall- Remove from /usr/local/bin (requires sudo)"
	@echo "  run      - Build and run the program"
	@echo "  test     - Build and run comprehensive tests"
	@echo "  test-simple - Build and run simple tests"
	@echo "  test-all  - Run all tests (comprehensive + simple)"
	@echo "  debug    - Build with debug symbols"
	@echo "  release  - Build optimized release version"
	@echo "  help     - Show this help message"

.PHONY: all clean install uninstall run debug release test test-simple test-all help 