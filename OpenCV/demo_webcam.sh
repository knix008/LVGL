#!/bin/bash

# Webcam Viewer Demo Script
# 웹캠 뷰어 데모 스크립트

echo "🎥 Webcam Viewer Demo"
echo "====================="
echo ""

# Check if webcam_viewer exists
if [ ! -f "./webcam_viewer" ]; then
    echo "❌ Error: webcam_viewer executable not found!"
    echo "Please build the project first:"
    echo "  cmake .. && make -j\$(nproc)"
    exit 1
fi

echo "✅ webcam_viewer found!"
echo ""

# Function to show usage
show_usage() {
    echo "Available demo options:"
    echo "  1) Basic webcam viewer (640x480)"
    echo "  2) High resolution webcam (1280x720)"
    echo "  3) Custom title webcam"
    echo "  4) Recording demo"
    echo "  5) Different camera index"
    echo "  6) Show help"
    echo "  0) Exit"
    echo ""
}

# Function to run webcam viewer
run_webcam() {
    local args="$1"
    local description="$2"
    
    echo "🎬 Running: $description"
    echo "Command: ./webcam_viewer $args"
    echo "Press ESC or Q to quit, R to toggle recording"
    echo ""
    
    ./webcam_viewer $args
}

# Main menu
while true; do
    show_usage
    read -p "Select demo option (0-6): " choice
    
    case $choice in
        1)
            run_webcam "" "Basic webcam viewer (640x480)"
            ;;
        2)
            run_webcam "-w 1280 -h 720" "High resolution webcam (1280x720)"
            ;;
        3)
            run_webcam "-t 'Custom Webcam Viewer'" "Custom title webcam"
            ;;
        4)
            echo "🎬 Running: Recording demo"
            echo "Command: ./webcam_viewer -r demo_recording.mp4"
            echo "Press ESC or Q to quit, R to toggle recording"
            echo ""
            ./webcam_viewer -r demo_recording.mp4
            ;;
        5)
            echo "🎬 Running: Different camera index"
            echo "Command: ./webcam_viewer -c 1"
            echo "Press ESC or Q to quit, R to toggle recording"
            echo ""
            ./webcam_viewer -c 1
            ;;
        6)
            ./webcam_viewer --help
            ;;
        0)
            echo "👋 Goodbye!"
            exit 0
            ;;
        *)
            echo "❌ Invalid option. Please select 0-6."
            ;;
    esac
    
    echo ""
    echo "Demo completed. Select another option or 0 to exit."
    echo ""
done
