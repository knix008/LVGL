#!/usr/bin/env python3
"""
Convert YOLOv8 Face PyTorch model to ONNX format
"""

import torch
import sys
import os

def convert_pytorch_to_onnx(model_path, output_path):
    """Convert PyTorch model to ONNX format"""
    try:
        print(f"Loading PyTorch model from: {model_path}")
        
        # Load the PyTorch model with weights_only=False for Ultralytics models
        model = torch.load(model_path, map_location='cpu', weights_only=False)
        
        # Set model to evaluation mode
        if hasattr(model, 'eval'):
            model.eval()
        
        # Create dummy input tensor
        dummy_input = torch.randn(1, 3, 640, 640)
        
        print(f"Converting to ONNX format...")
        
        # Export to ONNX
        torch.onnx.export(
            model,
            dummy_input,
            output_path,
            export_params=True,
            opset_version=11,
            do_constant_folding=True,
            input_names=['input'],
            output_names=['output'],
            dynamic_axes={
                'input': {0: 'batch_size'},
                'output': {0: 'batch_size'}
            }
        )
        
        print(f"ONNX model saved to: {output_path}")
        return True
        
    except Exception as e:
        print(f"Error converting model: {e}")
        print("Trying alternative approach...")
        
        try:
            # Try with ultralytics import
            from ultralytics import YOLO
            
            print("Loading model with Ultralytics YOLO...")
            model = YOLO(model_path)
            
            # Export to ONNX
            print("Exporting to ONNX...")
            model.export(format='onnx', imgsz=640)
            
            # Check if the ONNX file was created
            onnx_path = model_path.replace('.pt', '.onnx')
            if os.path.exists(onnx_path):
                # Move to our desired location
                os.rename(onnx_path, output_path)
                print(f"ONNX model saved to: {output_path}")
                return True
            else:
                print("ONNX export failed")
                return False
                
        except ImportError:
            print("Ultralytics not available. Please install it with: pip install ultralytics")
            return False
        except Exception as e2:
            print(f"Alternative approach also failed: {e2}")
            return False

def main():
    model_path = "models/yolov8_face_model.pt"
    output_path = "models/yolov8_face_model.onnx"
    
    # Check if input model exists
    if not os.path.exists(model_path):
        print(f"Error: Model file not found: {model_path}")
        sys.exit(1)
    
    # Convert model
    if convert_pytorch_to_onnx(model_path, output_path):
        print("Conversion completed successfully!")
        print(f"ONNX model ready for use: {output_path}")
    else:
        print("Conversion failed!")
        print("\nManual conversion instructions:")
        print("1. Install ultralytics: pip install ultralytics")
        print("2. Run: python -c \"from ultralytics import YOLO; YOLO('models/yolov8_face_model.pt').export(format='onnx')\"")
        sys.exit(1)

if __name__ == "__main__":
    main()
