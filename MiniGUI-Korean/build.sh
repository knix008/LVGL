#!/bin/bash

# MiniGUI Application Run Script
# This script downloads, builds MiniGUI locally, builds the application, and runs it

set -e  # Exit on any error

# Get the directory where this script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Local directories
MINIGUI_DIR="./minigui"
INSTALL_DIR="./install"

echo "=== MiniGUI Local Setup and Run Script ==="
echo "Working directory: $SCRIPT_DIR"
echo "Local MiniGUI directory: $MINIGUI_DIR"
echo ""
echo "Note: This script will install system packages using sudo."
echo "You may be prompted for your password during package installation."
echo ""

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Install required system packages
echo "Installing required system packages..."
echo "This will install development tools and libraries needed for MiniGUI."

# Check if we can install packages
if command_exists apt-get; then
    echo "Installing packages using apt-get..."
    echo "Note: This requires sudo privileges. You may be prompted for your password."
    if sudo apt-get update && sudo apt-get install -y \
        git gcc g++ binutils autoconf automake libtool make cmake pkg-config \
        libgtk2.0-dev libjpeg-dev libpng-dev libfreetype6-dev libinput-dev libdrm-dev \
        libsqlite3-dev libxml2-dev libssl-dev libx11-dev libxext-dev libxrender-dev \
        libxrandr-dev libxinerama-dev libxi-dev libxcursor-dev libxfixes-dev \
        libharfbuzz-dev libpixman-1-dev libwebp-dev libudev-dev libpciaccess-dev \
        xvfb x11-utils; then
        echo "✓ System packages installed"
        echo "✓ Xvfb virtual display server installed"
    else
        echo "⚠ Package installation failed. Please install packages manually:"
        echo "  sudo apt-get update"
        echo "  sudo apt-get install -y git gcc g++ binutils autoconf automake libtool make cmake pkg-config \\"
        echo "    libgtk2.0-dev libjpeg-dev libpng-dev libfreetype6-dev libinput-dev libdrm-dev \\"
        echo "    libsqlite3-dev libxml2-dev libssl-dev libx11-dev libxext-dev libxrender-dev \\"
        echo "    libxrandr-dev libxinerama-dev libxi-dev libxcursor-dev libxfixes-dev \\"
        echo "    libharfbuzz-dev libpixman-1-dev libwebp-dev libudev-dev libpciaccess-dev \\"
        echo "    xvfb x11-utils"
        echo "Continuing with build process..."
    fi
elif command_exists yum; then
    echo "Installing packages using yum..."
    echo "Note: This requires sudo privileges. You may be prompted for your password."
    if sudo yum groupinstall -y "Development Tools" && sudo yum install -y \
        git gcc gcc-c++ binutils autoconf automake libtool make cmake pkgconfig \
        gtk2-devel libjpeg-turbo-devel libpng-devel freetype-devel libinput-devel libdrm-devel \
        sqlite-devel libxml2-devel openssl-devel libX11-devel libXext-devel libXrender-devel \
        libXrandr-devel libXinerama-devel libXi-devel libXcursor-devel libXfixes-devel \
        harfbuzz-devel pixman-devel libwebp-devel systemd-devel libpciaccess-devel \
        xorg-x11-server-Xvfb xorg-x11-utils; then
        echo "✓ System packages installed"
        echo "✓ Xvfb virtual display server installed"
    else
        echo "⚠ Package installation failed. Please install packages manually."
        echo "Continuing with build process..."
    fi
elif command_exists dnf; then
    echo "Installing packages using dnf..."
    echo "Note: This requires sudo privileges. You may be prompted for your password."
    if sudo dnf groupinstall -y "Development Tools" && sudo dnf install -y \
        git gcc gcc-c++ binutils autoconf automake libtool make cmake pkgconfig \
        gtk2-devel libjpeg-turbo-devel libpng-devel freetype-devel libinput-devel libdrm-devel \
        sqlite-devel libxml2-devel openssl-devel libX11-devel libXext-devel libXrender-devel \
        libXrandr-devel libXinerama-devel libXi-devel libXcursor-devel libXfixes-devel \
        harfbuzz-devel pixman-devel libwebp-devel systemd-devel libpciaccess-devel \
        xorg-x11-server-Xvfb xorg-x11-utils; then
        echo "✓ System packages installed"
        echo "✓ Xvfb virtual display server installed"
    else
        echo "⚠ Package installation failed. Please install packages manually."
        echo "Continuing with build process..."
    fi
else
    echo "Warning: No supported package manager found (apt-get, yum, dnf)."
    echo "Please install the following packages manually:"
    echo "  - git, gcc, g++, make, cmake, pkg-config"
    echo "  - Development libraries: libjpeg, libpng, freetype, libinput, libdrm"
    echo "  - X11 libraries: libx11, libxext, libxrender, libxrandr, libxinerama, libxi"
    echo "  - Other: libharfbuzz, libpixman-1, libwebp, libudev, libpciaccess"
    echo "  - Virtual display: xvfb, x11-utils"
    echo ""
    echo "Continuing with build process..."
fi

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

# Check for xvfb availability
echo "Checking for Xvfb virtual display server..."
if command_exists Xvfb; then
    echo "✓ Xvfb is available"
    echo "Note: You can use 'Xvfb :99 -screen 0 800x600x16 &' to start a virtual display"
    echo "Then set DISPLAY=:99 to use the virtual display"
else
    echo "⚠ Xvfb not found. MiniGUI will use dummy engine instead."
    echo "To use pc_xvfb engine, install xvfb and start it with:"
    echo "  Xvfb :99 -screen 0 800x600x16 &"
    echo "  export DISPLAY=:99"
fi
echo ""

# Clean previous builds for fresh start
echo "Cleaning previous builds..."
rm -rf "$INSTALL_DIR"
rm -rf "$MINIGUI_DIR"

# Ensure we use only the local MiniGUI.cfg file
echo "Using local MiniGUI.cfg configuration..."

# Create local directories
echo "Creating local directories..."
mkdir -p "$MINIGUI_DIR"
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
        --enable-shared \
        --enable-unicodesupport \
        --enable-fullunicode \
        --enable-upfsupport \
        --enable-iso8859-1 \
        --enable-korean-support
    
    # Build MiniGUI
    echo "Building MiniGUI (this may take a while)..."
    make -j$(nproc)
    
    # Install MiniGUI locally
    echo "Installing MiniGUI locally..."
    make install
    
    cd "$SCRIPT_DIR"
    echo "✓ MiniGUI built and installed locally"
    
    # Remove any MiniGUI.cfg files that might have been installed
    # We want to use only our local configuration
    echo "Removing installed MiniGUI.cfg files to use local configuration..."
    rm -f "$INSTALL_DIR/etc/MiniGUI.cfg" 2>/dev/null || true
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

# Force MiniGUI to use dummy engine (fallback to pc_xvfb if available)
# Note: pc_xvfb requires xvfb to be running, dummy works without display
export MG_GAL_ENGINE="dummy"
export MG_IAL_ENGINE="dummy"

# Set default display mode
export MG_DEFAULTMODE="800x600-16bpp"

# Set configuration file path
export MG_CONFIG_FILE="./MiniGUI.cfg"

echo "Build completed successfully!"
echo ""
echo "=== Engine Configuration ==="
echo "Current configuration uses 'dummy' engine for maximum compatibility."
echo "If you want to use pc_xvfb engine (requires Xvfb):"
echo "  1. Start Xvfb: Xvfb :99 -screen 0 800x600x16 &"
echo "  2. Set display: export DISPLAY=:99"
echo "  3. Update run.sh to use: MG_GAL_ENGINE=pc_xvfb MG_IAL_ENGINE=pc_xvfb"
echo ""
echo "To run the application, use: ./run.sh"