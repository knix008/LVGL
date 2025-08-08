# C++ PyTorch to ONNX Converter

This project includes a native C++ implementation for converting PyTorch models to ONNX format using LibTorch (PyTorch C++ API).

## Features

- **Native C++ Implementation**: No Python dependencies required
- **Automatic Model Conversion**: Integrated into face detection system
- **Standalone Converter**: Command-line tool for model conversion
- **Validation**: Built-in ONNX model validation
- **Flexible Input Shapes**: Support for custom input dimensions
- **Error Handling**: Comprehensive error handling and fallback options

## Prerequisites

- LibTorch (PyTorch C++ API)
- CMake 3.16+
- C++17 compiler
- OpenCV 4.12.0 (for face detection integration)

## Installation

### 1. Install LibTorch

Run the installation script:
```bash
./install_libtorch.sh
```

This script will:
- Download LibTorch CPU version
- Extract it to the `libtorch/` directory
- Update CMakeLists.txt with LibTorch configuration

### 2. Build the Project

```bash
./build.sh
```

## Usage

### Standalone Model Converter

Convert a PyTorch model to ONNX format:

```bash
# Basic conversion
./build/model_converter -i models/yolov8_face_model.pt

# Specify output path
./build/model_converter -i models/yolov8_face_model.pt -o models/face_model.onnx

# Custom input shape
./build/model_converter -i models/yolov8_face_model.pt -s 1,3,640,640

# With validation
./build/model_converter -i models/yolov8_face_model.pt -v
```

### Command Line Options

```bash
./build/model_converter [OPTIONS]
```

**Options:**
- `-i, --input PATH`       Input PyTorch model path
- `-o, --output PATH`      Output ONNX model path (optional)
- `-s, --shape W,H,D`      Input shape (default: 1,3,640,640)
- `-v, --validate`         Validate the converted model
- `-h, --help`             Show help message

### Integration with Face Detection

The converter is automatically integrated into the face detection system:

```bash
# Face detection will automatically convert PyTorch models
./build/face_detection -w 0 -m models/yolov8_face_model.pt
```

## Implementation Details

### Core Components

1. **PyTorchConverter Class** (`pytorch_converter.h/cpp`):
   - Main conversion logic
   - Support for different model formats
   - Error handling and validation

2. **Model Converter Executable** (`model_converter_main.cpp`):
   - Standalone command-line tool
   - Flexible input/output options
   - Built-in validation

3. **Face Detection Integration** (`yolo_face_detector.cpp`):
   - Automatic model conversion
   - Seamless integration with detection pipeline

### Conversion Process

1. **Model Loading**: Load PyTorch model using LibTorch
2. **Format Detection**: Detect model format (TorchScript or StateDict)
3. **Conversion**: Convert to ONNX format
4. **Validation**: Validate the converted model
5. **Integration**: Use converted model for inference

### Supported Model Formats

- **TorchScript Models**: Direct loading and conversion
- **StateDict Models**: Load weights and create model structure
- **YOLOv8 Models**: Specialized handling for YOLO architecture

## Error Handling

The converter includes comprehensive error handling:

- **Model Loading Errors**: Fallback to alternative loading methods
- **Conversion Errors**: Detailed error messages and suggestions
- **Validation Errors**: File integrity and format validation
- **Memory Errors**: Proper resource cleanup

## Performance

- **CPU Optimized**: Uses LibTorch CPU backend
- **Memory Efficient**: Minimal memory footprint during conversion
- **Fast Conversion**: Optimized for YOLO model architectures

## Troubleshooting

### Common Issues

1. **LibTorch Not Found**:
   ```bash
   # Reinstall LibTorch
   ./install_libtorch.sh
   ```

2. **Model Loading Failed**:
   ```bash
   # Check model format
   file models/yolov8_face_model.pt
   ```

3. **Conversion Errors**:
   ```bash
   # Try with validation
   ./build/model_converter -i model.pt -v
   ```

### Debug Information

Enable verbose output:
```bash
# Set environment variable for debug info
export TORCH_CPP_LOG_LEVEL=INFO
./build/model_converter -i model.pt
```

## File Structure

```
OpenCV/
├── libtorch/                    # LibTorch installation
├── build/
│   ├── model_converter         # Standalone converter
│   └── face_detection         # Face detection with converter
├── src/
│   ├── pytorch_converter.cpp   # Core conversion logic
│   ├── model_converter_main.cpp # Standalone converter
│   └── yolo_face_detector.cpp  # Face detector with conversion
├── include/
│   └── pytorch_converter.h     # Converter header
├── install_libtorch.sh         # LibTorch installer
└── CPP_CONVERTER_README.md     # This file
```

## Comparison with Python Converter

| Feature | Python Converter | C++ Converter |
|---------|------------------|---------------|
| Dependencies | Python + ultralytics | LibTorch only |
| Performance | Slower | Faster |
| Integration | External script | Native integration |
| Memory Usage | Higher | Lower |
| Deployment | Requires Python | Standalone binary |

## Advanced Usage

### Custom Model Architectures

For custom model architectures, modify the `createYOLOv8Model()` function:

```cpp
std::shared_ptr<torch::nn::Module> PyTorchConverter::createYOLOv8Model() {
    // Define your custom model architecture here
    struct CustomModel : torch::nn::Module {
        // Your model layers
        torch::nn::Sequential backbone{nullptr};
        
        CustomModel() {
            // Define your layers
            backbone = torch::nn::Sequential(
                torch::nn::Conv2d(torch::nn::Conv2dOptions(3, 64, 3).padding(1)),
                torch::nn::ReLU()
                // Add more layers as needed
            );
            register_module("backbone", backbone);
        }
        
        torch::Tensor forward(torch::Tensor x) {
            return backbone->forward(x);
        }
    };
    
    return std::make_shared<CustomModel>();
}
```

### Batch Processing

Convert multiple models:

```bash
# Convert all .pt files in models directory
for file in models/*.pt; do
    ./build/model_converter -i "$file" -v
done
```

## Integration with Other Projects

The converter can be easily integrated into other C++ projects:

1. **Include the converter files** in your project
2. **Link against LibTorch** libraries
3. **Use the PyTorchConverter class** for model conversion

```cpp
#include "pytorch_converter.h"

PyTorchConverter converter;
bool success = converter.convertToOnnx("model.pt", "model.onnx");
```

## Future Enhancements

- **GPU Support**: CUDA-enabled conversion
- **More Model Formats**: Support for additional PyTorch formats
- **Optimization**: Model optimization during conversion
- **Quantization**: Support for quantized models

