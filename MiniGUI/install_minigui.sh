#!/bin/bash

# Exit on any error
set -e

echo "Installing MiniGUI from source..."
echo "This will take some time as it compiles MiniGUI and dependencies."
echo ""

# Check if running as root (not recommended)
if [ "$EUID" -eq 0 ]; then
    echo "Warning: Running as root is not recommended for development."
    echo "Consider running as a regular user with sudo privileges."
    read -p "Continue anyway? (y/N): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

# Install required development tools and libraries
echo "Installing development tools and dependencies..."
echo "Note: This requires sudo privileges. You may be prompted for your password."
sudo apt-get update
sudo apt-get install -y git gcc g++ binutils autoconf automake libtool make cmake pkg-config \
    libgtk2.0-dev libjpeg-dev libpng-dev libfreetype6-dev libinput-dev libdrm-dev \
    libsqlite3-dev libxml2-dev libssl-dev libx11-dev libxext-dev libxrender-dev \
    libxrandr-dev libxinerama-dev libxi-dev libxcursor-dev libxfixes-dev

# Create build directory
echo "Creating build directory..."
mkdir -p ~/minigui-build
cd ~/minigui-build

# Clone the MiniGUI build repository
echo "Cloning MiniGUI build repository..."
if [ -d "build-minigui-4.0" ]; then
    echo "Repository already exists. Removing old directory..."
    rm -rf build-minigui-4.0
fi
git clone https://github.com/VincentWei/build-minigui-4.0.git
cd build-minigui-4.0

# Copy and configure the build script
echo "Configuring build script..."
cp config.sh myconfig.sh

# Edit configuration for standalone mode
echo "Configuring for standalone mode..."
sed -i 's/runmode=.*/runmode=standalone-shared/' myconfig.sh

# Fetch source code
echo "Fetching MiniGUI source code..."
echo "Note: This may show some 'directory already exists' messages, which is normal."
echo "These messages are harmless and can be ignored."
./fetch-all.sh 2>/dev/null || {
    echo "Warning: Some repositories may already exist. This is normal."
    echo "Continuing with the build process..."
}

# Build dependencies
echo "Building dependencies (this may take a while)..."
./build-deps.sh

# Build and install MiniGUI
echo "Building and installing MiniGUI (this may take a while)..."
./build-all.sh

# Update library cache
echo "Updating library cache..."
echo "Note: This requires sudo privileges."
sudo ldconfig

echo "MiniGUI installation completed!"
echo "You can now build your MiniGUI application with: make"
echo "MiniGUI has been installed to /usr/local/"
