# YOLOv8 Face Detection

This project provides real-time face detection using YOLOv8 face model with webcam and video input.

## Features

- Real-time face detection from webcam feed
- Video file processing with face detection
- FPS (Frames Per Second) display
- Video recording capability
- Support for multiple camera indices
- CUDA acceleration support (optional)
- Interactive controls (quit with 'q', save frame with 's')
- Blue bounding boxes for detected faces

## Prerequisites

- OpenCV 4.12.0 (CPU-only build)
- YOLOv8 Face PyTorch model (converted to ONNX)
- Webcam access
- Python 3.x with ultralytics (for model conversion)

## Building the Project

1. Build OpenCV and the detection programs:
   ```bash
   ./build.sh
   ```

2. Convert the PyTorch face model to ONNX format:
   ```bash
   python3 convert_face_model.py
   ```

3. Verify the executables were created:
   ```bash
   ls build/face_detection
   ```

## Usage

### Quick Start

Run the interactive test script:
```bash
./test_face_detection.sh
```

### Command Line Options

```bash
./build/face_detection [OPTIONS]
```

**Options:**
- `-w, --webcam INDEX`     Use webcam (default: 0)
- `-m, --model PATH`       YOLOv8 face model path (default: models/yolov8_face_model.pt)
- `-c, --cuda`             Enable CUDA acceleration
- `-f, --fps`              Show FPS counter
- `-s, --save`             Save output video
- `-o, --output PATH`      Output video file path
- `-h, --help`             Show help message

### Examples

1. **Basic face detection webcam:**
   ```bash
   ./build/face_detection -w 0 -m models/yolov8_face_model.pt
   ```

2. **Face detection webcam with FPS display:**
   ```bash
   ./build/face_detection -w 0 -m models/yolov8_face_model.pt -f
   ```

3. **Face detection webcam with video recording:**
   ```bash
   ./build/face_detection -w 0 -m models/yolov8_face_model.pt -f -s -o face_webcam_output.mp4
   ```

4. **Face detection webcam with CUDA acceleration:**
   ```bash
   ./build/face_detection -w 0 -m models/yolov8_face_model.pt -f -c
   ```

5. **Test different camera index:**
   ```bash
   ./build/face_detection -w 1 -m models/yolov8_face_model.pt -f
   ```

6. **Process video file with face detection:**
   ```bash
   ./build/face_detection -v data/traffic.mp4 -m models/yolov8_face_model.pt -f -s -o face_detection_output.mp4
   ```

## Controls

When the face detection window is active:
- Press `q` or `ESC` to quit
- Press `s` to save the current frame as an image

## Model Conversion

The face detection system automatically handles PyTorch model conversion:

1. **Automatic Conversion:** The system will automatically convert `.pt` models to `.onnx` format
2. **Manual Conversion:** If automatic conversion fails, use the provided script:
   ```bash
   python3 convert_face_model.py
   ```

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

### Model Issues

1. **Verify model files exist:**
   ```bash
   ls -l models/yolov8_face_model.*
   ```

2. **Manual model conversion:**
   ```bash
   # Install ultralytics if not already installed
   pip install ultralytics
   
   # Convert model
   python3 convert_face_model.py
   ```

### Performance Issues

1. **Reduce resolution:** The default resolution is 640x480. For better performance, you can modify the resolution in the source code.

2. **Use CPU-only build:** The current build is CPU-only. If you have CUDA available, you can enable it with the `-c` flag.

3. **Check system resources:** Monitor CPU and memory usage during detection.

## File Structure

```
OpenCV/
├── build/                          # Build directory
│   └── face_detection             # Face detection executable
├── models/
│   ├── yolov8_face_model.pt      # Original PyTorch model
│   └── yolov8_face_model.onnx    # Converted ONNX model
├── src/
│   ├── yolo_face_detector.cpp     # Face detector implementation
│   ├── yolo_face_video_detector.cpp # Face video detector
│   └── face_detection_main.cpp    # Main program
├── include/
│   ├── yolo_face_detector.h       # Face detector header
│   └── yolo_face_video_detector.h # Face video detector header
├── build.sh                       # Build script
├── convert_face_model.py          # Model conversion script
├── test_face_detection.sh         # Test script
└── FACE_DETECTION_README.md       # This file
```

## Technical Details

- **Model:** YOLOv8 Face Detection (PyTorch/ONNX format)
- **Input Resolution:** 640x640 (model input)
- **Camera Resolution:** 640x480 (default)
- **FPS:** Target 30 FPS (depends on hardware)
- **Detection Class:** Face only
- **Bounding Box Color:** Blue (RGB: 255, 0, 0)

## Performance Tips

1. **Close unnecessary applications** to free up CPU resources
2. **Use a dedicated GPU** if available (CUDA support)
3. **Reduce camera resolution** for better performance
4. **Monitor system temperature** during extended use

## Output Files

- **Video output:** `face_webcam_output.mp4` (when using `-s` flag)
- **Frame captures:** `face_detection_frame_*.jpg` (when pressing 's' during detection)

## Comparison with General Object Detection

| Feature | General YOLO | Face Detection |
|---------|--------------|----------------|
| Classes | 80 COCO classes | Face only |
| Model Size | ~12MB | ~6MB |
| Speed | Slower | Faster |
| Accuracy | Good for general objects | Optimized for faces |
| Use Case | General object detection | Face detection only |

## Integration with Existing System

The face detection system is designed to work alongside the existing general object detection system:

- **General Object Detection:** `./build/yolov8n_video_detection`
- **Face Detection:** `./build/face_detection`

Both systems share the same infrastructure but use different models and detection classes.
