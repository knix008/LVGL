#!/bin/bash

# Build script for zlib library
# This script downloads, configures, and builds zlib from source

set -e  # Exit on any error

# Configuration
ZLIB_VERSION="1.3.1"
ZLIB_URL="https://zlib.net/zlib-${ZLIB_VERSION}.tar.gz"
BUILD_DIR="zlib-${ZLIB_VERSION}"
LIB_DIR="lib"

echo "=== Building zlib ${ZLIB_VERSION} ==="

# Create build directory
if [ -d "$BUILD_DIR" ]; then
    echo "Removing existing build directory..."
    rm -rf "$BUILD_DIR"
fi

# Download zlib
echo "Downloading zlib ${ZLIB_VERSION}..."
wget -O "zlib-${ZLIB_VERSION}.tar.gz" "$ZLIB_URL"

# Extract source
echo "Extracting source..."
tar -xzf "zlib-${ZLIB_VERSION}.tar.gz"

# Enter build directory
cd "$BUILD_DIR"

# Configure zlib
echo "Configuring zlib..."
./configure --prefix="$(pwd)/install" --static

# Build zlib
echo "Building zlib..."
make -j$(nproc)

# Install zlib
echo "Installing zlib..."
make install

# Copy library files to lib directory
echo "Copying library files to ${LIB_DIR}/..."
mkdir -p "../${LIB_DIR}/lib"
mkdir -p "../${LIB_DIR}/include/zlib"

cp install/lib/libz.a "../${LIB_DIR}/lib/"
cp install/include/zlib.h "../${LIB_DIR}/include/zlib/"
cp install/include/zconf.h "../${LIB_DIR}/include/zlib/"

# Verify the files were copied
echo "Verifying copied files..."
ls -la "../${LIB_DIR}/lib/libz.a"
ls -la "../${LIB_DIR}/include/zlib/zlib.h"

# Also copy to pkgconfig directory
mkdir -p "../${LIB_DIR}/lib/pkgconfig"
cp install/lib/pkgconfig/zlib.pc "../${LIB_DIR}/lib/pkgconfig/"

# Go back to original directory and verify
cd ..
echo "Final verification from original directory:"
ls -la "${LIB_DIR}/lib/libz.a"
ls -la "${LIB_DIR}/include/zlib/zlib.h"

# Clean up build directory
rm -rf "$BUILD_DIR"

# Clean up downloaded source archive
rm -f "zlib-${ZLIB_VERSION}.tar.gz"

echo "=== zlib build completed successfully ==="
echo "Library installed to: ${LIB_DIR}/lib/libz.a"
echo "Headers installed to: ${LIB_DIR}/include/zlib/"
