# CameraApp - Real-time Face Detection

A real-time face detection application using OpenCV and YOLOv8 with ONNX Runtime 1.16.3.

## Features

- Real-time face detection using webcam
- YOLOv8n-face model for accurate face detection
- ONNX Runtime 1.16.3 for optimized inference
- Cross-platform support (Linux, Windows, macOS)
- Self-contained build system using CMake
- Automated ONNX Runtime installation script

## Prerequisites

- CMake 3.10 or higher
- C++14 compatible compiler
- Webcam device
- Linux system with GTK libraries
- wget and tar (for ONNX Runtime installation)
- Git (for downloading OpenCV source)

## Quick Start

1. **Clone the repository:**
   ```bash
   git clone <repository-url>
   cd CameraApp
   ```

2. **Build the application (auto-installs dependencies):**
   ```bash
   cd Source
   ./build_webcam.sh
   ```

3. **Run the application:**
   ```bash
   cd Source
   ./build_webcam.sh run
   ```

## Build System

The project uses a comprehensive build system with the following components:

### Automated Build Scripts

1. **build_webcam.sh** - Main build script that:
   - Auto-installs ONNX Runtime if missing
   - Auto-builds OpenCV locally if missing
   - Builds the webcam application
   - Provides run, test, and clean commands

2. **build_opencv.sh** - OpenCV build script that:
   - Detects CPU architecture (x64/aarch64)
   - Downloads and builds OpenCV 4.8.1 locally
   - Includes self-contained image and video codecs
   - Optimized for the target architecture

3. **install_onnxruntime.sh** - ONNX Runtime installer that:
   - Detects CPU architecture (x64/aarch64)
   - Downloads appropriate ONNX Runtime version
   - Installs to the correct location

### Self-Contained Dependencies

The build system is designed to be self-contained:
- **OpenCV**: Built locally with all dependencies included
- **ONNX Runtime**: Downloaded and installed locally
- **System Dependencies**: Only requires GStreamer development packages

## Installation

### Automated Installation (Recommended)

The project includes automated scripts that handle all dependencies:

```bash
cd Source
./build_webcam.sh
```

This single command will:
1. Install ONNX Runtime 1.16.3 (if missing)
2. Build OpenCV 4.8.1 locally (if missing)
3. Build the webcam application
4. Set up all necessary configurations

### Manual Installation

If you prefer manual installation:

1. **Install ONNX Runtime:**
   ```bash
   cd Source
   ./install_onnxruntime.sh
   ```

2. **Build OpenCV:**
   ```bash
   cd Source
   ./build_opencv.sh
   ```

3. **Build the application:**
   ```bash
   cd Source
   mkdir build && cd build
   cmake ..
   make
   ```

## Building the Application

### Method 1: Using the Build Script (Recommended)
```bash
cd Source
./build_webcam.sh
```

### Method 2: Manual Build
```bash
cd Source
./build_webcam.sh build
```

### Method 3: Step-by-step Manual Build
```bash
cd Source
./install_onnxruntime.sh
./build_opencv.sh
mkdir build && cd build
cmake ..
make
```

The build process will:
- Auto-install missing dependencies
- Compile the C++ source code
- Link against OpenCV and ONNX Runtime libraries
- Copy model files to the build directory
- Set up proper RPATH for self-contained execution

## Running the Application

### Method 1: Using the Build Script (Recommended)
```bash
cd Source
./build_webcam.sh run
```

### Method 2: Using CMake Target
```bash
cd Source/build
make run-webcam
```

### Method 3: Direct Execution
```bash
cd Source/build
./webcam_app
```

The executable is configured with RPATH, so it should find the libraries automatically.

## Available Commands

### Build Script Commands
```bash
./build_webcam.sh build        # Build the application
./build_webcam.sh run          # Build and run the application
./build_webcam.sh test         # Run tests
./build_webcam.sh clean        # Clean build files
./build_webcam.sh info         # Show build information
./build_webcam.sh help         # Show help
```

### CMake Targets
- `make` - Build the application
- `make run-webcam` - Build and run the application
- `make info` - Display build configuration information
- `make clean` - Clean build files
- `make clean-build` - Complete clean rebuild
- `make test_webcam` - Run basic tests

## Configuration

The application uses the following configuration:

- **OpenCV**: Local installation in `Source/opencv/`
- **ONNX Runtime**: Prebuilt 1.16.3 in `Source/onnxruntime-linux-x64-1.16.3/`
- **Model**: YOLOv8n-face model in `Source/models/`
- **RPATH**: Automatically configured for self-contained execution

## Troubleshooting

### ONNX Runtime Installation Issues
```bash
# Check if the installation script exists
ls -la Source/install_onnxruntime.sh

# Make it executable if needed
chmod +x Source/install_onnxruntime.sh

# Run with verbose output
./install_onnxruntime.sh --force
```

### Library Not Found Errors
If you encounter library not found errors, ensure:
1. OpenCV is properly built in `Source/opencv/`
2. ONNX Runtime is available in `Source/onnxruntime-linux-x64-1.16.3/`
3. Use `make run-webcam` instead of direct execution

### Webcam Access Issues
- Ensure your webcam is connected and accessible
- Check webcam permissions on your system
- Try running with `sudo` if needed (for device access)

### Model Loading Issues
- Verify `yolov8n-face.onnx` exists in `Source/models/`
- Check file permissions on the model file

## Project Structure

```
CameraApp/
├── Source/
│   ├── build/                                    # Build directory
│   ├── include/                                  # Header files
│   │   └── webcam.h
│   ├── src/                                     # Source files
│   │   ├── main.cpp
│   │   └── webcam.cpp
│   ├── models/                                  # AI models
│   │   └── yolov8n-face.onnx
│   ├── opencv/                                  # Local OpenCV installation
│   │   ├── lib/                                # OpenCV libraries
│   │   ├── include/                            # OpenCV headers
│   │   └── share/                              # OpenCV data files
│   ├── onnxruntime-linux-x64-1.16.3/           # ONNX Runtime 1.16.3
│   │   ├── lib/
│   │   │   └── libonnxruntime.so
│   │   └── include/
│   │       └── onnxruntime_c_api.h
│   ├── build_webcam.sh                         # Main build script
│   ├── build_opencv.sh                         # OpenCV build script
│   ├── install_onnxruntime.sh                  # ONNX Runtime installer
│   ├── convert_yolo.sh                         # Model conversion script
│   ├── BUILD_SYSTEM.md                         # Build system documentation
│   └── CMakeLists.txt                          # Build configuration
├── .gitignore
└── README.md
```

## Development

### Adding New Features
1. Add source files to `Source/src/`
2. Add headers to `Source/include/`
3. Update `CMakeLists.txt` if needed
4. Rebuild with `make`

### Debugging
```bash
cd Source/build
make clean
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

### Updating ONNX Runtime
To update to a newer version of ONNX Runtime:
1. Edit `Source/install_onnxruntime.sh` and change the `ONNX_VERSION` variable
2. Run `./install_onnxruntime.sh --force`
3. Update `Source/CMakeLists.txt` if the directory structure changes

## Dependencies

- **OpenCV 4.8.1** - Computer vision library
- **ONNX Runtime 1.16.3** - Machine learning inference engine
- **YOLOv8n-face** - Face detection model
- **CMake 3.10+** - Build system
- **C++14** - Programming language standard

## License

This project is licensed under the MIT License - see the LICENSE file for details.
