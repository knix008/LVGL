#!/bin/bash

# MiniGUI Application Run Script
# This script sets up the environment and runs the MiniGUI Hello World application

# Set library path
export LD_LIBRARY_PATH="$HOME/minigui-local/usr/local/lib:$LD_LIBRARY_PATH"

# Set MiniGUI runtime mode to standalone
export MG_RUNTIME_MODE="standalone"

# Force MiniGUI to use pc_xvfb (PC Virtual FrameBuffer) engine
export MG_GAL_ENGINE="pc_xvfb"
export MG_IAL_ENGINE="pc_xvfb"

# Set default display mode
export MG_DEFAULTMODE="800x600-16bpp"

# Set configuration file path
export MG_CONFIG_FILE="./MiniGUI.cfg"

# Create a virtual display if X11 is not available
if [ -z "$DISPLAY" ]; then
    echo "No X11 display found. Starting Xvfb virtual display..."
    export DISPLAY=":99"
    Xvfb :99 -screen 0 800x600x16 &
    XVFB_PID=$!
    sleep 2
fi

# Get the directory where this script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Check if the executable exists
if [ ! -f "helloworld" ]; then
    echo "Error: helloworld executable not found. Please run 'make' first."
    exit 1
fi

# Check if the configuration file exists
if [ ! -f "MiniGUI.cfg" ]; then
    echo "Error: MiniGUI.cfg configuration file not found."
    exit 1
fi

echo "Starting MiniGUI Hello World application..."
echo "Library path: $LD_LIBRARY_PATH"
echo "GAL engine: $MG_GAL_ENGINE"
echo "IAL engine: $MG_IAL_ENGINE"
echo "Display: $DISPLAY"

# Run the application
./helloworld

# Clean up Xvfb if we started it
if [ ! -z "$XVFB_PID" ]; then
    echo "Stopping virtual display..."
    kill $XVFB_PID
fi

echo "Application finished."
