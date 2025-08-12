#!/bin/bash

# Build script for LVGL library
echo "Building LVGL library..."

# Create build directory for LVGL library
mkdir -p lvgl_build
cd lvgl_build

# Set environment variables to force use of custom lv_conf.h
export LV_CONF_INCLUDE_SIMPLE=1
export LV_CONF_SKIP=1

# Configure and build LVGL library with custom lv_conf.h
cmake ../lvgl -DCMAKE_C_FLAGS="-DLV_CONF_INCLUDE_SIMPLE=1 -DLV_CONF_SKIP=1 -I../include" -DCMAKE_CXX_FLAGS="-DLV_CONF_INCLUDE_SIMPLE=1 -DLV_CONF_SKIP=1 -I../include"
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
