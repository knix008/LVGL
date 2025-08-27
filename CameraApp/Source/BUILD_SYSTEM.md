# Webcam Application Build System

This document describes the build system for the webcam application with AI face detection capabilities using ONNX Runtime 1.16.3.

## Overview

The build system uses CMake for configuration and provides multiple build options including Release and Debug configurations. It includes comprehensive error checking, dependency validation, and automated testing. The system now uses prebuilt ONNX Runtime 1.16.3 for improved reliability and performance.

## Files

### Core Build Files

- **`CMakeLists.txt`** - Main CMake configuration file
- **`build_webcam.sh`** - Convenient build script with multiple options
- **`install_onnxruntime.sh`** - Automated ONNX Runtime installation script

### Source Files

- **`src/main.cpp`** - Main application entry point
- **`src/webcam.cpp`** - Webcam application implementation
- **`include/webcam.h`** - Application header file

### Dependencies

- **`onnxruntime-linux-x64-1.16.3/`** - Prebuilt ONNX Runtime 1.16.3
- **`models/`** - YOLOv8 face detection models
- **`opencv/`** - Local OpenCV installation

## Build Configuration

### CMakeLists.txt Features

#### 1. **Compiler Configuration**
```cmake
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
```

#### 2. **Build Types**
- **Release**: Optimized build with `-O3` and `-DNDEBUG`
- **Debug**: Debug build with `-g` and `-O0`

#### 3. **Compiler Flags**
```cmake
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
```

#### 4. **Dependency Checking**
- OpenCV version validation (requires 4.0.0+)
- ONNX Runtime 1.16.3 library and header validation
- Model file existence checking

#### 5. **Library Configuration**
- **OpenCV**: Local installation in `opencv/` directory
- **ONNX Runtime**: Prebuilt 1.16.3 in `onnxruntime-linux-x64-1.16.3/`
- **System Libraries**: pthread, dl, rt, m, stdc++

#### 6. **RPATH Configuration**
- Automatic RPATH setup for self-contained execution
- Includes both OpenCV and ONNX Runtime library paths

#### 7. **Installation Support**
- Install targets for executable and model files
- Configurable install prefix

## Installation Process

### ONNX Runtime Installation

The project includes an automated installation script for ONNX Runtime 1.16.3:

```bash
./install_onnxruntime.sh [OPTIONS]
```

**Features:**
- Downloads from official GitHub releases
- Automatic extraction and verification
- CMake configuration testing
- Error handling and cleanup

**Options:**
- `--help` - Show usage information
- `--version` - Show script version
- `--force` - Force reinstallation

## Build Script Usage

### Basic Commands

```bash
# Install ONNX Runtime first
./install_onnxruntime.sh

# Build with default Release configuration
./build_webcam.sh build

# Build with Debug configuration
./build_webcam.sh build Debug

# Clean build artifacts
./build_webcam.sh clean

# Run tests
./build_webcam.sh test

# Install application (requires sudo)
./build_webcam.sh install

# Build, test, and install
./build_webcam.sh all

# Show help
./build_webcam.sh help
```

### Build Process

1. **Dependency Check**: Validates cmake, make, and OpenCV
2. **ONNX Runtime Check**: Ensures ONNX Runtime 1.16.3 is installed
3. **Configuration**: Runs CMake with specified build type
4. **Compilation**: Builds with parallel compilation (`-j$(nproc)`)
5. **Post-processing**: Copies model files to build directory
6. **Testing**: Runs application tests

## Configuration Summary

The build system provides detailed configuration information:

```
=== Webcam Application Configuration ===
Build type: Release
C++ standard: 14
OpenCV version: 4.8.1
ONNX Runtime: /path/to/onnxruntime-linux-x64-1.16.3 (1.16.3)
Install prefix: /usr/local
=========================================
```

## Error Handling

### Dependency Errors
- Missing cmake or make tools
- OpenCV not found or version too old
- ONNX Runtime 1.16.3 library/headers missing
- Model file missing

### Build Errors
- Compilation errors with detailed messages
- Linking errors with library path information
- RPATH configuration issues

## Installation

### System Installation
```bash
# Install ONNX Runtime
./install_onnxruntime.sh

# Build and install
./build_webcam.sh build
./build_webcam.sh install
```

### Install Locations
- **Executable**: `/usr/local/bin/webcam_app`
- **Models**: `/usr/local/share/webcam_app/models/`

## Testing

### Test Targets
- **`test_webcam`**: Runs application tests
- **`run-webcam`**: Runs the application with proper environment
- **Manual Testing**: Direct application execution

### Test Commands
```bash
# Run automated tests
./build_webcam.sh test

# Run application
cd build
make run-webcam

# Manual testing
cd build
./webcam_app
```

## Troubleshooting

### Common Issues

1. **ONNX Runtime Not Found**
   ```bash
   ./install_onnxruntime.sh --force
   ```

2. **OpenCV Not Found**
   - Ensure OpenCV is built in `opencv/` directory

3. **Model Files Missing**
   - Check that `models/yolov8n-face.onnx` exists

4. **Permission Errors**
   - Use `sudo` for installation commands

5. **Library Loading Issues**
   - Use `make run-webcam` instead of direct execution

### Debug Build
```bash
./build_webcam.sh build Debug
```

## Performance Optimization

### Release Build Features
- **Optimization Level**: `-O3` for maximum performance
- **Debug Symbols**: Disabled with `-DNDEBUG`
- **Parallel Compilation**: Uses all CPU cores
- **ONNX Runtime**: Prebuilt optimized version

### Debug Build Features
- **Debug Symbols**: Enabled with `-g`
- **Optimization**: Disabled with `-O0`
- **Debug Information**: Full debugging support

## Project Structure

```
Source/
├── build/                                    # Build directory
├── include/                                  # Header files
│   └── webcam.h
├── src/                                     # Source files
│   ├── main.cpp
│   └── webcam.cpp
├── models/                                  # AI models
│   └── yolov8n-face.onnx
├── opencv/                                  # OpenCV installation
├── onnxruntime-linux-x64-1.16.3/           # ONNX Runtime 1.16.3
│   ├── lib/
│   │   └── libonnxruntime.so
│   └── include/
│       └── onnxruntime_c_api.h
├── install_onnxruntime.sh                   # Installation script
├── build_webcam.sh                         # Build script
├── convert_yolo.sh                         # Model conversion script
└── CMakeLists.txt                          # Build configuration
```

## Version Information

- **Version**: 1.0.0
- **C++ Standard**: C++14
- **CMake Minimum**: 3.10
- **OpenCV Minimum**: 4.0.0
- **ONNX Runtime**: 1.16.3

## Future Enhancements

- Support for additional build types (RelWithDebInfo, MinSizeRel)
- Cross-platform compilation support
- Package generation (DEB, RPM)
- Continuous Integration integration
- Automated dependency management
- Support for newer ONNX Runtime versions
