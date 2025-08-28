#!/bin/bash

# MariaDB Client Library Build Script
set -e

echo "=== Building MariaDB Client Library ==="

# Configuration
MARIADB_VERSION="3.3.9"
MARIADB_SOURCE="mariadb-connector-c-${MARIADB_VERSION}"
MARIADB_ARCHIVE="${MARIADB_SOURCE}-src.tar.gz"
MARIADB_URL="https://github.com/MariaDB/mariadb-connector-c/archive/refs/tags/v${MARIADB_VERSION}.tar.gz"
LIB_DIR="lib"

# Create lib directory structure
mkdir -p ${LIB_DIR}/include
mkdir -p ${LIB_DIR}/lib

# Download and extract MariaDB if not exists
if [ ! -d "${MARIADB_SOURCE}" ]; then
    echo "Downloading MariaDB Connector/C ${MARIADB_VERSION}..."
    if [ ! -f "${MARIADB_ARCHIVE}" ]; then
        wget "${MARIADB_URL}" || {
            echo "Error: Failed to download MariaDB Connector/C"
            exit 1
        }
    fi
    
    echo "Extracting MariaDB Connector/C..."
    tar -xf "v${MARIADB_VERSION}.tar.gz" || {
        echo "Error: Failed to extract MariaDB Connector/C"
        exit 1
    }
fi

echo "Building MariaDB Connector/C..."

# Change to MariaDB directory
cd ${MARIADB_SOURCE}

# Create build directory
BUILD_DIR="mariadb_build"
mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

# Configure MariaDB Connector/C
echo "Configuring MariaDB Connector/C..."
cmake .. \
    -DCMAKE_INSTALL_PREFIX="$(pwd)/install" \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_SHARED_LIBS=OFF \
    -DBUILD_STATIC_LIBS=ON \
    -DWITH_SSL=OPENSSL \
    -DWITH_EXTERNAL_ZLIB=ON \
    -DWITH_UNIT_TESTS=OFF \
    -DWITH_DYNAMIC_PLUGINS=OFF \
    -DWITH_OPENSSL=ON \
    -DOPENSSL_ROOT_DIR="$(pwd)/../../lib" \
    -DOPENSSL_INCLUDE_DIR="$(pwd)/../../lib/include" \
    -DOPENSSL_LIBRARIES="$(pwd)/../../lib/lib" \
    -DCMAKE_C_FLAGS="-I$(pwd)/../../lib/include" \
    -DCMAKE_EXE_LINKER_FLAGS="-L$(pwd)/../../lib/lib" \
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5

# Build MariaDB Connector/C
echo "Building MariaDB Connector/C..."
make -j$(nproc)

# Install MariaDB Connector/C
echo "Installing MariaDB Connector/C..."
make install

# Copy to lib directory
echo "Copying to lib directory..."
cp install/lib/mariadb/libmariadbclient.a ../../${LIB_DIR}/lib/libmariadb.a
cp -r install/include/* ../../${LIB_DIR}/include/

cd ../..

# Clean up downloaded archive and source directory
rm -f "v${MARIADB_VERSION}.tar.gz"
rm -rf "${MARIADB_SOURCE}"

echo "=== MariaDB Connector/C build completed successfully ==="
echo "Files installed to: ${LIB_DIR}/"
echo "Library: ${LIB_DIR}/lib/libmariadb.a"
echo "Headers: ${LIB_DIR}/include/"
