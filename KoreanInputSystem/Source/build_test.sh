#!/bin/bash

# Build script for ChunJiIn Input Test Suite using CMake
echo "Building ChunJiIn Input Test Suite with CMake..."

# Get the directory where this script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SOURCE_DIR="$SCRIPT_DIR"
BUILD_DIR="$SOURCE_DIR/build"

# Create build directory if it doesn't exist
if [ ! -d "$BUILD_DIR" ]; then
    echo "Creating build directory: $BUILD_DIR"
    mkdir -p "$BUILD_DIR"
fi

# Change to build directory
cd "$BUILD_DIR"

# Configure with cmake
echo "Configuring with cmake..."
cmake "$SOURCE_DIR"

if [ $? -ne 0 ]; then
    echo "CMake configuration failed!"
    exit 1
fi

# Build the test executables
echo "Building test executables..."
make test_chunjiin test_chunjiin_simple test_gwon_standalone

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "Running test suite..."
    echo "========================"
    
    # Run the main ChunJiIn test
    if [ -f "./test_chunjiin" ]; then
        echo "Running test_chunjiin..."
        ./test_chunjiin
        echo "========================"
    fi
    
    # Run the simple test if available
    if [ -f "./test_chunjiin_simple" ]; then
        echo "Running test_chunjiin_simple..."
        ./test_chunjiin_simple
        echo "========================"
    fi
    
    echo "Test completed!"
else
    echo "Build failed!"
    exit 1
fi 