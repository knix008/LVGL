#!/bin/bash

# Simple script to run the webcam application directly
# This uses GTK+ 3.x only to avoid GTK version conflicts

cd /home/shkwon/Projects/LVGL/CameraApp/Source/build

# Set environment variables for GTK+ 3.x only
export GTK_MODULES=""
export GDK_BACKEND="x11"
export GTK_THEME="Adwaita"

# Force OpenCV to use GTK+ 3.x backend only
export OPENCV_VIDEOIO_PRIORITY_GTK=1
export OPENCV_VIDEOIO_PRIORITY_GTK3=1

# Set library path to prioritize system GTK+ 3.x libraries
export LD_LIBRARY_PATH="/lib/x86_64-linux-gnu:/usr/lib/x86_64-linux-gnu:$LD_LIBRARY_PATH"

# Run the application with GTK+ 3.x only
./webcam_app
