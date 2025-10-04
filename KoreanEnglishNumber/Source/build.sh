#!/bin/bash

# Build script for Korean Input System

# Help function
show_help() {
    echo "Usage: $0 [clean]"
    echo ""
    echo "Commands:"
    echo "  (no args)  Build the project"
    echo "  clean      Clean all build artifacts including executables"
    echo "  help       Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0         # Normal build"
    echo "  $0 clean   # Clean build directory"
}

# Clean function
clean_build() {
    echo "Cleaning Korean Input System build artifacts..."
    
    # Get the directory where this script is located
    SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
    SOURCE_DIR="$SCRIPT_DIR"
    BUILD_DIR="$SOURCE_DIR/build"
    
    if [ -d "$BUILD_DIR" ]; then
        echo "Removing build directory: $BUILD_DIR"
        rm -rf "$BUILD_DIR"
        echo "Build directory cleaned successfully!"
    else
        echo "Build directory does not exist."
    fi
    
    # Also remove any executables that might be in the source directory
    echo "Removing any executables in source directory..."
    cd "$SOURCE_DIR"
    rm -f main test_* calendar_test 2>/dev/null
    
    echo "Clean completed!"
}

# Check for command line arguments
if [ "$1" = "clean" ]; then
    clean_build
    exit 0
elif [ "$1" = "help" ] || [ "$1" = "-h" ] || [ "$1" = "--help" ]; then
    show_help
    exit 0
elif [ $# -gt 0 ]; then
    echo "Unknown command: $1"
    show_help
    exit 1
fi

# Build process starts here
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