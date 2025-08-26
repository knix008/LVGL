#!/bin/bash

# ONNX Runtime Build Script
set -e

echo "=== Setting up ONNX Runtime ==="

# Configuration
ONNXRUNTIME_VERSION="1.16.3"
ONNXRUNTIME_DIR="onnxruntime"
ONNXRUNTIME_ARCHIVE="onnxruntime-linux-x64-${ONNXRUNTIME_VERSION}.tgz"
ONNXRUNTIME_URL="https://github.com/microsoft/onnxruntime/releases/download/v${ONNXRUNTIME_VERSION}/${ONNXRUNTIME_ARCHIVE}"

# Configuration
LIB_DIR="lib"
mkdir -p ${LIB_DIR}/include
mkdir -p ${LIB_DIR}/lib

# Check if ONNX Runtime directory exists
if [ ! -d "${ONNXRUNTIME_DIR}" ]; then
    echo "Error: ONNX Runtime directory ${ONNXRUNTIME_DIR} not found!"
    echo "Please ensure the ONNX Runtime is in the Source directory."
    exit 1
fi

echo "Copying ONNX Runtime to lib directory..."

# Copy headers
mkdir -p ${LIB_DIR}/include/onnxruntime
cp -r ${ONNXRUNTIME_DIR}/include/* ${LIB_DIR}/include/onnxruntime/

# Copy library
cp ${ONNXRUNTIME_DIR}/lib/* ${LIB_DIR}/lib/

echo "=== ONNX Runtime setup completed successfully ==="
echo "ONNX Runtime installed to: ${LIB_DIR}/"
echo "Include directory: ${LIB_DIR}/include/onnxruntime/"
echo "Library directory: ${LIB_DIR}/lib/"
