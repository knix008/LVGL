# YOLO to ONNX Conversion Scripts

This directory contains scripts to convert YOLO PyTorch (.pt) models to ONNX format for use with the webcam application.

## Files

- `convert_yolo_to_onnx.py` - Main Python conversion script
- `convert_yolo.sh` - Shell script wrapper for easier usage
- `README_YOLO_CONVERSION.md` - This documentation file

## Prerequisites

The scripts require the following Python packages:
- `ultralytics` - For YOLO model handling
- `onnx` - For ONNX model validation

These will be automatically installed if missing.

## Usage

### Using the Python Script Directly

```bash
# Basic conversion
python3 convert_yolo_to_onnx.py input_model.pt output_model.onnx

# With custom image size
python3 convert_yolo_to_onnx.py input_model.pt output_model.onnx --img-size 416

# Using named arguments
python3 convert_yolo_to_onnx.py --input input_model.pt --output output_model.onnx --img-size 640

# Check dependencies
python3 convert_yolo_to_onnx.py --check-deps
```

### Using the Shell Script Wrapper

```bash
# Basic conversion
./convert_yolo.sh input_model.pt output_model.onnx

# With custom parameters
./convert_yolo.sh input_model.pt output_model.onnx --img-size 416 --no-simplify

# Check dependencies
./convert_yolo.sh --check-deps
```

## Command Line Options

| Option | Description | Default |
|--------|-------------|---------|
| `--img-size, -s` | Input image size (width=height) | 640 |
| `--no-simplify` | Disable ONNX model simplification | False |
| `--opset` | ONNX opset version | 11 |
| `--check-deps` | Check and install dependencies | - |

## Examples

### Convert YOLOv8n Model
```bash
./convert_yolo.sh models/yolov8n.pt models/yolov8n.onnx
```

### Convert Face Detection Model
```bash
./convert_yolo.sh models/model.pt models/face_detection.onnx --img-size 640
```

### Convert with Custom Parameters
```bash
./convert_yolo.sh models/yolov8s.pt models/yolov8s.onnx --img-size 416 --opset 12
```

## Model Sources

### General YOLO Models
- [Ultralytics YOLOv8](https://github.com/ultralytics/ultralytics) - Official YOLOv8 models
- Download: `wget https://github.com/ultralytics/assets/releases/download/v0.0.0/yolov8n.pt`

### Specialized Models
- [YOLOv8-Face-Detection](https://huggingface.co/arnabdhar/YOLOv8-Face-Detection) - Face detection model
- Download: `wget https://huggingface.co/arnabdhar/YOLOv8-Face-Detection/resolve/main/model.pt`

## Integration with Webcam Application

After converting a model to ONNX format, update the webcam application to use the new model:

1. Place the ONNX file in the `models/` directory
2. Update the model path in `src/webcam_ipc_app.cpp`:
   ```cpp
   m_model_path = "../models/your_model.onnx";
   ```
3. Rebuild the webcam application:
   ```bash
   cd build && make webcam_ipc_app
   ```

## Troubleshooting

### Common Issues

1. **Missing Dependencies**: Run `./convert_yolo.sh --check-deps` to install required packages
2. **Permission Errors**: Make sure scripts are executable: `chmod +x convert_yolo.sh`
3. **Model Not Found**: Verify the input model file exists and is a valid YOLO model
4. **ONNX Validation Failed**: Try using `--no-simplify` option or different opset version

### Performance Notes

- Larger models (YOLOv8s, YOLOv8m, etc.) will be slower but more accurate
- Smaller input sizes (416, 320) will be faster but less accurate
- ONNX simplification reduces model size but may affect accuracy slightly

## File Structure

```
Source/
├── convert_yolo_to_onnx.py    # Main conversion script
├── convert_yolo.sh            # Shell script wrapper
├── README_YOLO_CONVERSION.md  # This documentation
└── models/                    # Model directory
    ├── yolov8n.pt            # YOLOv8n PyTorch model
    ├── yolov8n.onnx          # YOLOv8n ONNX model
    ├── model.pt              # Face detection PyTorch model
    └── model.onnx            # Face detection ONNX model
```
