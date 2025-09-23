#!/bin/bash

# MiniGUI Application Run Script
# This script downloads, builds MiniGUI locally, builds the application, and runs it

set -e  # Exit on any error

# Get the directory where this script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Local directories
LOCAL_DIR="./local"
MINIGUI_DIR="$LOCAL_DIR/minigui"
BUILD_DIR="$LOCAL_DIR/build"
INSTALL_DIR="$LOCAL_DIR/install"

echo "=== MiniGUI Local Setup and Run Script ==="
echo "Working directory: $SCRIPT_DIR"
echo "Local MiniGUI directory: $MINIGUI_DIR"
echo ""

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Check for required tools
echo "Checking for required tools..."
if ! command_exists git; then
    echo "Error: git is not installed. Please install git first."
    exit 1
fi

if ! command_exists gcc; then
    echo "Error: gcc is not installed. Please install gcc first."
    exit 1
fi

if ! command_exists make; then
    echo "Error: make is not installed. Please install make first."
    exit 1
fi

echo "✓ All required tools found"
echo ""

# Create local directories
echo "Creating local directories..."
mkdir -p "$MINIGUI_DIR"
mkdir -p "$BUILD_DIR"
mkdir -p "$INSTALL_DIR"

# Download MiniGUI source if not already present
if [ ! -d "$MINIGUI_DIR/.git" ]; then
    echo "Downloading MiniGUI source code..."
    git clone https://github.com/VincentWei/minigui.git "$MINIGUI_DIR"
    echo "✓ MiniGUI source downloaded"
else
    echo "✓ MiniGUI source already exists"
fi

# Build MiniGUI if not already built
if [ ! -f "$INSTALL_DIR/lib/libminigui_sa.so" ]; then
    echo "Building MiniGUI..."
    cd "$MINIGUI_DIR"
    
    # Configure MiniGUI for standalone mode
    echo "Configuring MiniGUI..."
    ./autogen.sh
    ./configure \
        --prefix="$SCRIPT_DIR/$INSTALL_DIR" \
        --with-runmode=sa \
        --disable-videofbcon \
        --enable-videopcxvfb \
        --disable-static \
        --enable-shared
    
    # Build MiniGUI
    echo "Building MiniGUI (this may take a while)..."
    make -j$(nproc)
    
    # Install MiniGUI locally
    echo "Installing MiniGUI locally..."
    make install
    
    cd "$SCRIPT_DIR"
    echo "✓ MiniGUI built and installed locally"
else
    echo "✓ MiniGUI already built"
fi

# Check if Makefile exists
if [ ! -f "Makefile" ]; then
    echo "Error: Makefile not found. Please ensure Makefile exists in the project directory."
    exit 1
fi

# Build the application
echo "Building the MiniGUI application..."
make clean
make

echo "✓ Application built successfully"
echo ""

# Set up environment for running
echo "Setting up environment..."

# Set library path to local installation
export LD_LIBRARY_PATH="$SCRIPT_DIR/$INSTALL_DIR/lib:$LD_LIBRARY_PATH"

# Suppress GTK and PNG warnings
export GTK_MODULES=""
export PNG_SKIP_sRGB_CHECK=1
export GTK_DEBUG="no-css-cache"
export MALLOC_CHECK_=0

# Set MiniGUI runtime mode to standalone
export MG_RUNTIME_MODE="standalone"

# Force MiniGUI to use pc_xvfb (PC Virtual FrameBuffer) engine
export MG_GAL_ENGINE="pc_xvfb"
export MG_IAL_ENGINE="pc_xvfb"

# Set default display mode
export MG_DEFAULTMODE="800x600-16bpp"

# Set configuration file path
export MG_CONFIG_FILE="./MiniGUI.cfg"

# Create a virtual display if X11 is not available
if [ -z "$DISPLAY" ]; then
    echo "No X11 display found. Starting Xvfb virtual display..."
    export DISPLAY=":99"
    Xvfb :99 -screen 0 800x600x16 &
    XVFB_PID=$!
    sleep 2
fi

echo "Starting MiniGUI Hello World application..."
echo "Library path: $LD_LIBRARY_PATH"
echo "GAL engine: $MG_GAL_ENGINE"
echo "IAL engine: $MG_IAL_ENGINE"
echo "Display: $DISPLAY"
echo ""

# Run the application
./helloworld

# Clean up Xvfb if we started it
if [ ! -z "$XVFB_PID" ]; then
    echo "Stopping virtual display..."
    kill $XVFB_PID
fi

echo "Application finished."