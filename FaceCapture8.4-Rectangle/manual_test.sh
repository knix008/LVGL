#!/bin/bash

echo "=== Manual Window Close Test Instructions ==="
echo ""
echo "This test will start the camera application in the foreground."
echo "Please follow these steps:"
echo ""
echo "1. The application window will open"
echo "2. Try clicking the X button (close button) on the window"
echo "3. Observe the terminal output to see if cleanup messages appear"
echo "4. If the window doesn't close, try Alt+F4 or Ctrl+C"
echo ""
echo "Expected behavior:"
echo "- Window should close when clicking X button"
echo "- Terminal should show cleanup messages"
echo "- Process should exit cleanly"
echo ""
echo "Starting application in 3 seconds..."
echo "Press Ctrl+C if you want to cancel..."
sleep 3

echo "Starting camera application..."
echo "===========================================" 
./camera