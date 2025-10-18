#!/bin/bash

# IP Address GUI - Run Script
# Make sure you're in a graphical environment (X11/Wayland)

echo "=========================================="
echo "IP Address Input GUI"
echo "=========================================="
echo ""

# Check if executable exists
if [ ! -f "./ip_address" ]; then
    echo "Executable not found. Building..."
    make
    if [ $? -ne 0 ]; then
        echo "Error: Build failed"
        exit 1
    fi
    echo ""
fi

# Check if DISPLAY is set (for X11)
if [ -z "$DISPLAY" ]; then
    echo "Warning: DISPLAY environment variable is not set."
    echo "Make sure you're running this in a graphical environment."
    echo ""
fi

# Run the application
echo "Starting IP Address Input GUI..."
echo "A 320x640 window should appear."
echo ""
echo "How to use:"
echo "  1. Click on the white input area to show/hide keyboard"
echo "  2. Use the toggle switch to change between IPv4/IPv6 modes"
echo "  3. Enter an IP address using the on-screen keyboard"
echo "  4. Click 'Validate' to check the address"
echo "  5. Click 'Clear' to reset"
echo ""
echo "Press Ctrl+C to exit"
echo ""

./ip_address
