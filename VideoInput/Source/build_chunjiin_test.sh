#!/bin/bash

# Build script for ChunJiIn Input Test Suite
echo "Building ChunJiIn Input Test Suite..."

# Set compiler
CC=gcc

# Compiler flags
CFLAGS="-Wall -Wextra -std=c99 -I./include"

# Source files
SOURCES="src/chunjiin_input.c src/test_chunjiin_input.c"

# Output executable
OUTPUT="test_chunjiin_input"

# Build command
$CC $CFLAGS -o $OUTPUT $SOURCES

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "Running test suite..."
    echo "========================"
    ./$OUTPUT
    echo "========================"
    echo "Test completed!"
else
    echo "Build failed!"
    exit 1
fi 