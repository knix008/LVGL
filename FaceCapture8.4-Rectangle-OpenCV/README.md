# Webcam Face Detection Application

A real-time webcam capture application with AI-powered face detection, built using LVGL, OpenCV, and ONNX Runtime.

![Platform](https://img.shields.io/badge/platform-Linux-blue)
![Language](https://img.shields.io/badge/language-C-green)
![License](https://img.shields.io/badge/license-MIT-orange)

## Features

- 📹 **Real-time webcam streaming** at 15 FPS
- 🤖 **AI face detection** using YOLOv8n-face model
- 📊 **Confidence display** showing detection accuracy for each face
- 📸 **Photo capture** with high-resolution JPEG output
- 🎨 **Modern GUI** with Korean language support
- 🎵 **Shutter sound** effect on photo capture
- ⚡ **Flash effect** for better photo lighting
- 🖼️ **Visual feedback** with green bounding boxes
- 🔒 **Robust thread management** with proper cleanup and cancellation
- ⚡ **Responsive exit** - window closes immediately even when camera is running

## Demo

```
┌─────────────────────────────────┐
│  Webcam Preview                 │
│                                  │
│     87%  ← Confidence           │
│    ┌──────────┐                 │
│    │          │                 │
│    │   Face   │ ← Detection box │
│    │          │                 │
│    └──────────┘                 │
│                                  │
│  [   Capture   ]  ← Button      │
│  Photos: 5                       │
└─────────────────────────────────┘
```

## Architecture

```
┌─────────────┐     ┌──────────────┐     ┌───────────┐
│   Webcam    │────→│   OpenCV     │────→│  Camera   │
│ /dev/video0 │     │ VideoCapture │     │  Module   │
└─────────────┘     └──────────────┘     └─────┬─────┘
                                                │
                                                ↓
                    ┌───────────────────────────┴─────┐
                    │                                 │
          ┌─────────▼──────────┐         ┌──────────▼─────────┐
          │  Face Detection    │         │   GUI Module       │
          │  (ONNX Runtime)    │────────→│   (LVGL + SDL2)    │
          │  YOLOv8n-face      │         │   Display Output   │
          └────────────────────┘         └────────────────────┘
```

## Requirements

### Hardware
- Linux system (tested on Ubuntu 22.04+)
- Webcam (V4L2 compatible)
- CPU with SSE2 support (for ONNX Runtime)

### Software Dependencies
- GCC and G++ compilers
- SDL2 and SDL2_mixer
- OpenCV 4.x development libraries
- FreeType2
- libjpeg
- ONNX Runtime (included)

## Installation

### 1. Install System Dependencies

```bash
sudo apt-get update
sudo apt-get install -y \
    gcc \
    g++ \
    git \
    libsdl2-dev \
    libsdl2-mixer-dev \
    libfreetype6-dev \
    libjpeg-dev \
    libopencv-dev
```

### 2. Clone and Build

```bash
cd /path/to/FaceCapture8.4

# Build LVGL and the application
./setup.sh

# Or build manually
make
```

### 3. Verify Installation

```bash
# Check executable
ls -lh camera

# Check webcam
ls -l /dev/video*

# Check model
ls -lh models/yolov8n-face.onnx
```

## Usage

### Basic Usage

```bash
# Run the application
./camera

# Or use make
make run
```

### Controls

- **Click "캡처" button** or **press space**: Take a photo
- **Close window** or **press ESC**: Exit application

### Photo Output

Photos are saved in the current directory with timestamps:
```
photo_20250104_143052.jpg
photo_20250104_143055.jpg
```

## Configuration

### Camera Device

Edit [camera.h](camera.h#L13):
```c
#define VIDEO_DEVICE "/dev/video2"  // Change to your camera
```

### Detection Parameters

Edit [face_detection.c](face_detection.c#L23-27):
```c
#define CONF_THRESHOLD 0.5f      // Minimum confidence (0-1)
#define NMS_THRESHOLD 0.45f      // NMS IoU threshold
#define MAX_FACES 10             // Maximum faces to detect
```

### Display Settings

Edit [main.c](main.c#L16-17):
```c
#define WINDOW_WIDTH  340
#define WINDOW_HEIGHT 640
```

### Preview Resolution

Edit [camera.h](camera.h#L14-15):
```c
#define CAMERA_WIDTH  320
#define CAMERA_HEIGHT 240
```

## Project Structure

```
FaceCapture8.4-Rectangle-OpenCV/
├── main.c                  # Application entry point
├── camera.cpp/h            # Webcam capture (OpenCV)
├── gui.c/h                 # User interface (LVGL)
├── face_detection.c/h      # AI detection (ONNX Runtime)
├── Makefile                # Build configuration
├── setup.sh                # Installation script
├── models/
│   └── yolov8n-face.onnx  # Face detection model (12MB)
├── assets/
│   ├── NanumGothicCoding.ttf       # Korean font
│   ├── NanumGothicCoding-Bold.ttf  # Korean font (bold)
│   └── CameraShuffter.mp3          # Shutter sound
├── onnxruntime-linux-x64-1.16.3/
│   ├── include/            # ONNX Runtime headers
│   └── lib/                # ONNX Runtime library
├── lvgl/                   # LVGL library (built by setup.sh)
├── FACE_DETECTION.md       # Face detection documentation
├── CONFIDENCE_DISPLAY.md   # Confidence display documentation
└── README.md               # This file
```

## Building from Source

### Clean Build

```bash
make clean     # Remove object files
make           # Compile application
```

### Deep Clean

```bash
make distclean  # Remove LVGL build artifacts
./setup.sh      # Rebuild everything
```

### Debug Build

```bash
# Add debug symbols
gcc -g -DDEBUG main.c camera.c gui.c face_detection.c ...
```

## Performance

| Metric | Value |
|--------|-------|
| Preview FPS | ~15 FPS |
| Detection time | 30-50ms per frame |
| Model size | 12 MB |
| Memory usage | ~50 MB |
| CPU usage | 30-40% (single core) |

## Troubleshooting

### Camera Not Found

```bash
# List available cameras
ls -l /dev/video*

# Check permissions
sudo usermod -a -G video $USER

# Test with v4l2
v4l2-ctl --list-devices
```

### Face Detection Not Working

```bash
# Verify model file
ls -lh models/yolov8n-face.onnx

# Check ONNX Runtime library
ls -lh onnxruntime-linux-x64-1.16.3/lib/

# Run with debug output
./camera 2>&1 | grep -i "face\|onnx"
```

### Build Errors

```bash
# Missing LVGL
./setup.sh

# Missing dependencies
sudo apt-get install -y libopencv-dev

# Check pkg-config
pkg-config --modversion sdl2 freetype2 opencv4
```

### Application Crashes

```bash
# Run with core dumps enabled
ulimit -c unlimited
./camera

# Debug with gdb
gdb ./camera
(gdb) run
(gdb) bt  # After crash
```

### Window Won't Close

If the application doesn't exit when closing the window:
- The entire cleanup process has a **2-second alarm timeout**
- If cleanup hangs for any reason, process is force-killed with `_exit(1)`
- Camera thread termination tries graceful shutdown first (200ms)
- Then pthread cancellation (100ms)
- Then detaches thread and relies on alarm to kill process
- Check terminal output for cleanup messages
- See [THREAD_CLEANUP_FIX.md](THREAD_CLEANUP_FIX.md) for details

## Technical Details

### Face Detection Pipeline

1. **Capture**: OpenCV VideoCapture grabs frames from V4L2 device
2. **Color Conversion**: BGR to RGB conversion for JPEG, keep BGR for display
3. **Resize**: Scale to 320×240 for preview (BGR for LVGL RGB888)
4. **Preprocess**: Resize to 640×640, normalize to [0,1]
5. **Inference**: ONNX Runtime runs YOLOv8n-face model
6. **Post-process**: Filter by confidence, apply NMS
7. **Visualize**: Draw boxes and confidence percentages
8. **Display**: Update LVGL canvas

### Model Information

- **Architecture**: YOLOv8 Nano
- **Task**: Face detection
- **Input**: 640×640×3 RGB
- **Output**: [1, 5, 8400] (x, y, w, h, confidence)
- **Framework**: ONNX
- **Quantization**: FP32

### Threading Model

- **Main thread**: LVGL event loop, GUI updates
- **Camera thread**: OpenCV frame capture (pthread) with cancellation support
- **Inference**: Synchronous on main thread
- **Thread cleanup**:
  - Graceful shutdown with 200ms timeout
  - pthread_cancel with 100ms timeout
  - Process-wide alarm forces _exit(1) after 2 seconds total
  - See [THREAD_CLEANUP_FIX.md](THREAD_CLEANUP_FIX.md)

## API Reference

### Camera Module

```c
int camera_init(void);                      // Initialize camera
int camera_start(void);                     // Start capture thread
void camera_stop(void);                     // Stop capture
uint8_t *camera_get_frame(void);           // Get latest frame
int camera_save_photo(const char *filename); // Save photo
void camera_get_dimensions(int *w, int *h); // Get frame size
void camera_cleanup(void);                  // Release resources
```

### Face Detection Module

```c
bool face_detection_init(const char *model_path);
bool face_detection_detect(const uint8_t *rgb_data,
                           int width,
                           int height,
                           FaceDetectionResult *result);
void face_detection_cleanup(void);
bool face_detection_is_initialized(void);
```

### GUI Module

```c
int gui_init(void);
void gui_update_camera_preview(uint8_t *frame_data);
void gui_update_camera_preview_with_faces(uint8_t *frame_data,
                                          const FaceDetectionResult *faces);
void gui_update_status(const char *message);
void gui_update_photo_count(int count);
void gui_set_capture_callback(void (*callback)(void *), void *user_data);
void gui_show_flash(uint32_t duration_ms);
void gui_play_shutter_sound(void);
void gui_cleanup(void);
```

## Development

### Adding New Features

1. **New detection models**: Replace `models/yolov8n-face.onnx`
2. **Custom UI elements**: Modify `gui.c`
3. **Additional filters**: Add processing in `camera.c`
4. **New callbacks**: Extend `gui_set_*_callback()` functions

### Code Style

- Follow K&R indentation style
- Use descriptive variable names
- Comment complex algorithms
- Keep functions under 100 lines
- Use `static` for internal functions

### Testing

```bash
# Build and run tests
cd test
make
./test_camera
```

## Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## Dependencies Licenses

- **LVGL**: MIT License
- **SDL2**: zlib License
- **OpenCV**: Apache 2.0 License
- **ONNX Runtime**: MIT License
- **YOLOv8**: AGPL-3.0 (Ultralytics)
- **FreeType**: FreeType License
- **NanumGothicCoding**: SIL Open Font License

## Acknowledgments

- **LVGL Team**: For the amazing embedded GUI library
- **Ultralytics**: For YOLOv8 face detection model
- **Microsoft**: For ONNX Runtime
- **OpenCV Team**: For computer vision libraries

## Documentation

- [FACE_DETECTION.md](FACE_DETECTION.md) - Face detection implementation details
- [CONFIDENCE_DISPLAY.md](CONFIDENCE_DISPLAY.md) - Confidence visualization guide
- [THREAD_CLEANUP_FIX.md](THREAD_CLEANUP_FIX.md) - Thread cleanup and force exit mechanism
- [THREAD_HANG_FIX.md](THREAD_HANG_FIX.md) - Async thread cancellation approach
- [MIGRATION_NOTES.md](MIGRATION_NOTES.md) - FFmpeg to OpenCV migration guide

## Support

For issues, questions, or suggestions:
- Create an issue on GitHub
- Check existing documentation
- Review troubleshooting section

## Roadmap

- [ ] GPU acceleration support (CUDA/OpenCL)
- [ ] Face recognition capabilities
- [ ] Multiple camera support
- [ ] Video recording feature
- [ ] Face tracking over time
- [ ] Emotion detection
- [ ] Age/gender estimation
- [ ] Web interface
- [ ] Mobile app version

## Version History

### v2.1.0 (Current)
- **Fixed**: Process now exits within 2 seconds even if camera thread hangs
- Added: POSIX alarm-based cleanup timeout (force _exit after 2 seconds)
- Improved: Thread cleanup escalation (graceful → cancel → detach → force exit)
- Enhanced: Thread termination tries multiple strategies before force kill
- Added: THREAD_CLEANUP_FIX.md documentation
- Removed: signal.h and sys/types.h from camera.cpp (not needed)

### v2.0.0
- **Major**: Migrated from FFmpeg to OpenCV for video capture
- Changed: camera.c → camera.cpp (C++ implementation)
- Improved: Simpler and more maintainable video capture code
- Enhanced: Proper BGR/RGB color format handling for LVGL RGB888
- Updated: Build system with C++ compiler support
- Added: OpenCV dependency checking in Makefile

### v1.1.0
- Fixed: Application now exits properly when camera is running
- Fixed: All compiler warnings resolved
- Improved: Camera thread with cancellation points
- Improved: Robust thread cleanup with 2-second timeout
- Enhanced: Better error handling in ONNX Runtime API calls

### v1.0.0
- Initial release
- Real-time face detection
- Confidence display
- Photo capture with flash
- Korean UI support

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Author

Developed with ❤️ using LVGL, OpenCV, and ONNX Runtime

---

**Note**: This application is designed for educational and research purposes. Ensure compliance with privacy laws and regulations when using face detection technology.
