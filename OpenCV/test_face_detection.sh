#!/bin/bash

echo "YOLOv8 Face Detection Test"
echo "=========================="

# Check if the executable exists
if [ ! -f "build/face_detection" ]; then
    echo "Error: Executable not found. Please run ./build.sh first."
    exit 1
fi

# Check if model exists
if [ ! -f "models/yolov8_face_model.pt" ]; then
    echo "Error: YOLOv8 face model not found at models/yolov8_face_model.pt"
    exit 1
fi

echo "Available options:"
echo "1. Basic face detection webcam (camera 0)"
echo "2. Face detection webcam with FPS display"
echo "3. Face detection webcam with video recording"
echo "4. Face detection webcam with CUDA (if available)"
echo "5. Test different camera index"
echo "6. Process video file with face detection"

read -p "Select option (1-6): " choice

case $choice in
    1)
        echo "Running basic face detection webcam..."
        ./build/face_detection -w 0 -m models/yolov8_face_model.pt
        ;;
    2)
        echo "Running face detection webcam with FPS display..."
        ./build/face_detection -w 0 -m models/yolov8_face_model.pt -f
        ;;
    3)
        echo "Running face detection webcam with video recording..."
        ./build/face_detection -w 0 -m models/yolov8_face_model.pt -f -s -o face_webcam_output.mp4
        ;;
    4)
        echo "Running face detection webcam with CUDA acceleration..."
        ./build/face_detection -w 0 -m models/yolov8_face_model.pt -f -c
        ;;
    5)
        read -p "Enter camera index (0, 1, 2, etc.): " camera_index
        echo "Testing camera index $camera_index for face detection..."
        ./build/face_detection -w $camera_index -m models/yolov8_face_model.pt -f
        ;;
    6)
        read -p "Enter video file path: " video_path
        if [ -f "$video_path" ]; then
            echo "Processing video file for face detection..."
            ./build/face_detection -v "$video_path" -m models/yolov8_face_model.pt -f -s -o face_detection_output.mp4
        else
            echo "Error: Video file not found: $video_path"
            exit 1
        fi
        ;;
    *)
        echo "Invalid option. Running basic face detection webcam..."
        ./build/face_detection -w 0 -m models/yolov8_face_model.pt
        ;;
esac
