#!/bin/bash

# ONNX Runtime Build Script
set -e

echo "=== Setting up ONNX Runtime ==="

# Configuration
ONNXRUNTIME_VERSION="1.16.3"
ONNXRUNTIME_DIR="onnxruntime"
ONNXRUNTIME_ARCHIVE="onnxruntime-linux-x64-${ONNXRUNTIME_VERSION}.tgz"
ONNXRUNTIME_URL="https://github.com/microsoft/onnxruntime/releases/download/v${ONNXRUNTIME_VERSION}/${ONNXRUNTIME_ARCHIVE}"

# Check if ONNX Runtime directory exists
if [ ! -d "${ONNXRUNTIME_DIR}" ]; then
    echo "Error: ONNX Runtime directory ${ONNXRUNTIME_DIR} not found!"
    echo "Please ensure the ONNX Runtime is in the Source directory."
    exit 1
fi

echo "=== ONNX Runtime setup completed successfully ==="
echo "ONNX Runtime installed to: ${ONNXRUNTIME_DIR}/"
echo "Include directory: ${ONNXRUNTIME_DIR}/include/"
echo "Library directory: ${ONNXRUNTIME_DIR}/lib/"
