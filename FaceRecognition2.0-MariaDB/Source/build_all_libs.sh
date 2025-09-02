#!/bin/bash

# Master Build Script for All Libraries
set -e

echo "=== Master Library Build Script ==="
echo "Usage: $0 [openssl|freetype|sdl2|lvgl|mariadb|zlib|all]"
echo "Note: OpenSSL is installed first when building all libraries"

# Function to build a specific library
build_library() {
    local lib_name=$1
    local script_name="build_${lib_name}.sh"
    
    if [ -f "$script_name" ]; then
        echo "Building $lib_name..."
        chmod +x "$script_name"
        ./"$script_name"
        echo "$lib_name build completed."
        echo ""
    else
        echo "Error: Build script $script_name not found!"
        exit 1
    fi
}

# Check if any arguments provided
if [ $# -eq 0 ]; then
    echo "No arguments provided. Building all libraries..."
    echo "Installing OpenSSL system dependencies first..."
    ./install_openssl.sh
    echo ""
    build_library "freetype"
    build_library "sdl2"
    build_library "lvgl"
    build_library "mariadb"
    build_library "zlib"
    echo "=== All libraries built successfully ==="
    exit 0
fi

# Process arguments
case "$1" in
    "freetype")
        build_library "freetype"
        ;;
    "sdl2")
        build_library "sdl2"
        ;;
    "lvgl")
        build_library "lvgl"
        ;;


    "mariadb")
        build_library "mariadb"
        ;;
    "zlib")
        build_library "zlib"
        ;;
    "openssl")
        echo "Installing OpenSSL system dependencies..."
        ./install_openssl.sh
        ;;
    "all")
        echo "Installing OpenSSL system dependencies first..."
        ./install_openssl.sh
        echo ""
        build_library "freetype"
        build_library "sdl2"
        build_library "lvgl"
        build_library "mariadb"
        build_library "zlib"
        echo "=== All libraries built successfully ==="
        ;;
    *)
        echo "Error: Unknown library '$1'"
        echo "Available options: openssl, freetype, sdl2, lvgl, mariadb, zlib, all"
        echo "Note: OpenSSL is installed first when building all libraries"
        exit 1
        ;;
esac

echo "=== Build completed successfully ==="
