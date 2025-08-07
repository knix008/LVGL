#!/bin/bash

# YOLOv8n Video Detection Test Script
# This script demonstrates various ways to use the video detection functionality

echo "YOLOv8n Video Detection Test Script"
echo "==================================="
echo

# Check if executables exist
if [ ! -f "./yolov8n_video_detection" ]; then
    echo "Error: yolov8n_video_detection executable not found!"
    echo "Please build the project first:"
    echo "  mkdir build && cd build"
    echo "  cmake .. && make"
    exit 1
fi

echo "✓ Video detection executable found"
echo

# Test 1: Show help
echo "Test 1: Help message"
echo "-------------------"
./yolov8n_video_detection --help
echo

# Test 2: Process video file (short test)
echo "Test 2: Process video file (10 seconds)"
echo "----------------------------------------"
echo "Processing traffic.mp4 for 10 seconds..."
timeout 10s ./yolov8n_video_detection -v ../data/traffic.mp4 -f
echo

# Test 3: Process video file with output
echo "Test 3: Process video file with output"
echo "--------------------------------------"
echo "Processing traffic.mp4 and saving output..."
timeout 15s ./yolov8n_video_detection -v ../data/traffic.mp4 -o test_output.mp4 -f -s
echo

# Test 4: Test webcam (should fail gracefully)
echo "Test 4: Test webcam (should fail gracefully)"
echo "--------------------------------------------"
timeout 5s ./yolov8n_video_detection -w 0
echo

# Test 5: Test with CUDA flag (should work on CPU)
echo "Test 5: Test with CUDA flag"
echo "---------------------------"
timeout 10s ./yolov8n_video_detection -v ../data/traffic.mp4 -c -f
echo

echo "All tests completed!"
echo
echo "Usage examples:"
echo "  # Process video file"
echo "  ./yolov8n_video_detection -v ../data/traffic.mp4 -o output.mp4"
echo
echo "  # Use webcam (if available)"
echo "  ./yolov8n_video_detection -w 0"
echo
echo "  # Process with CUDA acceleration"
echo "  ./yolov8n_video_detection -v ../data/traffic.mp4 -c"
echo
echo "  # Show FPS counter"
echo "  ./yolov8n_video_detection -v ../data/traffic.mp4 -f"
