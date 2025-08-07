#!/bin/bash

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Configure and build
cmake ..
make -j$(nproc)

echo "Build completed! Run the program with: ./yolo_detection" 