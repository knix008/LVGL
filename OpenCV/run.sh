#!/bin/bash

echo "Running YOLOv8 Detection Program..."

# Check if the executable exists
if [ ! -f "build/yolov8n_detection" ]; then
    echo "Error: Executable not found. Please build the program first."
    exit 1
fi

# Run the program
cd build
./yolov8n_detection

echo "Program execution completed."
