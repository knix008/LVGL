# YOLOv8n Webcam Detection

This project provides real-time object detection using YOLOv8n model with webcam input.

## Features

- Real-time object detection from webcam feed
- FPS (Frames Per Second) display
- Video recording capability
- Support for multiple camera indices
- CUDA acceleration support (optional)
- Interactive controls (quit with 'q', save frame with 's')

## Prerequisites

- OpenCV 4.12.0 (CPU-only build)
- YOLOv8n ONNX model
- Webcam access

## Building the Project

1. Build OpenCV and the detection program:
   ```bash
   ./build.sh
   ```

2. Verify the executable was created:
   ```bash
   ls build/yolov8n_video_detection
   ```

## Usage

### Quick Start

Run the interactive test script:
```bash
./test_webcam_yolo.sh
```

### Command Line Options

```bash
./build/yolov8n_video_detection [OPTIONS]
```

**Options:**
- `-w, --webcam INDEX`     Use webcam (default: 0)
- `-m, --model PATH`       YOLOv8n model path (default: models/yolov8n.onnx)
- `-c, --cuda`             Enable CUDA acceleration
- `-f, --fps`              Show FPS counter
- `-s, --save`             Save output video
- `-o, --output PATH`      Output video file path
- `-h, --help`             Show help message

### Examples

1. **Basic webcam detection:**
   ```bash
   ./build/yolov8n_video_detection -w 0 -m models/yolov8n.onnx
   ```

2. **Webcam detection with FPS display:**
   ```bash
   ./build/yolov8n_video_detection -w 0 -m models/yolov8n.onnx -f
   ```

3. **Webcam detection with video recording:**
   ```bash
   ./build/yolov8n_video_detection -w 0 -m models/yolov8n.onnx -f -s -o webcam_output.mp4
   ```

4. **Webcam detection with CUDA acceleration:**
   ```bash
   ./build/yolov8n_video_detection -w 0 -m models/yolov8n.onnx -f -c
   ```

5. **Test different camera index:**
   ```bash
   ./build/yolov8n_video_detection -w 1 -m models/yolov8n.onnx -f
   ```

## Controls

When the webcam detection window is active:
- Press `q` or `ESC` to quit
- Press `s` to save the current frame as an image

## Troubleshooting

### Camera Access Issues

1. **Check camera permissions:**
   ```bash
   ls -l /dev/video*
   ```

2. **Test camera with simple viewer:**
   ```bash
   ./build/webcam_viewer_simple
   ```

3. **List available cameras:**
   ```bash
   v4l2-ctl --list-devices
   ```

### Performance Issues

1. **Reduce resolution:** The default resolution is 640x480. For better performance, you can modify the resolution in the source code.

2. **Use CPU-only build:** The current build is CPU-only. If you have CUDA available, you can enable it with the `-c` flag.

3. **Check system resources:** Monitor CPU and memory usage during detection.

### Model Issues

1. **Verify model file exists:**
   ```bash
   ls -l models/yolov8n.onnx
   ```

2. **Download model if missing:**
   ```bash
   # You may need to download the YOLOv8n ONNX model
   # The model should be placed in the models/ directory
   ```

## File Structure

```
OpenCV/
├── build/                          # Build directory
│   └── yolov8n_video_detection    # Main executable
├── models/
│   └── yolov8n.onnx              # YOLOv8n model
├── src/
│   ├── yolov8n_video_detector.cpp # Video detector implementation
│   └── video_detection_main.cpp   # Main program
├── include/
│   └── yolov8n_video_detector.h  # Header file
├── build.sh                       # Build script
├── test_webcam_yolo.sh           # Test script
└── WEBCAM_YOLO_README.md         # This file
```

## Technical Details

- **Model:** YOLOv8n (nano) ONNX format
- **Input Resolution:** 640x640 (model input)
- **Camera Resolution:** 640x480 (default)
- **FPS:** Target 30 FPS (depends on hardware)
- **Supported Classes:** 80 COCO classes

## Performance Tips

1. **Close unnecessary applications** to free up CPU resources
2. **Use a dedicated GPU** if available (CUDA support)
3. **Reduce camera resolution** for better performance
4. **Monitor system temperature** during extended use

## Output Files

- **Video output:** `webcam_output.mp4` (when using `-s` flag)
- **Frame captures:** `webcam_frame_*.jpg` (when pressing 's' during detection)
