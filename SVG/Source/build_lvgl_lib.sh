#!/bin/bash

# Build script for LVGL library
echo "Building LVGL library..."

# Create build directory for LVGL library
mkdir -p lvgl_build
cd lvgl_build

# Configure and build LVGL library
cmake ../lvgl
make -j$(nproc)

# Check if build was successful
if [ $? -eq 0 ]; then
    echo "LVGL library built successfully!"
    echo "Library location: ../lib/liblvgl.a"
else
    echo "Failed to build LVGL library!"
    exit 1
fi

cd ..
