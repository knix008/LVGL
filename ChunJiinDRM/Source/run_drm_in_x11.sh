#!/bin/bash

# Script to run DRM application in virtual framebuffer and display in X11 window

echo "Starting DRM application in virtual framebuffer..."

# Set display for virtual framebuffer
export DISPLAY=:99

# Start virtual framebuffer in background
echo "Starting virtual framebuffer on display :99..."
Xvfb :99 -screen 0 1024x768x24 -ac +extension GLX +render -noreset &
XVFB_PID=$!

# Wait a moment for Xvfb to start
sleep 2

# Check if Xvfb started successfully
if ! kill -0 $XVFB_PID 2>/dev/null; then
    echo "Failed to start virtual framebuffer"
    exit 1
fi

echo "Virtual framebuffer started successfully"

# Start VNC server to expose the virtual display
echo "Starting VNC server..."
x11vnc -display :99 -nopw -listen localhost -xkb -ncache 10 -ncache_cr -forever &
VNC_PID=$!

# Wait for VNC to start
sleep 2

# Start VNC viewer in background (optional - you can also use other VNC clients)
echo "Starting VNC viewer..."
# You can uncomment the line below if you have a VNC viewer installed
# vncviewer localhost:5900 &

# Run the DRM application
echo "Running DRM application..."
cd "$(dirname "$0")/build"
sudo DISPLAY=:99 ./main

# Cleanup
echo "Cleaning up..."
kill $VNC_PID 2>/dev/null
kill $XVFB_PID 2>/dev/null

echo "Done!" 