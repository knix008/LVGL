#!/bin/bash

# OpenSSL Installation Script
# This script installs OpenSSL development libraries on the system
set -e

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

# Function to install OpenSSL on Ubuntu/Debian
install_openssl_ubuntu() {
    echo "Installing OpenSSL on Ubuntu/Debian..."
    sudo apt update
    sudo apt install -y libssl-dev
    echo "OpenSSL development libraries installed successfully"
}

# Function to install OpenSSL on CentOS/RHEL/Fedora
install_openssl_centos() {
    echo "Installing OpenSSL on CentOS/RHEL/Fedora..."
    if command -v dnf &> /dev/null; then
        sudo dnf install -y openssl-devel
    elif command -v yum &> /dev/null; then
        sudo yum install -y openssl-devel
    else
        echo "Error: Neither dnf nor yum found"
        exit 1
    fi
    echo "OpenSSL development libraries installed successfully"
}

# Function to install OpenSSL on Arch Linux
install_openssl_arch() {
    echo "Installing OpenSSL on Arch Linux..."
    sudo pacman -S --noconfirm openssl
    echo "OpenSSL development libraries installed successfully"
}

# Function to install OpenSSL on Alpine Linux
install_openssl_alpine() {
    echo "Installing OpenSSL on Alpine Linux..."
    sudo apk add --no-cache openssl-dev
    echo "OpenSSL development libraries installed successfully"
}

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

# Verify installation
echo ""
echo "=== Verifying OpenSSL Installation ==="

# Check if OpenSSL is available
if command -v openssl &> /dev/null; then
    OPENSSL_VERSION=$(openssl version)
    echo "✅ OpenSSL found: $OPENSSL_VERSION"
else
    echo "❌ OpenSSL not found in PATH"
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

# Check if libraries are available
if [ -f "/usr/lib/x86_64-linux-gnu/libssl.so" ] || [ -f "/usr/lib/libssl.so" ]; then
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
        exit 1
    fi
else
    echo "⚠️  pkg-config not found (optional)"
fi

echo ""
echo "=== OpenSSL Installation Complete ==="
echo "OpenSSL development libraries are now available for the project."
echo ""
echo "You can now build the project with:"
echo "  cd Source"
echo "  mkdir -p build"
echo "  cd build"
echo "  cmake .."
echo "  make"
