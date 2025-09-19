#!/bin/bash

echo "Installing MiniGUI from source..."
echo "This will take some time as it compiles MiniGUI and dependencies."

# Install required development tools and libraries
echo "Installing development tools and dependencies..."
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
./fetch-all.sh

# Build dependencies
echo "Building dependencies (this may take a while)..."
./build-deps.sh

# Build and install MiniGUI
echo "Building and installing MiniGUI (this may take a while)..."
./build-all.sh

# Update library cache
echo "Updating library cache..."
sudo ldconfig

echo "MiniGUI installation completed!"
echo "You can now build your MiniGUI application with: make"
echo "MiniGUI has been installed to /usr/local/"
