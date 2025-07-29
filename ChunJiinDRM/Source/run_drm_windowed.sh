#!/bin/bash

# Alternative approach: Run DRM application and capture the output

echo "Starting DRM application with windowed display..."

# Method 1: Use Xvfb with DRM
echo "Method 1: Virtual framebuffer with DRM"
echo "Starting virtual display..."

# Create a virtual display
Xvfb :99 -screen 0 1024x768x24 -ac +extension GLX +render -noreset &
XVFB_PID=$!

sleep 2

# Set the display
export DISPLAY=:99

# Run the application
echo "Running DRM application on virtual display..."
cd "$(dirname "$0")/build"

# Start VNC server to view the virtual display
echo "Starting VNC server on port 5900..."
x11vnc -display :99 -nopw -listen localhost -xkb -ncache 10 -ncache_cr -forever &
VNC_PID=$!

sleep 2

echo "=========================================="
echo "DRM Application is running!"
echo "To view it, you can:"
echo "1. Use a VNC viewer: vncviewer localhost:5900"
echo "2. Or open a web browser and go to: http://localhost:5900"
echo "3. Or use any VNC client to connect to localhost:5900"
echo "=========================================="

# Run the main application
sudo DISPLAY=:99 ./main

# Cleanup
echo "Cleaning up..."
kill $VNC_PID 2>/dev/null
kill $XVFB_PID 2>/dev/null

echo "Done!" 