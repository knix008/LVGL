# GTK Webcam Viewer

A simple, lightweight GTK3 application for viewing live webcam streams with camera on/off control.

## Features

- **Live Webcam Streaming**: Display real-time video from your webcam
- **Camera Control**: Toggle camera on and off with a simple button
- **FPS Display**: Monitor frame rate in real-time
- **Status Information**: View current camera status
- **Horizontal Flip**: Mirror effect for natural viewing
- **Multithreaded Capture**: Smooth video playback without UI blocking
- **Fixed Window Size**: Non-resizable 800x600 window with camera display area (640x480)

## Requirements

### System Dependencies

Install the required packages on your system:

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    libgtk-3-dev \
    libgdk-pixbuf2.0-dev \
    libopencv-dev \
    pkg-config
```

**Fedora/RHEL:**
```bash
sudo dnf install -y \
    gcc-c++ \
    cmake \
    gtk3-devel \
    gdk-pixbuf2-devel \
    opencv-devel \
    pkg-config
```

**Arch Linux:**
```bash
sudo pacman -S \
    base-devel \
    cmake \
    gtk3 \
    gdk-pixbuf2 \
    opencv \
    pkg-config
```

### Build Requirements
- C++17 compiler (GCC 7+, Clang 5+)
- OpenCV 4.0+
- GTK3
- GdkPixbuf2
- CMake 3.10+ (optional, can use Makefile)

## Building

### Option 1: Using Make (Recommended for simplicity)

```bash
cd gtk-webcam
make
```

### Option 2: Using CMake

```bash
cd gtk-webcam
mkdir build
cd build
cmake ..
make
```

### Build Targets

```bash
make              # Build the application
make run          # Build and run the application
make debug        # Build with debug symbols
make debug-run    # Run with GDB debugger
make clean        # Remove build artifacts
make help         # Show available targets
```

## Running

### From Build Directory

```bash
# Using Make
./bin/gtk_webcam

# Using CMake
./build/gtk_webcam
```

### Direct Run Command

```bash
make run
```

## Usage

1. **Start the Application**: Launch the executable
2. **Start Camera**: Click the "Start Camera" button to begin streaming
3. **View Stream**: The live webcam feed will appear in the 640x480 display area
4. **Monitor FPS**: Check the frame rate in the top-right corner
5. **Stop Camera**: Click the "Stop Camera" button to stop streaming
6. **Close Application**: Close the window or press Ctrl+C in terminal

## Architecture

### File Structure

```
gtk-webcam/
├── include/
│   ├── camera.h           # Camera capture interface
│   └── gtk_app.h          # GTK application class
├── src/
│   ├── main.cpp           # Application entry point
│   ├── camera.cpp         # Camera implementation
│   └── gtk_app.cpp        # GTK UI implementation
├── CMakeLists.txt         # CMake build configuration
├── Makefile               # Make build configuration
└── README.md              # This file
```

### Key Classes

#### Camera Class
- Handles OpenCV video capture
- Background thread for frame capture
- Thread-safe frame queue
- Properties: resolution, FPS, active status

#### GTKApp Class
- Main GTK application controller
- UI initialization and management
- Frame refresh timer
- Camera on/off toggle logic
- FPS calculation and display

## Troubleshooting

### Camera Not Found
- Check if your webcam is connected: `ls /dev/video*`
- Ensure you have permission to access the camera:
  ```bash
  sudo usermod -a -G video $USER
  # Log out and log back in for changes to take effect
  ```
- Try specifying a different camera ID (0, 1, 2, etc.)

### Low FPS or Dropped Frames
- Close other applications using the camera
- Reduce resolution or frame rate (modify in `camera.cpp`)
- Check CPU usage with `top` or `htop`

### GTK/GdkPixbuf Errors
- Ensure all GTK development libraries are installed
- Run `pkg-config --cflags gtk+-3.0` to verify GTK3 is installed

### OpenCV Not Found
- Verify OpenCV is installed: `pkg-config --modversion opencv4`
- If missing, install with your package manager
- For CMake, you may need to specify OpenCV path:
  ```bash
  cmake -DOpenCV_DIR=/path/to/opencv/build ..
  ```

## Configuration

### Modify Camera Parameters

Edit `camera.cpp` in the `open()` method to adjust:

```cpp
cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);   // Width
cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);  // Height
cap.set(cv::CAP_PROP_FPS, 30);            // Frame rate
```

### Change Window Properties

Edit `gtk_app.cpp` in the `init()` method:

```cpp
// Window size (currently fixed, not resizable)
gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

// To make resizable, change FALSE to TRUE:
gtk_window_set_resizable(GTK_WINDOW(window), FALSE);  // Change to TRUE for resizable

// Camera display area
gtk_widget_set_size_request(image_widget, 640, 480);  // Resolution
```

### Change Refresh Rate

Edit `gtk_app.cpp` in the `init()` method:

```cpp
refresh_timer = g_timeout_add(30, on_refresh_timer, this);  // 30ms = ~33 FPS
```

## Performance Tips

1. **Resolution**: Lower resolution = higher FPS
2. **Threading**: The camera capture runs in a separate thread, preventing UI freezing
3. **Queue Size**: Limited frame queue prevents memory buildup
4. **Frame Copy**: Minimal copying of frames for efficiency

## Development

### Building in Debug Mode

```bash
make debug
make debug-run    # Run with GDB
```

### Adding Features

The modular design allows easy extension:
- Add more camera properties in `Camera` class
- Extend GTK UI with additional widgets in `GTKApp` class
- Implement image processing on captured frames

## Limitations

- Single camera support (can be modified for multi-camera)
- Fixed resolution and FPS (configurable at compile time)
- No frame recording (can be added)
- No image effects/filters (OpenCV integration available)

## License

This project is open source and available for personal and educational use.

## Support

For issues or questions:
1. Check the Troubleshooting section
2. Verify all dependencies are installed
3. Check console output for error messages
4. Review camera permissions

## Future Enhancements

- [ ] Multiple camera support
- [ ] Frame recording to video file
- [ ] Image capture/screenshot
- [ ] Basic image filters (grayscale, blur, etc.)
- [ ] Configuration file for settings
- [ ] Hardware acceleration support
- [ ] Network streaming
- [ ] Resolution/FPS adjustment from GUI
