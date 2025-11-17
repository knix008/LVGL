#!/bin/bash

# GTK Webcam Setup Script

set -e

echo "GTK Webcam Viewer - Setup Script"
echo "================================="

# Detect OS and Architecture
if [ -f /etc/os-release ]; then
    . /etc/os-release
    OS=$ID
    VERSION=$VERSION_ID
fi

# Detect Architecture
ARCH=$(uname -m)

echo "Detected OS: $OS (Version: $VERSION)"
echo "Detected Architecture: $ARCH"

# Check if running in VirtualBox
if grep -q "VirtualBox" /sys/class/dmi/id/product_name 2>/dev/null; then
    echo "Running in VirtualBox detected"
elif grep -q "VirtualBox" /sys/class/dmi/id/sys_vendor 2>/dev/null; then
    echo "Running in VirtualBox detected"
else
    VBOX_CHECK=$(dmidecode 2>/dev/null | grep -i virtualbox || echo "")
    if [ -n "$VBOX_CHECK" ]; then
        echo "Running in VirtualBox detected"
    fi
fi

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
            libsqlite3-dev \
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
            sqlite-devel \
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
            sqlite \
            pkg-config
        ;;
    darwin)
        echo "Installing dependencies for macOS..."

        # Check if Homebrew is installed
        if ! command -v brew &> /dev/null; then
            echo "Homebrew not found. Installing Homebrew..."
            /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
        fi

        # Detect architecture
        if [ "$ARCH" = "arm64" ]; then
            echo "Detected Apple Silicon (ARM64) architecture"
            # For Apple Silicon, use native Homebrew path
            BREW_PATH="/opt/homebrew"
        else
            echo "Detected Intel (x86_64) architecture"
            # For Intel Macs, use standard Homebrew path
            BREW_PATH="/usr/local"
        fi

        echo "Installing dependencies using Homebrew..."
        brew install gtk+3 opencv sqlite pkg-config cmake

        echo "Setting up environment variables..."
        export PATH="$BREW_PATH/bin:$PATH"
        export PKG_CONFIG_PATH="$BREW_PATH/lib/pkgconfig:$PKG_CONFIG_PATH"

        echo "Architecture: $ARCH"
        echo "Homebrew path: $BREW_PATH"
        ;;
    *)
        echo "Unsupported OS: $OS"
        echo "Detected Architecture: $ARCH"
        echo ""
        echo "Please install the following packages manually:"
        echo "  - build-essential (or equivalent)"
        echo "  - cmake"
        echo "  - gtk3 development files"
        echo "  - gdk-pixbuf2 development files"
        echo "  - opencv development files"
        echo "  - sqlite3 development files"
        echo "  - pkg-config"
        echo ""
        echo "For macOS (both Intel and Apple Silicon):"
        echo "  1. Install Homebrew: https://brew.sh"
        echo "  2. Run: brew install gtk+3 opencv sqlite pkg-config cmake"
        echo "  3. Continue with building the project"
        exit 1
        ;;
esac

# Add user to video group for camera access (Linux only)
if [ "$OS" != "darwin" ]; then
    echo "Adding current user to video group..."
    sudo usermod -a -G video $USER || true
    echo "NOTE: You may need to log out and log back in for video group changes to take effect."
else
    echo "macOS detected - video group permissions not needed (camera access handled by system)"
fi

echo ""
echo "Setup Complete!"
echo "=============="
echo ""
echo "To start building:"
echo "  cd /path/to/GTK-Camaera"
echo "  make clean && make"
echo ""
echo "To run the application:"
echo "  ./gtk_webcam"
echo ""
echo "Architecture Support:"
echo "  ✅ Linux x86_64 (Intel)"
echo "  ✅ Linux ARM64 (Raspberry Pi 4, etc.)"
echo "  ✅ macOS Intel (x86_64)"
echo "  ✅ macOS Apple Silicon (ARM64)"
