# Test Makefile for Video Player
# This Makefile builds and runs test cases for the video player

# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2 -D_DEFAULT_SOURCE
INCLUDES = -I.. -I../lvgl -I../lvgl/src -I/usr/include/SDL2 -D_REENTRANT -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/x86_64-linux-gnu
LIBS = -lm -lpthread -lSDL2 -lfreetype -lavcodec -lavformat -lavutil -lswscale -lswresample

# Source files
TEST_SOURCES = test_video_player.c
SIMPLE_TEST_SOURCES = test_video_player_simple.c
CORE_TEST_SOURCES = test_video_player_core.c
VIDEO_PLAYER_SOURCES = ../video_player.c ../korean_fonts.c
LVGL_LIB = ../lvgl/build/liblvgl.a

# Target executables
TEST_TARGET = test_video_player
SIMPLE_TEST_TARGET = test_video_player_simple
CORE_TEST_TARGET = test_video_player_core

# Default target
all: $(TEST_TARGET) $(CORE_TEST_TARGET)

# Build test executable
$(TEST_TARGET): $(TEST_SOURCES) $(VIDEO_PLAYER_SOURCES) $(LVGL_LIB)
	@echo "Building test executable..."
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TEST_TARGET) $(TEST_SOURCES) $(VIDEO_PLAYER_SOURCES) $(LVGL_LIB) $(LIBS)
	@echo "Test executable built successfully!"

# Build simple test executable (without LVGL)
$(SIMPLE_TEST_TARGET): $(SIMPLE_TEST_SOURCES) $(VIDEO_PLAYER_SOURCES)
	@echo "Building simple test executable..."
	$(CC) $(CFLAGS) -I.. -o $(SIMPLE_TEST_TARGET) $(SIMPLE_TEST_SOURCES) $(VIDEO_PLAYER_SOURCES) $(LIBS)
	@echo "Simple test executable built successfully!"

# Build core test executable (standalone)
$(CORE_TEST_TARGET): $(CORE_TEST_SOURCES)
	@echo "Building core test executable..."
	$(CC) $(CFLAGS) -o $(CORE_TEST_TARGET) $(CORE_TEST_SOURCES)
	@echo "Core test executable built successfully!"

# Run tests
test: $(TEST_TARGET)
	@echo "Running video player tests..."
	@echo "=========================================="
	./$(TEST_TARGET)
	@echo "=========================================="

# Run simple tests
test-simple: $(SIMPLE_TEST_TARGET)
	@echo "Running simple video player tests..."
	@echo "=========================================="
	./$(SIMPLE_TEST_TARGET)
	@echo "=========================================="

# Run core tests
test-core: $(CORE_TEST_TARGET)
	@echo "Running core video player tests..."
	@echo "=========================================="
	./$(CORE_TEST_TARGET)
	@echo "=========================================="

# Run tests with verbose output
test-verbose: $(TEST_TARGET)
	@echo "Running video player tests with verbose output..."
	@echo "=========================================="
	./$(TEST_TARGET) 2>&1 | tee test_results.log
	@echo "=========================================="
	@echo "Test results saved to test_results.log"

# Clean up
clean:
	@echo "Cleaning up test files..."
	rm -f $(TEST_TARGET) $(SIMPLE_TEST_TARGET) $(CORE_TEST_TARGET) test_results.log
	rm -f ../video/test_video.mp4
	@echo "Cleanup complete!"

# Install test dependencies
install-deps:
	@echo "Installing test dependencies..."
	@if command -v apt-get >/dev/null 2>&1; then \
		echo "Installing dependencies via apt-get..."; \
		sudo apt-get update && sudo apt-get install -y ffmpeg; \
	elif command -v yum >/dev/null 2>&1; then \
		echo "Installing dependencies via yum..."; \
		sudo yum install -y ffmpeg; \
	elif command -v pacman >/dev/null 2>&1; then \
		echo "Installing dependencies via pacman..."; \
		sudo pacman -S --noconfirm ffmpeg; \
	else \
		echo "Please install ffmpeg manually for test video generation"; \
	fi

# Help target
help:
	@echo "Video Player Test Suite Makefile"
	@echo "================================="
	@echo "Available targets:"
	@echo "  all          - Build test executable (default)"
	@echo "  test         - Build and run tests"
	@echo "  test-verbose - Build and run tests with verbose output"
	@echo "  clean        - Clean up test files"
	@echo "  install-deps - Install test dependencies (ffmpeg)"
	@echo "  help         - Show this help message"
	@echo ""
	@echo "Usage:"
	@echo "  make test        # Run tests"
	@echo "  make test-verbose # Run tests with detailed output"
	@echo "  make clean      # Clean up"

# Phony targets
.PHONY: all test test-verbose clean install-deps help
