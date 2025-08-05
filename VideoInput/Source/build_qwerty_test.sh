#!/bin/bash

# Build script for Qwerty Korean Input Test Suite
echo "Building Qwerty Korean Input Test Suite..."

# Set compiler
CC=gcc

# Compiler flags
CFLAGS="-Wall -Wextra -std=c99 -I./include"

# Source files
SOURCES="src/test_qwerty_korea.c src/qwerty_korean.c"

# Output executable
OUTPUT="qwerty_korean_test"

# Build command
$CC $CFLAGS -o $OUTPUT $SOURCES

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "Running qwerty Korean test suite..."
    echo "========================"
    ./$OUTPUT
    echo "========================"
    echo "Qwerty Korean test completed!"
else
    echo "Build failed!"
    exit 1
fi 