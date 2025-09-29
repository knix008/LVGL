#!/bin/bash

# Certificate Authority Build Script

set -e

# Function to clean generated files
clean_generated_files() {
    echo "Cleaning generated certificate files..."
    
    # Stop any running CA server
    pkill -f ca_server 2>/dev/null || true
    
    # Remove CA files from certs directory
    rm -f certs/ca.crt certs/ca.key certs/ca.db certs/ca.crl
    
    # Remove client certificates and keys
    rm -rf certs/ keys/
    
    # Remove build directory
    rm -rf build/
    
    # Remove temporary files
    rm -f /tmp/cert_response.json /tmp/cert_list.json /tmp/ca.crt /tmp/ca.crl
    
    echo "✓ Cleaned all generated files and directories"
    echo "  - Removed CA files from certs/"
    echo "  - Removed client certificates from certs/"
    echo "  - Removed client keys from keys/"
    echo "  - Removed build artifacts"
    echo "  - Stopped any running CA server"
}

# Check for clean command
if [ "$1" = "clean" ]; then
    clean_generated_files
    exit 0
fi

echo "Building Certificate Authority Server..."

# Check dependencies
echo "Checking dependencies..."

# Check for required commands
for cmd in cmake make gcc; do
    if ! command -v $cmd &> /dev/null; then
        echo "Error: $cmd is not installed"
        exit 1
    fi
done

# Check if dependencies are already installed
echo "Checking for required dependencies..."

# Check if we can install dependencies
if command -v sudo >/dev/null 2>&1; then
    echo "Attempting to install dependencies..."
    sudo apt-get update
    sudo apt-get install -y \
        cmake \
        build-essential \
        libssl-dev \
        libjson-c-dev \
        libsqlite3-dev \
        pkg-config \
        jq
    echo "Dependencies installation completed!"
else
    echo "Please install the following dependencies manually:"
    echo "  sudo apt-get update"
    echo "  sudo apt-get install -y cmake build-essential libssl-dev libjson-c-dev libsqlite3-dev pkg-config jq"
    echo ""
    echo "Then run this script again."
    exit 1
fi

# Check for required libraries
echo "Checking for required libraries..."

# Check OpenSSL
if pkg-config --exists openssl; then
    echo "✓ OpenSSL found"
else
    echo "✗ OpenSSL not found"
    exit 1
fi

# Check JSON-C
if pkg-config --exists json-c; then
    echo "✓ JSON-C found"
else
    echo "✗ JSON-C not found"
    exit 1
fi

# Check SQLite3
if pkg-config --exists sqlite3; then
    echo "✓ SQLite3 found"
else
    echo "✗ SQLite3 not found"
    exit 1
fi

echo "All dependencies verified!"

# Create build directory
echo "Creating build directory..."
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
echo "Building..."
make -j$(nproc)

echo "Build completed successfully!"
echo ""
echo "Usage:"
echo "  ./build.sh          - Build the CA server"
echo "  ./build.sh clean    - Remove all generated files"
echo ""
echo "To run the CA server:"
echo "  cd /home/shkwon/Projects/LVGL/CA"
echo "  ./build/ca_server"
echo ""
echo "The server will:"
echo "  - Create certs/ directory for CA files (ca.crt, ca.key, ca.db) and client certificates"
echo "  - Store client certificates and private keys in the same certs/ directory"
echo "  - Load configuration from config/ca.conf"
echo ""
echo "Additional tools installed:"
echo "  - jq: JSON processor for API testing and data manipulation"
echo ""
echo "To clean all generated files:"
echo "  ./build.sh clean"
echo ""
echo "To install system-wide:"
echo "  sudo make install"
