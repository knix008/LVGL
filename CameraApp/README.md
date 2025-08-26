# Webcam Application with AI Face Detection

A real-time webcam application that uses YOLOv8 for AI-powered face detection, built with OpenCV and ONNX Runtime.

## Features

- 📹 **Real-time webcam capture** with automatic camera detection
- 🤖 **AI face detection** using YOLOv8 neural network
- ⚡ **High-performance inference** with ONNX Runtime
- 🟢 **Visual feedback** with bounding boxes around detected faces
- 📊 **Confidence scoring** for each detection
- 🔧 **Configurable parameters** (confidence threshold, NMS, etc.)
- 🎮 **Simulation mode** for testing without camera
- 🛠️ **Background processing** support
- 📝 **Comprehensive logging** and monitoring

## Requirements

### System Dependencies
- **Linux** (Ubuntu 20.04+ recommended)
- **OpenCV 4.0+** (`libopencv-dev`)
- **CMake 3.10+**
- **Make**
- **pkg-config**

### AI Model
- **YOLOv8 Face Detection Model** (`yolov8n-face.onnx`)

## Installation

### 1. Install System Dependencies

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    libopencv-dev \
    libgtk-3-dev \
    libavcodec-dev \
    libavformat-dev \
    libswscale-dev \
    libv4l-dev \
    libxvidcore-dev \
    libx264-dev \
    libjpeg-dev \
    libpng-dev \
    libtiff-dev \
    libatlas-base-dev \
    gfortran \
    wget \
    unzip
```

### 2. Download AI Model

The application expects the YOLOv8 face detection model at `Source/models/yolov8n-face.onnx`.

If you don't have the model, you can convert it from PyTorch:

```bash
cd Source
python3 convert_yolo_to_onnx.py --input yolov8n-face.pt --output models/yolov8n-face.onnx
```

### 3. Build the Application

```bash
# Quick build and run
./run.sh

# Or build only
./run.sh build

# Or use the build script directly
cd Source
./build_webcam.sh build
```

## Usage

### Basic Usage

```bash
# Run with default settings
./run.sh

# Or run directly
cd Source/build
./webcam_app
```

### Advanced Usage

```bash
# Specify model path
./webcam_app --model models/yolov8n-face.onnx

# Use specific camera
./webcam_app --camera 0

# Adjust confidence threshold
./webcam_app --confidence 0.5

# Run in simulation mode
./webcam_app --simulation

# Custom input dimensions
./webcam_app --width 640 --height 640

# Show help
./webcam_app --help
```

### Command Line Options

| Option | Description | Default |
|--------|-------------|---------|
| `--model <path>` | Path to YOLOv8 model | `models/yolov8n-face.onnx` |
| `--camera <index>` | Camera device index | Auto-detect |
| `--confidence <val>` | Confidence threshold (0.0-1.0) | `0.25` |
| `--nms <val>` | NMS threshold (0.0-1.0) | `0.4` |
| `--width <pixels>` | Input width | `640` |
| `--height <pixels>` | Input height | `640` |
| `--simulation` | Run in simulation mode | `false` |
| `--help, -h` | Show help message | - |
| `--version, -v` | Show version information | - |

### Background Processing

```bash
# Start in background
./run.sh background

# Check status
./run.sh status

# Stop background process
./run.sh stop
```

## Project Structure

```
CameraApp/
├── README.md                 # This file
├── run.sh                   # Main runner script
├── Source/
│   ├── CMakeLists.txt       # CMake configuration
│   ├── build_webcam.sh      # Build script
│   ├── include/
│   │   └── webcam.h         # Main header file
│   ├── src/
│   │   ├── main.cpp         # Application entry point
│   │   └── webcam.cpp       # Core implementation
│   ├── models/
│   │   ├── yolov8n-face.onnx # AI model
│   │   └── yolov8n-face.pt   # PyTorch model (source)
│   ├── onnxruntime/         # ONNX Runtime library
│   └── build/               # Build output directory
└── convert_yolo_to_onnx.py  # Model conversion script
```

## Building from Source

### Prerequisites

1. **OpenCV**: Install development libraries
2. **ONNX Runtime**: Included in the project
3. **YOLOv8 Model**: Place in `Source/models/`

### Build Steps

```bash
# Clone or navigate to project
cd CameraApp

# Build using the runner script
./run.sh build

# Or build manually
cd Source
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
```

### Build Options

```bash
# Debug build
./run.sh build Debug

# Clean build
./run.sh clean

# Install (requires sudo)
cd Source
./build_webcam.sh install
```

## Troubleshooting

### Camera Issues

```bash
# Check camera devices
ls -la /dev/video*

# Check camera permissions
groups $USER

# Test camera with v4l2-ctl
v4l2-ctl --list-devices

# Check if camera is busy
lsof /dev/video0
```

### Build Issues

```bash
# Clean and rebuild
./run.sh clean
./run.sh build

# Check dependencies
./run.sh info

# Debug build
./run.sh build Debug
```

### Performance Issues

- **Lower resolution**: Use `--width 320 --height 320`
- **Higher confidence**: Use `--confidence 0.5`
- **Reduce processing**: The app processes every 3rd frame by default

### Common Errors

1. **"OpenCV not found"**: Install `libopencv-dev`
2. **"Model not found"**: Ensure `yolov8n-face.onnx` is in `Source/models/`
3. **"Camera busy"**: Close other applications using the camera
4. **"Permission denied"**: Add user to `video` group: `sudo usermod -a -G video $USER`

## Development

### Adding New Features

1. **Modify `webcam.h`**: Add new method declarations
2. **Update `webcam.cpp`**: Implement new functionality
3. **Update `main.cpp`**: Add command line options if needed
4. **Test**: Use `./run.sh test`

### Code Style

- **C++14** standard
- **OpenCV** for image processing
- **ONNX Runtime** for AI inference
- **POSIX** for system calls

### Testing

```bash
# Run tests
./run.sh test

# Test specific camera
./webcam_app --camera 0

# Test simulation mode
./webcam_app --simulation
```

## Performance

### Benchmarks

- **Resolution**: 640x640 (default)
- **FPS**: ~30 FPS on modern hardware
- **Latency**: <100ms end-to-end
- **Memory**: ~200MB RAM usage
- **CPU**: Single-threaded inference

### Optimization Tips

1. **Use GPU**: Consider CUDA-enabled ONNX Runtime
2. **Lower resolution**: Trade accuracy for speed
3. **Batch processing**: Process multiple frames together
4. **Model quantization**: Use INT8 models for speed

## License

This project is provided as-is for educational and research purposes.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## Support

For issues and questions:
1. Check the troubleshooting section
2. Review the logs in `Source/build/webcam.log`
3. Run `./run.sh info` for system information
4. Create an issue with detailed information

---

**Happy Face Detecting! 🎯**
