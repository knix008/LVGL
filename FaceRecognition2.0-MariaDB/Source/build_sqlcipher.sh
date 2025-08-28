#!/bin/bash

# SQLCipher Build Script
set -e

echo "=== Building SQLCipher ==="

# Configuration
SQLCIPHER_VERSION="4.5.4"
SQLCIPHER_SOURCE="sqlcipher-${SQLCIPHER_VERSION}"
SQLCIPHER_ARCHIVE="${SQLCIPHER_SOURCE}.tar.gz"
SQLCIPHER_URL="https://github.com/sqlcipher/sqlcipher/archive/refs/tags/v${SQLCIPHER_VERSION}.tar.gz"
LIB_DIR="lib"

# Create lib directory structure
mkdir -p ${LIB_DIR}/include
mkdir -p ${LIB_DIR}/lib

# Download and extract SQLCipher if not exists
if [ ! -d "${SQLCIPHER_SOURCE}" ]; then
    echo "Downloading SQLCipher ${SQLCIPHER_VERSION}..."
    if [ ! -f "${SQLCIPHER_ARCHIVE}" ]; then
        wget "${SQLCIPHER_URL}" || {
            echo "Error: Failed to download SQLCipher"
            exit 1
        }
    fi
    
    echo "Extracting SQLCipher..."
    tar -xf "v${SQLCIPHER_VERSION}.tar.gz" || {
        echo "Error: Failed to extract SQLCipher"
        exit 1
    }
fi

echo "Building SQLCipher..."

# Change to SQLCipher directory
cd ${SQLCIPHER_SOURCE}

# Create build directory
BUILD_DIR="sqlcipher_build"
mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

# Configure SQLCipher
echo "Configuring SQLCipher..."
../configure \
    --prefix="$(pwd)/install" \
    --enable-static \
    --disable-shared \
    --disable-tcl \
    --disable-readline \
    --disable-load-extension \
    --with-crypto-lib=openssl \
    --enable-tempstore=yes \
    CFLAGS="-DSQLITE_HAS_CODEC=1 -DSQLCIPHER_CRYPTO_OPENSSL=1" \
    LDFLAGS="-L$(pwd)/../../lib/lib" \
    CPPFLAGS="-I$(pwd)/../../lib/include"

# Build SQLCipher
echo "Building SQLCipher..."
make -j$(nproc)

# Install SQLCipher
echo "Installing SQLCipher..."
make install

# Copy to lib directory
echo "Copying to lib directory..."
cp install/lib/libsqlcipher.a ../../${LIB_DIR}/lib/
cp -r install/include/* ../../${LIB_DIR}/include/

cd ../..

# Clean up downloaded archive and source directory
rm -f "v${SQLCIPHER_VERSION}.tar.gz"
rm -rf "${SQLCIPHER_SOURCE}"

echo "=== SQLCipher build completed successfully ==="
echo "Files installed to: ${LIB_DIR}/"
echo "Library: ${LIB_DIR}/lib/libsqlcipher.a"
echo "Headers: ${LIB_DIR}/include/" 