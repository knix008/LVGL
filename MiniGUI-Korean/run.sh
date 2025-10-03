#!/bin/bash

# Korean Font Display Application Runner
# This script runs the Korean font display application

echo "Starting Korean Font Display Application..."

# Check if the application exists
if [ ! -f "./korean_display" ]; then
    echo "Error: korean_display executable not found!"
    echo "Please run 'make' first to build the application."
    exit 1
fi

# Check if MiniGUI is installed locally
if [ ! -f "./install/lib/libminigui_sa.so" ]; then
    echo "Error: MiniGUI not found locally!"
    echo "Please run the build script first to install MiniGUI."
    exit 1
fi

# Set up environment variables for MiniGUI
export LD_LIBRARY_PATH="./install/lib"
export FREETYPE_FONT_PATH="./install/share/fonts"
export MG_RUNTIME_MODE="standalone"
export MG_GAL_ENGINE="pc_xvfb"
export MG_IAL_ENGINE="pc_xvfb"
export MG_DEFAULTMODE="800x600-16bpp"
export MG_CONFIG_FILE="./MiniGUI.cfg"

echo "Environment configured:"
echo "  LD_LIBRARY_PATH: $LD_LIBRARY_PATH"
echo "  FREETYPE_FONT_PATH: $FREETYPE_FONT_PATH"
echo "  MG_RUNTIME_MODE: $MG_RUNTIME_MODE"
echo "  MG_GAL_ENGINE: $MG_GAL_ENGINE"
echo "  MG_IAL_ENGINE: $MG_IAL_ENGINE"
echo "  MG_DEFAULTMODE: $MG_DEFAULTMODE"
echo "  MG_CONFIG_FILE: $MG_CONFIG_FILE"
echo ""

echo "Starting Korean Font Display Application..."
echo "Press Ctrl+C to exit the application."
echo ""

# Run the application
./korean_display

echo ""
echo "Korean Font Display Application has exited."
