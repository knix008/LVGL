#!/bin/bash

# Webcam Viewer Test Script
# 웹캠 뷰어 테스트 스크립트

echo "🎥 Webcam Viewer Test"
echo "===================="
echo ""

# Check if webcam devices exist
echo "📹 Checking webcam devices..."
if ls /dev/video* 2>/dev/null; then
    echo "✅ Webcam devices found!"
    echo ""
else
    echo "❌ No webcam devices found!"
    exit 1
fi

# Check if webcam_viewer exists
if [ ! -f "./webcam_viewer" ]; then
    echo "❌ Error: webcam_viewer executable not found!"
    echo "Please build the project first:"
    echo "  cmake .. && make -j\$(nproc)"
    exit 1
fi

echo "✅ webcam_viewer found!"
echo ""

# Function to show test options
show_options() {
    echo "Available test options:"
    echo "  1) Basic webcam viewer (camera 0, 640x480)"
    echo "  2) High resolution webcam (camera 0, 1280x720)"
    echo "  3) External camera (camera 1, 1280x720)"
    echo "  4) Recording test (camera 0 with recording)"
    echo "  5) Custom title test"
    echo "  6) Show webcam info"
    echo "  7) Test both cameras"
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

# Function to show webcam info
show_webcam_info() {
    echo "📹 Webcam Information:"
    echo "======================"
    
    # List video devices
    echo "Available video devices:"
    ls -la /dev/video*
    echo ""
    
    # Check camera capabilities (if v4l2-ctl is available)
    if command -v v4l2-ctl &> /dev/null; then
        echo "Camera 0 capabilities:"
        v4l2-ctl --device=/dev/video0 --list-formats-ext 2>/dev/null || echo "Could not get camera 0 info"
        echo ""
        
        if [ -e /dev/video1 ]; then
            echo "Camera 1 capabilities:"
            v4l2-ctl --device=/dev/video1 --list-formats-ext 2>/dev/null || echo "Could not get camera 1 info"
            echo ""
        fi
    else
        echo "v4l2-ctl not available. Install with: sudo apt install v4l-utils"
    fi
}

# Main menu
while true; do
    show_options
    read -p "Select test option (0-7): " choice
    
    case $choice in
        1)
            run_webcam "-c 0 -w 640 -h 480" "Basic webcam viewer (camera 0, 640x480)"
            ;;
        2)
            run_webcam "-c 0 -w 1280 -h 720" "High resolution webcam (camera 0, 1280x720)"
            ;;
        3)
            run_webcam "-c 1 -w 1280 -h 720" "External camera (camera 1, 1280x720)"
            ;;
        4)
            echo "🎬 Running: Recording test"
            echo "Command: ./webcam_viewer -c 0 -r webcam_recording.mp4"
            echo "Press ESC or Q to quit, R to toggle recording"
            echo ""
            ./webcam_viewer -c 0 -r webcam_recording.mp4
            ;;
        5)
            run_webcam "-c 0 -t 'Custom Webcam Title'" "Custom title test"
            ;;
        6)
            show_webcam_info
            ;;
        7)
            echo "🎬 Testing both cameras..."
            echo "First, testing camera 0..."
            ./webcam_viewer -c 0 -t "Camera 0 Test" &
            sleep 5
            echo "Now testing camera 1..."
            ./webcam_viewer -c 1 -t "Camera 1 Test" &
            echo "Both cameras are running. Close windows to stop."
            ;;
        0)
            echo "👋 Goodbye!"
            exit 0
            ;;
        *)
            echo "❌ Invalid option. Please select 0-7."
            ;;
    esac
    
    echo ""
    echo "Test completed. Select another option or 0 to exit."
    echo ""
done
