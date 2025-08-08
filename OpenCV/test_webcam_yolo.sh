#!/bin/bash

echo "YOLOv8n Webcam Detection Test"
echo "============================="

# Check if the executable exists
if [ ! -f "build/yolov8n_video_detection" ]; then
    echo "Error: Executable not found. Please run ./build.sh first."
    exit 1
fi

# Check if model exists
if [ ! -f "models/yolov8n.onnx" ]; then
    echo "Error: YOLOv8n model not found at models/yolov8n.onnx"
    exit 1
fi

echo "Available options:"
echo "1. Basic webcam detection (camera 0)"
echo "2. Webcam detection with FPS display"
echo "3. Webcam detection with video recording"
echo "4. Webcam detection with CUDA (if available)"
echo "5. Test different camera index"

read -p "Select option (1-5): " choice

case $choice in
    1)
        echo "Running basic webcam detection..."
        ./build/yolov8n_video_detection -w 0 -m models/yolov8n.onnx
        ;;
    2)
        echo "Running webcam detection with FPS display..."
        ./build/yolov8n_video_detection -w 0 -m models/yolov8n.onnx -f
        ;;
    3)
        echo "Running webcam detection with video recording..."
        ./build/yolov8n_video_detection -w 0 -m models/yolov8n.onnx -f -s -o webcam_output.mp4
        ;;
    4)
        echo "Running webcam detection with CUDA acceleration..."
        ./build/yolov8n_video_detection -w 0 -m models/yolov8n.onnx -f -c
        ;;
    5)
        read -p "Enter camera index (0, 1, 2, etc.): " camera_index
        echo "Testing camera index $camera_index..."
        ./build/yolov8n_video_detection -w $camera_index -m models/yolov8n.onnx -f
        ;;
    *)
        echo "Invalid option. Running basic webcam detection..."
        ./build/yolov8n_video_detection -w 0 -m models/yolov8n.onnx
        ;;
esac
