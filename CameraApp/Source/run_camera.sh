#!/bin/bash

# Camera Webcam Application Runner
# This script runs the webcam application with real camera but handles IPC errors gracefully

# Set the LD_LIBRARY_PATH to prioritize system libraries and avoid conflicts
export LD_LIBRARY_PATH="/lib/x86_64-linux-gnu:/usr/lib/x86_64-linux-gnu:/home/shkwon/Projects/LVGL/CameraApp/Source/opencv/lib:/home/shkwon/Projects/LVGL/CameraApp/Source/onnxruntime-linux-x64-1.16.3/lib:$LD_LIBRARY_PATH"

# Remove any existing socket file to avoid conflicts
rm -f /tmp/webcam_gui_socket

# Navigate to the build directory
cd /home/shkwon/Projects/LVGL/CameraApp/Source/build

echo "Starting Webcam Application with Real Camera..."
echo "Note: IPC errors are expected since no GUI application is running"
echo "The application will still work for face detection"
echo "Press Ctrl+C to stop"
echo ""

# Run the application (IPC errors will be shown but won't stop execution)
./webcam_app "$@" 2>/dev/null || {
    echo ""
    echo "Camera access failed. Trying simulation mode..."
    ./webcam_app --simulation "$@"
}
