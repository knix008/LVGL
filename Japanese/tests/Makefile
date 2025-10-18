# Makefile for Japanese Input Tests
# Compiles and runs test cases for the Japanese input application

CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99
INCLUDES = -I.. -I../lvgl
LIBS = -lm

# Test source files
TEST_CORE_SRC = test_core.c ../japanese_input.c
TEST_GUI_SRC = test_gui.c ../japanese_input.c

# Test executables
TEST_CORE = test_core
TEST_GUI = test_gui

# Default target - run all tests
all: $(TEST_CORE) $(TEST_GUI)
	@echo "Running all tests..."
	@echo ""
	@./$(TEST_CORE)
	@echo ""
	@./$(TEST_GUI)
	@echo ""
	@echo "All tests completed!"

# Compile core tests
$(TEST_CORE): $(TEST_CORE_SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

# Compile GUI tests
$(TEST_GUI): $(TEST_GUI_SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

# Run only core tests
test-core: $(TEST_CORE)
	./$(TEST_CORE)

# Run only GUI tests
test-gui: $(TEST_GUI)
	./$(TEST_GUI)

# Clean test executables
clean:
	rm -f $(TEST_CORE) $(TEST_GUI)

# Help target
help:
	@echo "Available targets:"
	@echo "  all      - Run all tests (default)"
	@echo "  test-core - Run only core logic tests"
	@echo "  test-gui  - Run only GUI tests"
	@echo "  clean    - Remove test executables"
	@echo "  help     - Show this help message"

.PHONY: all test-core test-gui clean help
