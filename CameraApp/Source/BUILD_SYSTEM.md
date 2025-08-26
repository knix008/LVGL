# Webcam Application Build System

This document describes the updated build system for the webcam application with AI face detection capabilities.

## Overview

The build system uses CMake for configuration and provides multiple build options including Release and Debug configurations. It includes comprehensive error checking, dependency validation, and automated testing.

## Files

### Core Build Files

- **`CMakeLists.txt`** - Main CMake configuration file
- **`build_webcam.sh`** - Convenient build script with multiple options
- **`build_all_libs.sh`** - Library build script (for compatibility)

### Source Files

- **`src/webcam_ipc_app.cpp`** - Main webcam application source
- **`include/webcam_ipc_app.h`** - Application header file

### Dependencies

- **`onnxruntime/`** - ONNX Runtime library for AI inference
- **`models/`** - YOLOv8 face detection models

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
- ONNX Runtime library and header validation
- Model file existence checking

#### 5. **System Libraries**
- Automatic detection of required system libraries
- Platform-specific linking (Unix/Linux)

#### 6. **Installation Support**
- Install targets for executable and model files
- Configurable install prefix

## Build Script Usage

### Basic Commands

```bash
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
2. **Configuration**: Runs CMake with specified build type
3. **Compilation**: Builds with parallel compilation (`-j$(nproc)`)
4. **Post-processing**: Copies model files to build directory
5. **Testing**: Runs application tests

## Configuration Summary

The build system provides detailed configuration information:

```
=== Webcam Application Configuration ===
Build type: Release
C++ standard: 14
OpenCV version: 4.5.4
ONNX Runtime: /path/to/onnxruntime
Install prefix: /usr/local
=========================================
```

## Error Handling

### Dependency Errors
- Missing cmake or make tools
- OpenCV not found or version too old
- ONNX Runtime library/headers missing

### Build Errors
- Compilation errors with detailed messages
- Linking errors with library path information
- Model file missing warnings

## Installation

### System Installation
```bash
# Build and install
./build_webcam.sh build
./build_webcam.sh install
```

### Install Locations
- **Executable**: `/usr/local/bin/webcam_ipc_app`
- **Models**: `/usr/local/share/webcam_app/models/`

## Testing

### Test Targets
- **`test_webcam`**: Runs application tests
- **Manual Testing**: Direct application execution

### Test Commands
```bash
# Run automated tests
./build_webcam.sh test

# Manual testing
cd build
./webcam_ipc_app --model models/yolov8n-face.onnx
```

## Troubleshooting

### Common Issues

1. **OpenCV Not Found**
   ```bash
   sudo apt-get install libopencv-dev
   ```

2. **ONNX Runtime Missing**
   - Ensure `onnxruntime/` directory exists with library files

3. **Model Files Missing**
   - Check that `models/yolov8n-face.onnx` exists

4. **Permission Errors**
   - Use `sudo` for installation commands

### Debug Build
```bash
./build_webcam.sh build Debug
```

## Performance Optimization

### Release Build Features
- **Optimization Level**: `-O3` for maximum performance
- **Debug Symbols**: Disabled with `-DNDEBUG`
- **Parallel Compilation**: Uses all CPU cores

### Debug Build Features
- **Debug Symbols**: Enabled with `-g`
- **Optimization**: Disabled with `-O0`
- **Debug Information**: Full debugging support

## Version Information

- **Version**: 1.0.0
- **C++ Standard**: C++14
- **CMake Minimum**: 3.10
- **OpenCV Minimum**: 4.0.0

## Future Enhancements

- Support for additional build types (RelWithDebInfo, MinSizeRel)
- Cross-platform compilation support
- Package generation (DEB, RPM)
- Continuous Integration integration
- Automated dependency management
