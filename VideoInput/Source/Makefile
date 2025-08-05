# Makefile for building all tests and main application
# Usage: make [target]
# Targets: all, main, tests, test_chunjiin, calendar_test, clock_test, clean, clean-all

# Default target
all: main tests

# Build directory
BUILD_DIR = build
LIB_DIR = $(BUILD_DIR)/lib

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	mkdir -p $(LIB_DIR)

# Main application
main: $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake .. && make main
	@echo "Main application built successfully"

# All tests
tests: test_chunjiin calendar_test clock_test svg_test video_test webcam_test
	@echo "All tests built successfully"

# Individual test targets
test_chunjiin: $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake .. && make test_chunjiin
	@echo "ChunJiIn test built successfully"

calendar_test: $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake .. && make calendar_test
	@echo "Calendar test built successfully"

clock_test: $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake .. && make clock_test
	@echo "Clock test built successfully"

svg_test: $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake .. && make svg_test
	@echo "SVG test built successfully"

video_test: $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake .. && make video_test
	@echo "Video test built successfully"

webcam_test: $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake .. && make webcam_test
	@echo "Webcam test built successfully"

# Run all tests
run-tests: tests
	@echo "Running all tests..."
	@echo "Running ChunJiIn test..."
	./$(BUILD_DIR)/test_chunjiin
	@echo "Running Calendar test..."
	./$(BUILD_DIR)/calendar_test
	@echo "Running Clock test..."
	./$(BUILD_DIR)/clock_test
	@echo "Running SVG test..."
	./$(BUILD_DIR)/svg_test
	@echo "Running Video test..."
	./$(BUILD_DIR)/video_test
	@echo "Running Webcam test..."
	./$(BUILD_DIR)/webcam_test
	@echo "All tests completed"

# Clean targets
clean:
	cd $(BUILD_DIR) && make clean-main
	@echo "Cleaned main application files"

clean-all:
	cd $(BUILD_DIR) && make clean-all
	@echo "Cleaned all build files"

# Install dependencies (if needed)
install-deps:
	./install_freetype_deps.sh
	@echo "Dependencies installed"

# Help target
help:
	@echo "Available targets:"
	@echo "  all          - Build main application and all tests (default)"
	@echo "  main         - Build main application only"
	@echo "  tests        - Build all tests"
	@echo "  test_chunjiin - Build ChunJiIn test only"
	@echo "  calendar_test - Build calendar test only"
	@echo "  clock_test   - Build clock test only"
	@echo "  svg_test     - Build SVG test only"
	@echo "  video_test   - Build video test only"
	@echo "  webcam_test  - Build webcam test only"
	@echo "  run-tests    - Build and run all tests"
	@echo "  clean        - Clean main application files"
	@echo "  clean-all    - Clean all build files"
	@echo "  install-deps - Install dependencies"
	@echo "  help         - Show this help message"

.PHONY: all main tests test_chunjiin calendar_test clock_test svg_test run-tests clean clean-all install-deps help 