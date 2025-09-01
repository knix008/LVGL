#!/bin/bash

# Project Setup Script for New Computers
# This script builds all required libraries before building the main application

set -e

echo "=== FaceRecognition2.0-MariaDB Project Setup ==="
echo "This script will build all required libraries for the project."
echo ""

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    echo "Error: Please run this script from the Source directory!"
    echo "Current directory: $(pwd)"
    echo "Expected: .../Source/"
    exit 1
fi

# Make all build scripts executable
echo "Making build scripts executable..."
chmod +x build_*.sh
chmod +x install_openssl.sh

# Build all libraries
echo ""
echo "Building all required libraries..."
echo "This may take several minutes..."
echo ""

./build_all_libs.sh all

echo ""
echo "=== Library Build Complete ==="
echo ""
echo "Now you can build the main application:"
echo "  mkdir -p build"
echo "  cd build"
echo "  cmake .."
echo "  make"
echo ""
echo "Or use the run script:"
echo "  ./run.sh"
echo ""
echo "=== Setup Complete ==="
