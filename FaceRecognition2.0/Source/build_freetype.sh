#!/bin/bash

# FreeType Build Script
set -e

echo "=== Building FreeType ==="

# Configuration
FREETYPE_VERSION="2.13.2"
FREETYPE_SOURCE="freetype-${FREETYPE_VERSION}"
FREETYPE_ARCHIVE="${FREETYPE_SOURCE}.tar.xz"
FREETYPE_URL="https://download.savannah.gnu.org/releases/freetype/${FREETYPE_ARCHIVE}"
LIB_DIR="lib"

# Create lib directory structure
mkdir -p ${LIB_DIR}/include
mkdir -p ${LIB_DIR}/lib

# Download and extract FreeType if not exists
if [ ! -d "${FREETYPE_SOURCE}" ] || [ ! -f "${FREETYPE_SOURCE}/configure" ]; then
    echo "Downloading FreeType ${FREETYPE_VERSION}..."
    if [ ! -f "${FREETYPE_ARCHIVE}" ]; then
        wget "${FREETYPE_URL}" || {
            echo "Error: Failed to download FreeType"
            exit 1
        }
    fi
    
    echo "Extracting FreeType..."
    rm -rf "${FREETYPE_SOURCE}"  # Remove incomplete directory if exists
    tar -xf "${FREETYPE_ARCHIVE}" || {
        echo "Error: Failed to extract FreeType"
        exit 1
    }
fi

# Create build directory
BUILD_DIR="freetype_build"
rm -rf "${BUILD_DIR}"
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

# Configure FreeType
echo "Configuring FreeType..."
../${FREETYPE_SOURCE}/configure \
    --prefix="$(pwd)/install" \
    --enable-static \
    --disable-shared \
    --with-harfbuzz=no \
    --with-brotli=no \
    --with-bzip2=no \
    --with-png=no \
    --with-zlib=no

# Build FreeType
echo "Building FreeType..."
make -j$(nproc)

# Install to build directory
echo "Installing FreeType..."
make install

# Copy to lib directory
echo "Copying to lib directory..."
cp -r install/include/* ../${LIB_DIR}/include/
cp install/lib/libfreetype.a ../${LIB_DIR}/lib/

# Cleanup
cd ..
rm -rf "${BUILD_DIR}"

# Clean up downloaded archive and source directory
rm -f "${FREETYPE_ARCHIVE}"
rm -rf "${FREETYPE_SOURCE}"

echo "=== FreeType build completed successfully ==="
echo "Libraries installed to: ${LIB_DIR}/lib/libfreetype.a"
echo "Headers installed to: ${LIB_DIR}/include/freetype2/"
