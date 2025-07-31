#!/bin/bash

# Build script for Calendar Test Suite
echo "Building Calendar Test Suite..."

# Set compiler
CC=gcc

# Compiler flags
CFLAGS="-Wall -Wextra -std=c99 -I./include"

# Source files
SOURCES="src/test_calendar.c"

# Output executable
OUTPUT="calendar_test"

# Build command
$CC $CFLAGS -o $OUTPUT $SOURCES

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "Running calendar test suite..."
    echo "========================"
    ./$OUTPUT
    echo "========================"
    echo "Calendar test completed!"
else
    echo "Build failed!"
    exit 1
fi 