#!/bin/bash

# OpenSSL Installation Script
# This script installs OpenSSL development libraries on the system
# Removed set -e to handle already installed packages gracefully

echo "=== Installing OpenSSL Development Libraries ==="

# Detect the operating system
if [ -f /etc/os-release ]; then
    . /etc/os-release
    OS=$NAME
    VER=$VERSION_ID
else
    echo "Error: Cannot detect operating system"
    exit 1
fi

echo "Detected OS: $OS $VER"

# Function to check if OpenSSL is already installed
check_openssl_installed() {
    echo "Checking if OpenSSL is already installed..."
    
    local already_installed=false
    
    # Check if OpenSSL binary is available
    if command -v openssl &> /dev/null; then
        echo "✅ OpenSSL binary found"
        already_installed=true
    fi
    
    # Check if development headers are available
    if [ -f "/usr/include/openssl/ssl.h" ]; then
        echo "✅ OpenSSL development headers found"
        already_installed=true
    fi
    
    # Check if libraries are available
    if [ -f "/usr/lib/x86_64-linux-gnu/libssl.so" ] || [ -f "/usr/lib/libssl.so" ] || [ -f "/usr/lib/aarch64-linux-gnu/libssl.so" ]; then
        echo "✅ OpenSSL libraries found"
        already_installed=true
    fi
    
    # Check pkg-config
    if command -v pkg-config &> /dev/null && pkg-config --exists openssl; then
        echo "✅ pkg-config OpenSSL found"
        already_installed=true
    fi
    
    if [ "$already_installed" = true ]; then
        echo "✅ OpenSSL appears to be already installed and properly configured"
        return 0
    else
        echo "❌ OpenSSL not found or incomplete installation"
        return 1
    fi
}

# Function to install OpenSSL on Ubuntu/Debian
install_openssl_ubuntu() {
    echo "Installing OpenSSL on Ubuntu/Debian..."
    sudo apt update
    
    # Try to install, but don't fail if already installed
    if sudo apt install -y libssl-dev; then
        echo "OpenSSL development libraries installed successfully"
    else
        echo "OpenSSL installation completed (may have been already installed)"
    fi
}

# Function to install OpenSSL on CentOS/RHEL/Fedora
install_openssl_centos() {
    echo "Installing OpenSSL on CentOS/RHEL/Fedora..."
    if command -v dnf &> /dev/null; then
        if sudo dnf install -y openssl-devel; then
            echo "OpenSSL development libraries installed successfully"
        else
            echo "OpenSSL installation completed (may have been already installed)"
        fi
    elif command -v yum &> /dev/null; then
        if sudo yum install -y openssl-devel; then
            echo "OpenSSL development libraries installed successfully"
        else
            echo "OpenSSL installation completed (may have been already installed)"
        fi
    else
        echo "Error: Neither dnf nor yum found"
        exit 1
    fi
}

# Function to install OpenSSL on Arch Linux
install_openssl_arch() {
    echo "Installing OpenSSL on Arch Linux..."
    if sudo pacman -S --noconfirm openssl; then
        echo "OpenSSL development libraries installed successfully"
    else
        echo "OpenSSL installation completed (may have been already installed)"
    fi
}

# Function to install OpenSSL on Alpine Linux
install_openssl_alpine() {
    echo "Installing OpenSSL on Alpine Linux..."
    if sudo apk add --no-cache openssl-dev; then
        echo "OpenSSL development libraries installed successfully"
    else
        echo "OpenSSL installation completed (may have been already installed)"
    fi
}

# Check if OpenSSL is already installed first
if check_openssl_installed; then
    echo ""
    echo "✅ OpenSSL is already properly installed and configured!"
    echo "No installation needed."
else
    echo ""
    echo "Installing OpenSSL development libraries..."
    
    # Install based on detected OS
    case $OS in
        "Ubuntu"|"Debian GNU/Linux"|"Linux Mint")
            install_openssl_ubuntu
            ;;
        "CentOS Linux"|"Red Hat Enterprise Linux"|"Fedora")
            install_openssl_centos
            ;;
        "Arch Linux")
            install_openssl_arch
            ;;
        "Alpine Linux")
            install_openssl_alpine
            ;;
        *)
            echo "Unsupported operating system: $OS"
            echo "Please install OpenSSL development libraries manually:"
            echo "  Ubuntu/Debian: sudo apt install libssl-dev"
            echo "  CentOS/RHEL: sudo yum install openssl-devel"
            echo "  Fedora: sudo dnf install openssl-devel"
            echo "  Arch: sudo pacman -S openssl"
            echo "  Alpine: sudo apk add openssl-dev"
            exit 1
            ;;
    esac
fi

# Verify installation
echo ""
echo "=== Verifying OpenSSL Installation ==="

# Check if OpenSSL is available
if command -v openssl &> /dev/null; then
    OPENSSL_VERSION=$(openssl version)
    echo "✅ OpenSSL found: $OPENSSL_VERSION"
else
    echo "❌ OpenSSL not found in PATH"
    echo "Please ensure OpenSSL is properly installed"
    exit 1
fi

# Check if development headers are available
if [ -f "/usr/include/openssl/ssl.h" ]; then
    echo "✅ OpenSSL development headers found"
else
    echo "❌ OpenSSL development headers not found"
    echo "Please ensure libssl-dev package is installed"
    exit 1
fi

# Check if libraries are available (multiple possible locations)
if [ -f "/usr/lib/x86_64-linux-gnu/libssl.so" ] || [ -f "/usr/lib/libssl.so" ] || [ -f "/usr/lib/aarch64-linux-gnu/libssl.so" ] || [ -f "/usr/local/lib/libssl.so" ]; then
    echo "✅ OpenSSL libraries found"
else
    echo "❌ OpenSSL libraries not found"
    echo "Please ensure libssl-dev package is installed"
    exit 1
fi

# Test pkg-config
if command -v pkg-config &> /dev/null; then
    if pkg-config --exists openssl; then
        OPENSSL_PC_VERSION=$(pkg-config --modversion openssl)
        echo "✅ pkg-config OpenSSL version: $OPENSSL_PC_VERSION"
    else
        echo "❌ pkg-config cannot find OpenSSL"
        echo "This may indicate a configuration issue"
        exit 1
    fi
else
    echo "⚠️  pkg-config not found (optional but recommended)"
fi

echo ""
echo "=== OpenSSL Installation Summary ==="
echo "✅ OpenSSL installation and verification completed successfully!"
echo ""
echo "OpenSSL development libraries are now available for the project."
echo ""
echo "Next steps:"
echo "  1. Build the project libraries:"
echo "     cd Source && ./build_all_libs.sh all"
echo ""
echo "  2. Or build the main application:"
echo "     cd Source"
echo "     mkdir -p build"
echo "     cd build"
echo "     cmake .."
echo "     make"
echo ""
echo "  3. Or use the run script:"
echo "     cd .. && ./run.sh libs"
