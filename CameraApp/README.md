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

## Quick Start

1. **Clone the repository:**
   ```bash
   git clone <repository-url>
   cd CameraApp
   ```

2. **Install ONNX Runtime:**
   ```bash
   cd Source
   ./install_onnxruntime.sh
   ```

3. **Build the application:**
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

4. **Run the application:**
   ```bash
   make run-webcam
   ```

## Installation

### ONNX Runtime Installation

The project includes an automated script to install ONNX Runtime 1.16.3:

```bash
cd Source
./install_onnxruntime.sh [OPTIONS]
```

**Options:**
- `--help` - Show usage information
- `--version` - Show script version
- `--force` - Force reinstallation

**What the script does:**
1. Downloads ONNX Runtime 1.16.3 from official releases
2. Extracts it to the correct location
3. Verifies the installation
4. Tests CMake configuration

### Manual Installation

If you prefer manual installation:

1. Download ONNX Runtime 1.16.3 from [GitHub releases](https://github.com/microsoft/onnxruntime/releases/tag/v1.16.3)
2. Extract to `Source/onnxruntime-linux-x64-1.16.3/`
3. Ensure the library file `lib/libonnxruntime.so` exists

## Building the Application

### Method 1: Using the Installation Script (Recommended)
```bash
cd Source
./install_onnxruntime.sh
mkdir build && cd build
cmake ..
make
```

### Method 2: Manual Build
```bash
cd Source
mkdir build
cd build
cmake ..
make
```

The build process will:
- Compile the C++ source code
- Link against OpenCV and ONNX Runtime libraries
- Copy model files to the build directory
- Set up proper RPATH for self-contained execution

## Running the Application

### Method 1: Using CMake Target (Recommended)
```bash
cd Source/build
make run-webcam
```

This method automatically sets the correct library paths and runs the application.

### Method 2: Direct Execution
```bash
cd Source/build
./webcam_app
```

The executable is configured with RPATH, so it should find the libraries automatically.

## Available CMake Targets

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
│   ├── opencv/                                  # OpenCV installation
│   ├── onnxruntime-linux-x64-1.16.3/           # ONNX Runtime 1.16.3
│   │   ├── lib/
│   │   │   └── libonnxruntime.so
│   │   └── include/
│   │       └── onnxruntime_c_api.h
│   ├── install_onnxruntime.sh                   # Installation script
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
