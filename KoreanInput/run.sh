#!/bin/bash

# Run the LVGL Korean Tab application
cd Source/build

# Check if Makefile exists, if not run cmake
if [ ! -f "Makefile" ]; then
    echo "Makefile not found, running cmake..."
    cmake ..
fi

# Clean and rebuild to ensure source changes are applied
echo "Cleaning previous build..."
make clean-all 2>/dev/null || make clean 2>/dev/null || echo "No previous build to clean"

echo "Building application..."
make -j$(nproc)

if [ $? -eq 0 ]; then
    echo "Build successful! Running application..."
    ./lvgl_main
else
    echo "Build failed!"
    exit 1
fi 