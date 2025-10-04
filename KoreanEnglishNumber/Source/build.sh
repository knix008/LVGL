#!/bin/bash

# Build script for Korean Input System
echo "Building Korean Input System..."

# Install all required dependencies
echo "Checking and installing dependencies..."
PACKAGES="build-essential cmake pkg-config libsdl2-dev libfreetype-dev"
MISSING_PACKAGES=""

for package in $PACKAGES; do
    if ! dpkg -l | grep -q "^ii  $package"; then
        MISSING_PACKAGES="$MISSING_PACKAGES $package"
    fi
done

if [ -n "$MISSING_PACKAGES" ]; then
    echo "Installing missing packages:$MISSING_PACKAGES"
    sudo apt-get update
    sudo apt-get install -y $MISSING_PACKAGES
else
    echo "All dependencies already installed"
fi

# Get the directory where this script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SOURCE_DIR="$SCRIPT_DIR"
BUILD_DIR="$SOURCE_DIR/build"

# Create build directory if it doesn't exist
if [ ! -d "$BUILD_DIR" ]; then
    echo "Creating build directory: $BUILD_DIR"
    mkdir -p "$BUILD_DIR"
fi

# Change to build directory
cd "$BUILD_DIR"

# Configure with cmake
echo "Configuring with cmake..."
cmake "$SOURCE_DIR"

if [ $? -ne 0 ]; then
    echo "CMake configuration failed!"
    exit 1
fi

# Build the project
echo "Building project..."
make -j$(nproc)

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "Available executables:"
    ls -la main test_* calendar_test 2>/dev/null || echo "No executables found"
else
    echo "Build failed!"
    exit 1
fi 