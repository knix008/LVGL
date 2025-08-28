#!/bin/bash

# Master Build Script for All Libraries
set -e

echo "=== Master Library Build Script ==="
echo "Usage: $0 [freetype|sdl2|lvgl|openssl|mariadb|all]"

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
    build_library "freetype"
    build_library "sdl2"
    build_library "lvgl"
    build_library "openssl"
    build_library "mariadb"
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

    "openssl")
        build_library "openssl"
        ;;
    "mariadb")
        build_library "mariadb"
        ;;
    "all")
        build_library "freetype"
        build_library "sdl2"
        build_library "lvgl"
        build_library "openssl"
        build_library "mariadb"
        echo "=== All libraries built successfully ==="
        ;;
    *)
        echo "Error: Unknown library '$1'"
        echo "Available options: freetype, sdl2, lvgl, openssl, mariadb, all"
        exit 1
        ;;
esac

echo "=== Build completed successfully ==="
