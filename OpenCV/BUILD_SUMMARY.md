# OpenCV 4.12.0 Build Summary

## Overview
Successfully built OpenCV 4.12.0 from source in the `src/opencv` directory and created a YOLOv8 detection program that uses the built library.

## Build Process

### 1. OpenCV Build
- **Source Location**: `src/opencv/`
- **Build Configuration**: Minimal build with only essential modules
- **Installed Location**: `src/lib/`
- **Key Features**:
  - Static library build (`libopencv_world.a`)
  - Core, DNN, ImgProc, and ImgCodecs modules only
  - No GUI components (GTK, Qt disabled)
  - No video I/O components
  - Protobuf support for DNN

### 2. Program Build
- **Main Program**: `src/main.cpp`
- **Detector Class**: `src/yolov8n_detector.cpp`
- **Header**: `include/yolov8n_detector.h`
- **Executable**: `build/yolov8n_detection`

## Build Scripts

### `build.sh`
- Downloads OpenCV 4.12.0 source if not present
- Configures OpenCV with minimal dependencies
- Builds OpenCV and installs to `src/lib/`
- Builds the main program using the built OpenCV

### `run.sh`
- Simple script to run the built executable

## Key Configuration

### OpenCV Build Options
- `BUILD_SHARED_LIBS=OFF` - Static library
- `BUILD_opencv_world=ON` - Single world library
- `WITH_GTK=OFF`, `WITH_QT=OFF` - No GUI
- `WITH_OPENCV_HIGHGUI=OFF` - No highgui
- `WITH_OPENCV_VIDEOIO=OFF` - No video I/O
- `WITH_OPENCV_DNN=ON` - DNN support for YOLO
- `WITH_OPENCV_IMGCODECS=ON` - Image I/O
- `WITH_OPENCV_IMGPROC=ON` - Image processing

### Program Dependencies
- OpenCV 4.12.0 (built from source)
- Eigen3 (system library)
- JPEG, PNG, Zlib (system libraries)
- Protobuf (built with OpenCV)

## File Structure
```
OpenCV/
├── build.sh                 # Main build script
├── run.sh                   # Run script
├── CMakeLists.txt          # Program CMake configuration
├── build/                  # Build directory
│   └── yolov8n_detection  # Executable
├── src/
│   ├── main.cpp           # Main program
│   ├── yolov8n_detector.cpp # YOLO detector implementation
│   └── opencv/            # OpenCV source
├── include/
│   └── yolov8n_detector.h # Header file
├── data/
│   └── bus.jpg            # Test image
└── models/                # Model directory
```

## Usage

### Build Everything
```bash
./build.sh
```

### Run the Program
```bash
./run.sh
```

### Manual Build
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

## Notes
- The program expects a YOLOv8 ONNX model in the `models/` directory
- Test images should be placed in the `data/` directory
- The build uses static linking for portability
- All dependencies are either built from source or use system libraries
