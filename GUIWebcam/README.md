# LVGL Webcam Application

A LVGL-based GUI application with embedded web server, AI-powered face detection, and real-time webcam processing capabilities.

## Features

- **LVGL GUI**: Rich graphical user interface with multiple tabs
- **AI Face Detection**: YOLOv8-based real-time face detection using ONNX Runtime
- **Webcam Processing**: Real-time camera capture and processing
- **IPC Communication**: Inter-process communication between webcam and GUI
- **Embedded Web Server**: Mongoose-based HTTP server with WebSocket support
- **Database Integration**: SQLCipher encrypted database
- **Video Support**: FFmpeg-based video playback
- **Korean Input**: ChunJiIn and QWERTY Korean input methods
- **Calendar & Clock**: Date/time functionality
- **Remote Control**: Web-based interface for controlling the LVGL application

## Quick Start

### Prerequisites

- CMake (3.10 or higher)
- Make
- FreeType2 (`libfreetype6-dev`)
- SDL2 (`libsdl2-dev`)
- FFmpeg libraries
- OpenSSL (`libssl-dev`) - for TLS 1.3 support
- OpenCV (`libopencv-dev`) - for webcam processing
- ONNX Runtime - for AI inference
- curl (for testing)

### Installation

1. Clone the repository
2. Navigate to the project directory
3. Run the application:

```bash
./run.sh
```

This will:
- Check dependencies
- Build the application
- Start the LVGL GUI with embedded web server

## Usage

### Running the Application

```bash
# Build and run (default)
./run.sh

# Build only
./run.sh build

# Clean build artifacts
./run.sh clean

# Show help
./run.sh help
```

### Webcam Application

The project includes two main executables:

#### 1. LVGL GUI Application
```bash
cd Source/build
./main
```
- Starts the LVGL GUI with all tabs
- Includes OpenCV tab for webcam message display
- Web interface available at http://localhost:8080

#### 2. Webcam IPC Application
```bash
cd Source/build
./webcam_ipc_app
```
- Standalone webcam processing with AI face detection
- Communicates with GUI via IPC (Unix domain socket)
- Real-time face detection with coordinate reporting
- Automatic detection count change notifications

### Running Both Applications

For full functionality, run both applications:

**Terminal 1 (GUI):**
```bash
cd Source/build
./main
```

**Terminal 2 (Webcam):**
```bash
cd Source/build
./webcam_ipc_app
```

### Web Interface

Once the application is running, you can access:

- **HTTP Interface**: http://localhost:8080
- **HTTPS Interface**: https://localhost:8443 (TLS 1.3)
- **API Status**: http://localhost:8080/api/status
- **UI State**: http://localhost:8080/api/ui/state
- **WebSocket**: ws://localhost:8080/ws
- **Secure WebSocket**: wss://localhost:8443/ws

## AI Face Detection

### Features

- **YOLOv8 Model**: State-of-the-art face detection using YOLOv8
- **Real-time Processing**: 30 FPS target with detection every 5 frames
- **Multi-camera Support**: Automatic camera detection (indices 0, 2, 3, 1)
- **Coordinate Reporting**: Real-time bounding box coordinates
- **Change Detection**: Automatic notifications when face count changes
- **Simulation Mode**: Fallback when no camera is available

### Model Information

- **Model**: `yolov8_face_model.onnx` (12MB)
- **Input Resolution**: 640x640 pixels
- **Confidence Threshold**: 0.1 (10%)
- **NMS Threshold**: 0.4
- **Processing**: Every 5 frames for real-time performance

### Detection Output

The webcam application sends detection messages in this format:
```
"Faces: 2 | Face1: (270,157,88x116) | Face2: (356,220,12x12)"
```

Where:
- **Faces: N** - Total number of detected faces
- **FaceN: (x,y,widthxheight)** - Bounding box coordinates for each face

### IPC Communication

The webcam application communicates with the GUI via Unix domain socket:

- **Socket Path**: `/tmp/opencv_gui_socket`
- **Protocol**: SOCK_DGRAM (datagram)
- **Message Types**:
  - `IPC_MSG_DETECTION`: Face detection results with coordinates
  - `IPC_MSG_FRAME_PROCESSED`: Frame processing statistics
  - `IPC_MSG_STATUS`: Application status updates
  - `IPC_MSG_ERROR`: Error messages

## Project Structure

```
GUIWebcam/
├── run.sh              # Main runner script
├── test_web.sh         # Web server test script
├── README.md           # This file
├── certs/              # TLS certificates (auto-generated)
│   ├── server.crt      # Server certificate
│   ├── server.key      # Private key
│   └── ca.crt          # CA certificate
├── models/             # AI models
│   ├── yolov8_face_model.onnx  # Face detection model
│   ├── yolov8_face_model.pt    # PyTorch version
│   ├── yolov8n.onnx            # General YOLOv8 model
│   └── yolov8n.pt              # PyTorch version
└── Source/
    ├── include/        # Header files
    │   ├── web_server.h
    │   ├── mongoose.h
    │   ├── tab_opencv.h
    │   └── tls_config.h
    ├── src/           # Source files
    │   ├── main.c                    # LVGL GUI application
    │   ├── webcam_ipc_app.cpp        # Webcam AI application
    │   ├── tab_opencv.c              # OpenCV tab for GUI
    │   ├── web_server.c
    │   ├── mongoose.c
    │   └── ...
    ├── assets/        # Media files
    ├── lvgl/          # LVGL library
    ├── sqlcipher/     # SQLCipher library
    ├── onnxruntime/   # ONNX Runtime library
    └── build/         # Build artifacts
```

## LVGL Features

- **Database Tab**: SQLCipher database operations
- **Settings Tab**: Application configuration
- **Info Tab**: System information and status
- **Number Tab**: Numeric input interface
- **Korean Tab**: Korean text input
- **ChunJiIn Tab**: ChunJiIn input method
- **QWERTY Tab**: QWERTY Korean input
- **Calendar Tab**: Date and calendar functionality
- **Clock Tab**: Time display and management
- **Video Tab**: Video playback with controls
- **OpenCV Tab**: Real-time webcam detection messages

## Development

### Building

```bash
cd Source
mkdir -p build
cd build
cmake ..
make
```

This builds both:
- `main` - LVGL GUI application
- `webcam_ipc_app` - Webcam AI processing application

### Running

```bash
cd Source/build

# Run GUI application
./main

# Run webcam application (in another terminal)
./webcam_ipc_app
```

### Testing

```bash
# Test web server
./test_web.sh

# Test individual components
cd Source
./build_chunjiin_test.sh
./build_calendar_test.sh
./build_clock_test.sh
```

## Troubleshooting

### Common Issues

1. **Build fails**: Check that all dependencies are installed
2. **Web server not accessible**: Ensure port 8080 is not in use
3. **LVGL GUI not appearing**: Check SDL2 installation and display settings
4. **HTTPS not working**: Ensure OpenSSL is installed and port 8443 is available
5. **Certificate warnings**: Self-signed certificates will show browser warnings (normal for development)
6. **TLS handshake fails**: Check that OpenSSL libraries are properly linked
7. **Webcam not detected**: Check camera permissions and try different camera indices
8. **Face detection not working**: Ensure ONNX Runtime and model files are present
9. **IPC communication fails**: Check that both applications are running and socket permissions

### Dependencies

Install required packages on Ubuntu/Debian:

```bash
sudo apt-get install cmake make libfreetype6-dev libsdl2-dev libavformat-dev libavcodec-dev libavutil-dev libswscale-dev libssl-dev libopencv-dev
```

### Camera Issues

If the webcam application can't detect your camera:

1. **Check camera permissions**:
   ```bash
   ls -la /dev/video*
   ```

2. **Test camera with other applications**:
   ```bash
   v4l2-ctl --list-devices
   ```

3. **Try different camera indices**: The application automatically tries indices 0, 2, 3, 1

4. **Check GStreamer warnings**: Some warnings are normal and don't affect functionality

## License

This project uses various open-source libraries:
- LVGL (MIT License)
- Mongoose (GPL v2)
- SQLCipher (BSD License)
- SDL2 (zlib License)
- FFmpeg (LGPL/GPL)
- OpenCV (Apache 2.0 License)
- ONNX Runtime (MIT License)
- YOLOv8 (AGPL-3.0 License)

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## Recent Updates

### Version 2.0 - AI Face Detection
- ✅ Added YOLOv8 face detection model
- ✅ Real-time webcam processing with AI inference
- ✅ IPC communication between webcam and GUI applications
- ✅ Automatic detection count change notifications
- ✅ Coordinate reporting for detected faces
- ✅ Multi-camera support with automatic detection
- ✅ Simulation mode for testing without camera
- ✅ OpenCV tab for real-time message display
- ✅ Improved confidence thresholds for better detection
- ✅ Fixed coordinate calculation for proper bounding boxes
