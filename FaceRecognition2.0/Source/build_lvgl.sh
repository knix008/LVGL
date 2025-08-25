#!/bin/bash
# LVGL Build Script
set -e
echo "=== Building LVGL ==="

# Get the absolute path of the Source directory
SOURCE_DIR="$(pwd)"
LVGL_DIR="${SOURCE_DIR}/lvgl"
LIB_DIR="${SOURCE_DIR}/lib"

# Create lib directories
mkdir -p ${LIB_DIR}/include
mkdir -p ${LIB_DIR}/lib

# Check if LVGL source exists
if [ ! -d "${LVGL_DIR}" ]; then
    echo "Error: LVGL source directory ${LVGL_DIR} not found!"
    echo "Please ensure the LVGL source is in the Source directory."
    exit 1
fi

# Create build directory
BUILD_DIR="lvgl_build"
rm -rf "${BUILD_DIR}"
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

# Create CMakeLists.txt for LVGL build
cat > CMakeLists.txt << EOF
cmake_minimum_required(VERSION 3.10)
project(lvgl_lib C)

set(CMAKE_C_STANDARD 99)

# Set LVGL source directory
set(LVGL_SOURCE_DIR "${LVGL_DIR}")

# Include directories
include_directories(
    \${LVGL_SOURCE_DIR}
    \${LVGL_SOURCE_DIR}/src
    \${LVGL_SOURCE_DIR}/src/libs
    \${LVGL_SOURCE_DIR}/src/libs/freetype
    \${LVGL_SOURCE_DIR}/src/drivers
    \${LVGL_SOURCE_DIR}/src/drivers/sdl
    ${LIB_DIR}/include
    ${LIB_DIR}/include/freetype2
)

# Get LVGL source files only (not application files)
file(GLOB_RECURSE LVGL_SOURCES
    \${LVGL_SOURCE_DIR}/src/*.c
    \${LVGL_SOURCE_DIR}/src/*.cpp
)

# Exclude problematic ThorVG files
list(FILTER LVGL_SOURCES EXCLUDE REGEX ".*tvgGl.*\\.cpp$")
list(FILTER LVGL_SOURCES EXCLUDE REGEX ".*tvgWg.*\\.cpp$")
list(FILTER LVGL_SOURCES EXCLUDE REGEX ".*thorvg.*\\.cpp$")
list(FILTER LVGL_SOURCES EXCLUDE REGEX ".*thorvg.*\\.h$")

# Get SDL driver sources
file(GLOB LV_DRIVERS_SOURCES
    \${LVGL_SOURCE_DIR}/src/drivers/sdl/*.c
)

# Create LVGL static library
add_library(lvgl_lib STATIC \${LVGL_SOURCES} \${LV_DRIVERS_SOURCES})

# Set output directory
set_target_properties(lvgl_lib PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "\${CMAKE_BINARY_DIR}"
)
EOF

echo "Configuring LVGL..."
cmake .

echo "Building LVGL..."
make -j$(nproc)

echo "Copying to lib directory..."
cp liblvgl_lib.a ${LIB_DIR}/lib/

echo "Copying headers..."
cp -r ${LVGL_DIR}/src ${LIB_DIR}/include/lvgl_src
cp -r ${LVGL_DIR}/src/drivers ${LIB_DIR}/include/lvgl_drivers

cd ..
rm -rf "${BUILD_DIR}"

echo "=== LVGL build completed successfully ==="
echo "Library installed to: ${LIB_DIR}/lib/liblvgl_lib.a"
echo "Headers installed to: ${LIB_DIR}/include/"
