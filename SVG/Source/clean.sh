#!/bin/bash

# Clean script to remove build artifacts
echo "Cleaning build artifacts..."

# Remove LVGL library build directory
if [ -d "lvgl_build" ]; then
    echo "Removing lvgl_build directory..."
    rm -rf lvgl_build
fi

# Remove main application build directory
if [ -d "build" ]; then
    echo "Removing build directory..."
    rm -rf build
fi

# Remove library files
if [ -f "lib/liblvgl.a" ]; then
    echo "Removing lib/liblvgl.a..."
    rm -f lib/liblvgl.a
fi

echo "Clean completed!"
