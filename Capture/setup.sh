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

# Check for Python3
if ! command -v python3 &> /dev/null; then
    print_error "Python3 not found"
    MISSING_PACKAGES+=("python3")
else
    print_success "Python3 found"
fi

# Check for Node.js and npm (for lv_font_conv)
if ! command -v node &> /dev/null; then
    print_error "Node.js not found"
    MISSING_PACKAGES+=("nodejs")
else
    print_success "Node.js found"
fi

if ! command -v npm &> /dev/null; then
    print_error "npm not found"
    MISSING_PACKAGES+=("npm")
else
    print_success "npm found"
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
    print_info "Cloning LVGL v9.2..."
    git clone --depth 1 --branch release/v9.2 https://github.com/lvgl/lvgl.git
    print_success "LVGL cloned"
fi

echo ""

# 3. Check and install lv_font_conv
echo "Step 3: Checking font converter..."
echo ""

if command -v lv_font_conv &> /dev/null; then
    print_success "lv_font_conv found"
else
    print_info "lv_font_conv not found, installing..."
    sudo npm install -g lv_font_conv
    if command -v lv_font_conv &> /dev/null; then
        print_success "lv_font_conv installed"
    else
        print_error "Failed to install lv_font_conv"
        exit 1
    fi
fi

echo ""

# 4. Download Korean font
echo "Step 4: Checking Korean font..."
echo ""

mkdir -p assets

if [ -f "assets/NanumGothic.ttf" ]; then
    print_success "Nanum Gothic font found"
else
    print_info "Downloading Nanum Gothic font..."
    wget -q https://github.com/google/fonts/raw/main/ofl/nanumgothic/NanumGothic-Regular.ttf -O assets/NanumGothic.ttf
    if [ -f "assets/NanumGothic.ttf" ]; then
        print_success "Font downloaded"
    else
        print_error "Failed to download font"
        exit 1
    fi
fi

echo ""

# 5. Generate Korean fonts
echo "Step 5: Generating Korean fonts for LVGL..."
echo ""

if [ -d "assets/fonts" ] && [ "$(ls -A assets/fonts/*.c 2>/dev/null)" ]; then
    print_info "Fonts already generated, skipping"
else
    print_info "Generating fonts (this may take a minute)..."
    ./generate_fonts.py
    if [ $? -eq 0 ]; then
        print_success "Fonts generated"
    else
        print_error "Font generation failed"
        exit 1
    fi
fi

echo ""

# 6. Check webcam access
echo "Step 6: Checking webcam access..."
echo ""

if [ -c "/dev/video0" ]; then
    print_success "Webcam device found at /dev/video0"
else
    print_info "No webcam found at /dev/video0 (you may need to plug in a camera)"
fi

echo ""

# 7. Build LVGL library
echo "Step 7: Building LVGL library..."
echo ""

# Check if LVGL needs to be rebuilt (force rebuild if requested)
if [ -f "lvgl/lib/liblvgl.a" ] && [ "$1" != "--rebuild" ]; then
    print_info "LVGL library already exists, skipping build"
    print_info "Use './setup.sh --rebuild' to force rebuild LVGL with FreeType"
else
    print_info "Building LVGL static library..."
    
    # Create build and lib directories for LVGL
    mkdir -p lvgl/build
    mkdir -p lvgl/lib
    
    # Find all LVGL source files
    LVGL_SOURCES=$(find lvgl/src -name "*.c")
    
    print_info "Compiling LVGL sources..."
    for src in $LVGL_SOURCES; do
        obj="lvgl/build/$(basename ${src%.c}.o)"
        if [ ! -f "$obj" ] || [ "$src" -nt "$obj" ]; then
            echo "  Compiling $src"
            gcc -Wall -Wextra -O2 -I. -Ilvgl $(pkg-config --cflags freetype2) -c "$src" -o "$obj"
        fi
    done
    
    print_info "Creating static library..."
    ar rcs lvgl/lib/liblvgl.a lvgl/build/*.o
    print_success "LVGL library built: lvgl/lib/liblvgl.a"
fi

echo ""

# 8. Build the webcam capture application
echo "Step 8: Building webcam capture application..."
echo ""

if [ -f "Makefile" ]; then
    print_info "Building application..."
    make clean
    make
    print_success "Application built successfully: webcam_capture"
else
    print_error "Makefile not found"
    exit 1
fi

echo ""
echo "========================================="
echo " Setup Complete!"
echo "========================================="
echo ""
echo "To run the application:"
echo "  ./webcam_capture"
echo ""
echo "Make sure your webcam is connected to /dev/video0"
echo "Photos will be saved as JPEG files in the current directory"
echo ""