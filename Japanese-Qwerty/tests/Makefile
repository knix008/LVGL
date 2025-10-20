# Makefile for Japanese QWERTY Tests

CC = gcc
CFLAGS = -Wall -Wextra -O2 -I..

# Test executables
TESTS = test_japanese_qwerty test_dakuten

# Default target - build and run all tests
all: $(TESTS)
	@echo ""
	@echo "Running all tests..."
	@echo ""
	@./test_japanese_qwerty
	@./test_dakuten

# Build test executables
test_japanese_qwerty: test_japanese_qwerty.c ../japanese_qwerty.c
	$(CC) $(CFLAGS) -o $@ test_japanese_qwerty.c ../japanese_qwerty.c

test_dakuten: test_dakuten.c
	$(CC) $(CFLAGS) -o $@ test_dakuten.c

# Run individual tests
run-qwerty: test_japanese_qwerty
	./test_japanese_qwerty

run-dakuten: test_dakuten
	./test_dakuten

# Clean
clean:
	rm -f $(TESTS)
	@echo "Test cleanup complete!"

# Help
help:
	@echo "Japanese QWERTY Test Suite"
	@echo ""
	@echo "Targets:"
	@echo "  all          - Build and run all tests (default)"
	@echo "  run-qwerty   - Run Japanese QWERTY tests only"
	@echo "  run-dakuten  - Run dakuten combining tests only"
	@echo "  clean        - Remove test executables"
	@echo "  help         - Show this help"

.PHONY: all run-qwerty run-dakuten clean help

