#!/bin/bash

# Build script for SVG test
# This script builds and runs the SVG test

echo "Building SVG test..."

# Create build directory if it doesn't exist
mkdir -p build

# Change to build directory
cd build

# Run CMake and make
cmake .. && make svg_test

# Check if build was successful
if [ $? -eq 0 ]; then
    echo "SVG test built successfully!"
    echo "Running SVG test..."
    ./svg_test
else
    echo "Build failed!"
    exit 1
fi 