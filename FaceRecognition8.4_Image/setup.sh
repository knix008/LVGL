#!/bin/bash
# Webcam Capture Application - Automated Setup Script
# Sets up LVGL environment and builds the application

set -e  # Exit on error

echo "========================================="
echo " LVGL Setup"
echo "========================================="
echo ""

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored messages
print_success() {
    echo -e "${GREEN}✓${NC} $1"
}

print_error() {
    echo -e "${RED}✗${NC} $1"
}

print_info() {
    echo -e "${YELLOW}→${NC} $1"
}

# Check if running on Linux
if [[ "$OSTYPE" != "linux-gnu"* ]]; then
    print_error "This script is designed for Linux systems"
    exit 1
fi

# 1. Check for required system packages
echo "Step 1: Checking system requirements..."
echo ""

MISSING_PACKAGES=()

# Check for SDL2
if ! pkg-config --exists sdl2; then
    print_error "SDL2 development libraries not found"
    MISSING_PACKAGES+=("libsdl2-dev")
else
    print_success "SDL2 found"
fi

# Check for build tools
if ! command -v gcc &> /dev/null; then
    print_error "GCC not found"
    MISSING_PACKAGES+=("build-essential")
else
    print_success "GCC found"
fi

if ! command -v git &> /dev/null; then
    print_error "Git not found"
    MISSING_PACKAGES+=("git")
else
    print_success "Git found"
fi

# Check for FreeType
if ! pkg-config --exists freetype2; then
    print_error "FreeType development libraries not found"
    MISSING_PACKAGES+=("libfreetype6-dev")
else
    print_success "FreeType found"
fi

# Check for libjpeg
if [ -f "/usr/include/jpeglib.h" ] || [ -f "/usr/local/include/jpeglib.h" ]; then
    print_success "libjpeg found"
else
    print_error "libjpeg development library not found"
    MISSING_PACKAGES+=("libjpeg-dev")
fi

# Check for FFmpeg libraries
if pkg-config --exists libavformat libavcodec libavdevice libswscale libavutil; then
    print_success "FFmpeg development libraries found"
else
    print_error "FFmpeg development libraries not found"
    MISSING_PACKAGES+=("libavformat-dev" "libavcodec-dev" "libavdevice-dev" "libswscale-dev" "libavutil-dev")
fi

# Check for SDL2_mixer
if pkg-config --exists SDL2_mixer; then
    print_success "SDL2_mixer found"
else
    print_error "SDL2_mixer development library not found"
    MISSING_PACKAGES+=("libsdl2-mixer-dev")
fi

# Install missing packages if any
if [ ${#MISSING_PACKAGES[@]} -gt 0 ]; then
    echo ""
    echo "Missing packages: ${MISSING_PACKAGES[*]}"
    echo ""
    read -p "Install missing packages? (requires sudo) [Y/n]: " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]] || [[ -z $REPLY ]]; then
        print_info "Installing packages..."
        sudo apt-get update
        sudo apt-get install -y "${MISSING_PACKAGES[@]}"
        print_success "Packages installed"
    else
        print_error "Cannot continue without required packages"
        exit 1
    fi
fi

echo ""

# 2. Clone LVGL
echo "Step 2: Setting up LVGL..."
echo ""

if [ -d "lvgl" ]; then
    print_info "LVGL directory already exists, skipping clone"
else
    print_info "Cloning LVGL v8.4..."
    git clone --depth 1 --branch release/v8.4 https://github.com/lvgl/lvgl.git
    print_success "LVGL cloned"
fi

echo ""

# 3. Check Korean fonts
echo "Step 3: Checking Korean fonts..."
echo ""

mkdir -p assets

FONTS_FOUND=0
if [ -f "assets/NanumGothicCoding.ttf" ]; then
    print_success "NanumGothicCoding.ttf found"
    FONTS_FOUND=1
fi

if [ -f "assets/NanumGothicCoding-Bold.ttf" ]; then
    print_success "NanumGothicCoding-Bold.ttf found"
    FONTS_FOUND=1
fi

if [ $FONTS_FOUND -eq 0 ]; then
    print_info "Downloading Nanum Gothic font..."
    mkdir -p assets
    if ! wget -q https://github.com/google/fonts/raw/main/ofl/nanumgothic/NanumGothic-Regular.ttf -O assets/NanumGothic.ttf; then
        print_error "Failed to download font"
        exit 1
    fi
    print_success "Font downloaded"
fi

echo ""

# 4. Check webcam access
echo "Step 4: Checking webcam access..."
echo ""

if [ -c "/dev/video0" ]; then
    print_success "Webcam device found at /dev/video0"
else
    print_info "No webcam found at /dev/video0 (you may need to plug in a camera)"
fi

echo ""

# 5. Build LVGL library
echo "Step 5: Building LVGL library..."
echo ""

# Check if LVGL needs to be rebuilt (force rebuild if requested)
if [ -f "lvgl/lib/liblvgl.a" ] && [ "$1" != "--rebuild" ]; then
    print_info "LVGL library already exists, skipping build"
    print_info "Use './setup.sh --rebuild' to force rebuild LVGL with FreeType"
else
    print_info "Building LVGL static library with FreeType support..."

    # Create build and lib directories for LVGL
    mkdir -p lvgl/build
    mkdir -p lvgl/lib
    # Find all LVGL source files
    print_info "Compiling LVGL sources..."
    cd lvgl

    # Count total source files
    TOTAL_FILES=$(find src -name "*.c" | wc -l)
    COMPILED_COUNT=0

    print_info "Found $TOTAL_FILES source files to compile"
    echo ""

    # Compile all LVGL source files to object files
    find src -name "*.c" | while read src_file; do
        COMPILED_COUNT=$((COMPILED_COUNT + 1))
        obj_file="build/$(echo $src_file | sed 's/\.c$/\.o/' | sed 's/\//_/g')"

        # Show progress
        printf "\r${YELLOW}→${NC} Compiling: [$COMPILED_COUNT/$TOTAL_FILES] $src_file"

        gcc -c "$src_file" -I. -I.. -O2 -DLV_CONF_INCLUDE_SIMPLE \
            $(pkg-config --cflags freetype2) \
            -o "$obj_file" 2>/dev/null

        if [ $? -ne 0 ]; then
            echo ""
            print_error "Failed to compile $src_file"
            cd ..
            exit 1
        fi
    done

    echo ""
    echo ""

    # Create static library from all object files
    OBJECT_COUNT=$(ls -1 build/*.o 2>/dev/null | wc -l)
    print_info "Creating static library from $OBJECT_COUNT object files..."
    ar rcs lib/liblvgl.a build/*.o 2>/dev/null

    if [ $? -eq 0 ]; then
        print_success "LVGL library created successfully"
    else
        print_error "Failed to create LVGL static library"
        cd ..
        exit 1
    fi

    # Show library size
    LIBRARY_SIZE=$(du -h lib/liblvgl.a | cut -f1)
    print_info "Library size: $LIBRARY_SIZE"

    cd ..

    if [ -f "lvgl/lib/liblvgl.a" ]; then
        print_success "LVGL compilation completed successfully"
    else
        print_error "LVGL compilation failed"
        exit 1
    fi
fi

echo ""

echo ""
echo "========================================="
echo " Setup Complete!"
echo "========================================="
echo ""
echo "To run the application:"
echo "  ./face_recognition"
echo ""
echo "Make sure your webcam is connected to /dev/video0"
echo "Photos will be saved as JPEG files in the current directory"
echo ""
echo "To rebuild LVGL with FreeType support, run:"
echo "  ./setup.sh --rebuild"
echo ""