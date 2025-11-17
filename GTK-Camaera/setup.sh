#!/bin/bash

# GTK Webcam Setup Script

set -e

echo "GTK Webcam Viewer - Setup Script"
echo "================================="

# Detect OS
if [ -f /etc/os-release ]; then
    . /etc/os-release
    OS=$ID
    VERSION=$VERSION_ID
fi

echo "Detected OS: $OS (Version: $VERSION)"

# Install dependencies based on OS
case "$OS" in
    ubuntu|debian)
        echo "Installing dependencies for Ubuntu/Debian..."
        sudo apt-get update
        sudo apt-get install -y \
            build-essential \
            cmake \
            libgtk-3-dev \
            libgdk-pixbuf2.0-dev \
            libopencv-dev \
            pkg-config
        ;;
    fedora|rhel|centos)
        echo "Installing dependencies for Fedora/RHEL..."
        sudo dnf install -y \
            gcc-c++ \
            cmake \
            gtk3-devel \
            gdk-pixbuf2-devel \
            opencv-devel \
            pkg-config
        ;;
    arch)
        echo "Installing dependencies for Arch Linux..."
        sudo pacman -S --noconfirm \
            base-devel \
            cmake \
            gtk3 \
            gdk-pixbuf2 \
            opencv \
            pkg-config
        ;;
    *)
        echo "Unsupported OS: $OS"
        echo "Please install the following packages manually:"
        echo "  - build-essential (or equivalent)"
        echo "  - cmake"
        echo "  - gtk3 development files"
        echo "  - gdk-pixbuf2 development files"
        echo "  - opencv development files"
        echo "  - pkg-config"
        exit 1
        ;;
esac

# Add user to video group for camera access
echo "Adding current user to video group..."
sudo usermod -a -G video $USER || true

echo ""
echo "Setup Complete!"
echo "=============="
echo ""
echo "To start building:"
echo "  cd gtk-webcam"
echo "  make"
echo ""
echo "To run the application:"
echo "  make run"
echo ""
echo "NOTE: You may need to log out and log back in for video group changes to take effect."
