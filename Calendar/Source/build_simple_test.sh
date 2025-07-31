#!/bin/bash

# Build script for ChunJiIn Input Simple Test Suite
echo "Building ChunJiIn Input Simple Test Suite..."

# Set compiler
CC=gcc

# Compiler flags
CFLAGS="-Wall -Wextra -std=c99 -I./include"

# Source files
SOURCES="src/chunjiin_input.c src/test_chunjiin_input_simple.c"

# Output executable
OUTPUT="test_chunjiin_input_simple"

# Build command
$CC $CFLAGS -o $OUTPUT $SOURCES

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "Running simple test suite..."
    echo "========================"
    ./$OUTPUT
    echo "========================"
    echo "Simple test completed!"
else
    echo "Build failed!"
    exit 1
fi 