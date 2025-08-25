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

# Check if FreeType source exists
if [ ! -d "${FREETYPE_SOURCE}" ]; then
    echo "Error: FreeType source directory ${FREETYPE_SOURCE} not found!"
    echo "Please ensure the FreeType source is in the Source directory."
    exit 1
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

echo "=== FreeType build completed successfully ==="
echo "Libraries installed to: ${LIB_DIR}/lib/libfreetype.a"
echo "Headers installed to: ${LIB_DIR}/include/freetype2/"
