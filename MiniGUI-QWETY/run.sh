#!/bin/bash

# Korean Input Application Runner
# This script runs the Korean QWERTY input application

echo "Starting Korean QWERTY Input Application..."

# Check if the application exists
if [ ! -f "./korean_input" ]; then
    echo "Error: korean_input executable not found!"
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
export LD_LIBRARY_PATH="./install/lib:$LD_LIBRARY_PATH"

echo "Environment configured:"
echo "  LD_LIBRARY_PATH: $LD_LIBRARY_PATH"
echo ""

echo "Starting Korean Input Application..."
echo "Press Ctrl+C to exit the application."
echo ""

# Run the application
./korean_input

echo ""
echo "Korean Input Application has exited."
