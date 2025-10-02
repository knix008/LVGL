#!/bin/bash

# Script to install dependencies for Device Simulator
# Supports Ubuntu/Debian, CentOS/RHEL/Fedora, and macOS

set -e

echo "=========================================="
echo "Device Simulator - Dependency Installer"
echo "=========================================="
echo ""

# Detect OS
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    # Check if it's Debian/Ubuntu or RHEL/CentOS/Fedora
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        OS=$ID
    else
        echo "Error: Cannot detect Linux distribution"
        exit 1
    fi
elif [[ "$OSTYPE" == "darwin"* ]]; then
    OS="macos"
else
    echo "Error: Unsupported operating system: $OSTYPE"
    exit 1
fi

echo "Detected OS: $OS"
echo ""

# Install dependencies based on OS
case $OS in
    ubuntu|debian)
        echo "Installing dependencies for Ubuntu/Debian..."
        echo ""

        # Update package list
        echo "Updating package list..."
        sudo apt-get update

        # Install build tools
        echo "Installing build tools..."
        sudo apt-get install -y build-essential

        # Install CMake
        echo "Installing CMake..."
        sudo apt-get install -y cmake

        # Install OpenSSL development files
        echo "Installing OpenSSL development libraries..."
        sudo apt-get install -y libssl-dev

        # Install jsoncpp
        echo "Installing jsoncpp library..."
        sudo apt-get install -y libjsoncpp-dev

        # Install pkg-config
        echo "Installing pkg-config..."
        sudo apt-get install -y pkg-config

        echo ""
        echo "✓ All dependencies installed successfully!"
        ;;

    centos|rhel|fedora)
        echo "Installing dependencies for CentOS/RHEL/Fedora..."
        echo ""

        # Determine package manager
        if command -v dnf &> /dev/null; then
            PKG_MGR="dnf"
        else
            PKG_MGR="yum"
        fi

        # Install development tools
        echo "Installing development tools..."
        sudo $PKG_MGR groupinstall -y "Development Tools"

        # Install CMake
        echo "Installing CMake..."
        sudo $PKG_MGR install -y cmake

        # Install OpenSSL development files
        echo "Installing OpenSSL development libraries..."
        sudo $PKG_MGR install -y openssl-devel

        # Install jsoncpp
        echo "Installing jsoncpp library..."
        sudo $PKG_MGR install -y jsoncpp-devel

        # Install pkg-config
        echo "Installing pkg-config..."
        sudo $PKG_MGR install -y pkgconfig

        echo ""
        echo "✓ All dependencies installed successfully!"
        ;;

    macos)
        echo "Installing dependencies for macOS..."
        echo ""

        # Check if Homebrew is installed
        if ! command -v brew &> /dev/null; then
            echo "Error: Homebrew is not installed"
            echo "Please install Homebrew first: https://brew.sh"
            echo ""
            echo "Run this command:"
            echo '/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"'
            exit 1
        fi

        # Update Homebrew
        echo "Updating Homebrew..."
        brew update

        # Install CMake
        echo "Installing CMake..."
        brew install cmake

        # Install OpenSSL
        echo "Installing OpenSSL..."
        brew install openssl

        # Install jsoncpp
        echo "Installing jsoncpp..."
        brew install jsoncpp

        # Install pkg-config
        echo "Installing pkg-config..."
        brew install pkg-config

        echo ""
        echo "✓ All dependencies installed successfully!"

        # Check OpenSSL version
        echo ""
        echo "Note: On macOS, you may need to set PKG_CONFIG_PATH:"
        echo "  export PKG_CONFIG_PATH=\"/usr/local/opt/openssl/lib/pkgconfig\""
        ;;

    *)
        echo "Error: Unsupported distribution: $OS"
        echo "Please install the following packages manually:"
        echo "  - CMake (3.10 or higher)"
        echo "  - OpenSSL development libraries (1.1.1 or higher)"
        echo "  - jsoncpp development libraries"
        echo "  - pkg-config"
        exit 1
        ;;
esac

echo ""
echo "=========================================="
echo "Verifying Installation"
echo "=========================================="
echo ""

# Verify CMake
if command -v cmake &> /dev/null; then
    CMAKE_VERSION=$(cmake --version | head -n1)
    echo "✓ CMake: $CMAKE_VERSION"
else
    echo "✗ CMake: Not found"
fi

# Verify OpenSSL
if command -v openssl &> /dev/null; then
    OPENSSL_VERSION=$(openssl version)
    echo "✓ OpenSSL: $OPENSSL_VERSION"
else
    echo "✗ OpenSSL: Not found"
fi

# Verify pkg-config
if command -v pkg-config &> /dev/null; then
    echo "✓ pkg-config: Installed"
else
    echo "✗ pkg-config: Not found"
fi

# Verify jsoncpp
if pkg-config --exists jsoncpp 2>/dev/null; then
    JSONCPP_VERSION=$(pkg-config --modversion jsoncpp)
    echo "✓ jsoncpp: $JSONCPP_VERSION"
else
    echo "✗ jsoncpp: Not found or pkg-config cannot locate it"
fi

echo ""
echo "=========================================="
echo "Next Steps"
echo "=========================================="
echo ""
echo "1. Build the project (automatically generates certificates):"
echo "   ./build.sh"
echo ""
echo "2. Run the device simulator:"
echo "   ./run.sh"
echo ""
echo "3. In another terminal, upload firmware:"
echo "   cd build"
echo "   ./firmware_uploader --file firmware.bin --version 2.0.0"
echo ""
echo "4. Or run automated tests:"
echo "   cd build"
echo "   ./test_upload.sh"
echo ""
