#!/bin/bash

# OpenSSL Build Script
set -e

echo "=== Building OpenSSL ==="

# Configuration
OPENSSL_VERSION="3.0.16"
OPENSSL_SOURCE="openssl-${OPENSSL_VERSION}"
OPENSSL_ARCHIVE="${OPENSSL_SOURCE}.tar.gz"
OPENSSL_URL="https://www.openssl.org/source/${OPENSSL_ARCHIVE}"
LIB_DIR="lib"

# Create lib directory structure
mkdir -p ${LIB_DIR}/include
mkdir -p ${LIB_DIR}/lib

# Download and extract OpenSSL if not exists
if [ ! -d "${OPENSSL_SOURCE}" ]; then
    echo "Downloading OpenSSL ${OPENSSL_VERSION}..."
    if [ ! -f "${OPENSSL_ARCHIVE}" ]; then
        wget "${OPENSSL_URL}" || {
            echo "Error: Failed to download OpenSSL"
            exit 1
        }
    fi
    
    echo "Extracting OpenSSL..."
    tar -xf "${OPENSSL_ARCHIVE}" || {
        echo "Error: Failed to extract OpenSSL"
        exit 1
    }
fi

# Create build directory
BUILD_DIR="openssl_build"
rm -rf "${BUILD_DIR}"
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

# Configure OpenSSL
echo "Configuring OpenSSL..."
../${OPENSSL_SOURCE}/config \
    --prefix="$(pwd)/install" \
    --openssldir="$(pwd)/install" \
    no-shared \
    no-dso \
    no-engine \
    no-hw \
    no-asm

# Build OpenSSL
echo "Building OpenSSL..."
make -j$(nproc)

# Install to build directory
echo "Installing OpenSSL..."
make install_sw

# Copy to lib directory
echo "Copying to lib directory..."
cp -r install/include/* ../${LIB_DIR}/include/

# Check for library files in both lib/ and lib64/ directories
if [ -f "install/lib/libcrypto.a" ] && [ -f "install/lib/libssl.a" ]; then
    echo "Found libraries in install/lib/"
    cp install/lib/libcrypto.a ../${LIB_DIR}/lib/
    cp install/lib/libssl.a ../${LIB_DIR}/lib/
elif [ -f "install/lib64/libcrypto.a" ] && [ -f "install/lib64/libssl.a" ]; then
    echo "Found libraries in install/lib64/"
    cp install/lib64/libcrypto.a ../${LIB_DIR}/lib/
    cp install/lib64/libssl.a ../${LIB_DIR}/lib/
else
    echo "Error: Could not find OpenSSL libraries in install/lib/ or install/lib64/"
    exit 1
fi

# Cleanup build directory only (keep source)
cd ..
rm -rf "${BUILD_DIR}"

# Clean up downloaded archive and source directory
rm -f "${OPENSSL_ARCHIVE}"
rm -rf "${OPENSSL_SOURCE}"

echo "=== OpenSSL build completed successfully ==="
echo "Libraries installed to: ${LIB_DIR}/lib/"
echo "Headers installed to: ${LIB_DIR}/include/"
echo "Source kept in: ${OPENSSL_SOURCE}/"
