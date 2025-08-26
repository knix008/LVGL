#!/bin/bash

# YOLO to ONNX Converter Shell Script
# This is a simple wrapper around the Python conversion script

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PYTHON_SCRIPT="$SCRIPT_DIR/convert_yolo_to_onnx.py"

# Check if Python script exists
if [ ! -f "$PYTHON_SCRIPT" ]; then
    echo "Error: Python conversion script not found: $PYTHON_SCRIPT"
    exit 1
fi

# Make sure Python script is executable
chmod +x "$PYTHON_SCRIPT"

# Pass all arguments to the Python script
python3 "$PYTHON_SCRIPT" "$@"
