#!/bin/bash

# Chunjiin Keyboard Build Script

set -e

echo "=== Chunjiin Korean Keyboard Build Script ==="

# Check if LVGL exists
if [ ! -d "lvgl" ]; then
    echo "LVGL not found. Cloning LVGL repository..."
    git clone --depth 1 --branch release/v9.3 https://github.com/lvgl/lvgl.git
    echo "LVGL cloned successfully"
fi

# Create build directory
mkdir -p build
cd build

# Run CMake
echo "Running CMake..."
cmake ..

# Build
echo "Building..."
make -j$(nproc)

echo ""
echo "=== Build Complete ==="
echo "Executable: build/chunjiin_keyboard"
echo "Run with: ./run.sh"
