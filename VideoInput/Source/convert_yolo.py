#!/usr/bin/env python3

import torch
from ultralytics import YOLO
import os

def convert_yolo_to_onnx():
    print("Converting YOLOv8 PyTorch model to ONNX format...")
    
    # Load the YOLOv8 model
    model = YOLO('models/yolov8n.pt')
    
    # Export to ONNX format
    success = model.export(format='onnx', 
                          imgsz=640, 
                          simplify=True, 
                          opset=11,
                          dynamic=True,
                          half=False)
    
    if success:
        print("YOLOv8 model successfully converted to ONNX format!")
        print("ONNX model saved as: models/yolov8n.onnx")
    else:
        print("Failed to convert YOLOv8 model to ONNX format!")

if __name__ == "__main__":
    convert_yolo_to_onnx() 