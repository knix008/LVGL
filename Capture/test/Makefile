# Makefile for Unit Tests
# Tests the camera module without GUI dependencies

CC = gcc
CFLAGS = -Wall -Wextra -g -I..
LDFLAGS = -lpthread -lm -ljpeg

# FFmpeg configuration
FFMPEG_CFLAGS = $(shell pkg-config --cflags libavformat libavcodec libavdevice libswscale libavutil)
FFMPEG_LIBS = $(shell pkg-config --libs libavformat libavcodec libavdevice libswscale libavutil)

# Test executables
TESTS = test_camera test_integration

# Camera module object (without GUI dependencies)
CAMERA_OBJ = camera_test.o

all: $(TESTS)

camera_test.o: ../camera.c
	@echo "Compiling camera module for testing..."
	$(CC) $(CFLAGS) $(FFMPEG_CFLAGS) -c ../camera.c -o camera_test.o

test_camera: test_camera.o $(CAMERA_OBJ)
	@echo "Linking test_camera..."
	$(CC) test_camera.o $(CAMERA_OBJ) $(FFMPEG_LIBS) $(LDFLAGS) -o test_camera

test_integration: test_integration.o $(CAMERA_OBJ)
	@echo "Linking test_integration..."
	$(CC) test_integration.o $(CAMERA_OBJ) $(FFMPEG_LIBS) $(LDFLAGS) -o test_integration

%.o: %.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) $(FFMPEG_CFLAGS) -c $< -o $@

clean:
	@echo "Cleaning test files..."
	rm -f *.o $(TESTS) *.jpg
	@echo "Clean complete"

run: all
	@echo ""
	@echo "Running all tests..."
	@echo ""
	@./test_camera
	@echo ""
	@./test_integration
	@echo ""
	@echo "All tests completed!"

.PHONY: all clean run
