#!/bin/bash

# Run script for Korean Input System
# This script builds and runs the application

# Get the directory where this script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SOURCE_DIR="$SCRIPT_DIR"
BUILD_DIR="$SOURCE_DIR/build"
EXECUTABLE="$BUILD_DIR/main"

# Check if executable exists
if [ ! -f "$EXECUTABLE" ]; then
    echo "Executable not found. Building the project first..."
    "$SOURCE_DIR/build.sh"

    if [ $? -ne 0 ]; then
        echo "Build failed. Cannot run application."
        exit 1
    fi
fi

# Run the application
echo "Running Korean Input System..."
echo "Window size: 320 x 640"
echo "Press Ctrl+C to exit"
echo ""

"$EXECUTABLE"
