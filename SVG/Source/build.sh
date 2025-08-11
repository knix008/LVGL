#!/bin/bash

# Main build script for the entire project
echo "Starting build process..."

# Step 1: Build LVGL library
echo "Step 1: Building LVGL library..."
./build_lvgl_lib.sh

if [ $? -ne 0 ]; then
    echo "Failed to build LVGL library. Exiting."
    exit 1
fi

# Step 2: Build main application
echo "Step 2: Building main application..."

# Create build directory for main application
mkdir -p build
cd build

# Configure and build main application
cmake ..
make -j$(nproc)

# Check if build was successful
if [ $? -eq 0 ]; then
    echo "Build completed successfully!"
    echo "Executable location: ./lvgl_main"
else
    echo "Failed to build main application!"
    exit 1
fi

cd ..
