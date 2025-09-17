#!/bin/bash

# Static Build Runner Script
# This script runs the hybrid static build with proper library paths

# Set the LD_LIBRARY_PATH to prioritize system libraries
export LD_LIBRARY_PATH="/lib/x86_64-linux-gnu:/usr/lib/x86_64-linux-gnu:/home/shkwon/Projects/LVGL/CameraApp/Source/opencv/lib:/home/shkwon/Projects/LVGL/CameraApp/Source/onnxruntime-linux-x64-1.16.3/lib:$LD_LIBRARY_PATH"

# Navigate to the build directory and execute the application
cd /home/shkwon/Projects/LVGL/CameraApp/Source/build
./webcam_app "$@"
