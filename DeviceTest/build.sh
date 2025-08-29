#!/bin/bash

# Device Test Program Build Script

set -e  # Exit on any error

echo "=== Device Test Program Build Script ==="

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    echo "Error: CMakeLists.txt not found. Please run this script from the project root directory."
    exit 1
fi

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir build
fi

# Configure with CMake
echo "Configuring with CMake..."
cd build
cmake ..
if [ $? -ne 0 ]; then
    echo "Error: CMake configuration failed."
    exit 1
fi

# Build the project
echo "Building the project..."
make
if [ $? -ne 0 ]; then
    echo "Error: Build failed."
    exit 1
fi

echo ""
echo "=== Build completed successfully! ==="
echo "Executable location: build/bin/DeviceTest"
echo ""
echo "Usage examples:"
echo "  ./bin/DeviceTest -h                    # Show help"
echo "  ./bin/DeviceTest -t auto               # Run automated test suite"
echo "  ./bin/DeviceTest -t auto -n eth0       # Run automated test suite with specific interface"
echo "  ./bin/DeviceTest                       # Run all camera tests on camera 0"
echo "  ./bin/DeviceTest -d network -n eth0 -t all  # Run all network tests on eth0"
echo "  ./bin/DeviceTest -d network -n wlan0 -t connectivity  # Test connectivity on wlan0"
echo "  ./bin/DeviceTest -i                    # Start interactive camera mode"
echo "  ./bin/DeviceTest -d network -n eth0 -i # Start interactive network mode"
echo ""
