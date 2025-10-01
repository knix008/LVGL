#!/bin/bash

echo "========================================="
echo "ChunJiIn Korean Character Display Test"
echo "========================================="
echo ""
echo "Setting up environment..."

# Set up environment variables
export LD_LIBRARY_PATH="./install/lib:$LD_LIBRARY_PATH"
export MG_CONFIG_FILE="./MiniGUI.cfg"
export MG_RUNTIME_MODE="standalone"
export MG_GAL_ENGINE="pc_xvfb"
export MG_IAL_ENGINE="pc_xvfb"

echo "Environment configured:"
echo "  MG_CONFIG_FILE: $MG_CONFIG_FILE"
echo "  LD_LIBRARY_PATH: $LD_LIBRARY_PATH"
echo ""

echo "Checking font files..."
if [ -f "./install/share/fonts/NanumGothic-Regular.ttf" ]; then
    echo "  ✓ NanumGothic-Regular.ttf found"
else
    echo "  ✗ NanumGothic-Regular.ttf NOT found"
fi

if [ -f "./install/share/fonts/NanumGothic-Bold.ttf" ]; then
    echo "  ✓ NanumGothic-Bold.ttf found"
else
    echo "  ✗ NanumGothic-Bold.ttf NOT found"
fi

echo ""
echo "Starting ChunJiIn application..."
echo "Watch for font loading messages below:"
echo "========================================="
echo ""

# Run the application
./chunjiin_app

echo ""
echo "Application exited."
