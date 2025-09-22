#!/bin/bash

# MiniGUI Application Run Script (Quiet Mode)
# This script runs the MiniGUI Hello World application with suppressed warnings

# Set library path
export LD_LIBRARY_PATH="$HOME/minigui-local/usr/local/lib:$LD_LIBRARY_PATH"

# Suppress all warnings and messages
export GTK_MODULES=""
export PNG_SKIP_sRGB_CHECK=1
export GTK_DEBUG="no-css-cache"
export MALLOC_CHECK_=0

# Set MiniGUI runtime mode to standalone
export MG_RUNTIME_MODE="standalone"

# Force MiniGUI to use pc_xvfb (PC Virtual FrameBuffer) engine
export MG_GAL_ENGINE="pc_xvfb"
export MG_IAL_ENGINE="pc_xvfb"

# Set default display mode
export MG_DEFAULTMODE="800x600-16bpp"

# Set configuration file path
export MG_CONFIG_FILE="./MiniGUI.cfg"

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

echo "Starting MiniGUI Hello World application (quiet mode)..."

# Run the application with stderr redirected to suppress warnings
./helloworld 2>/dev/null

echo "Application finished."
