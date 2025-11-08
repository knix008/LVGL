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
    print_info "Cloning LVGL v9.2..."
    git clone --depth 1 --branch release/v9.2 https://github.com/lvgl/lvgl.git
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

# 4. Build LVGL library
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

    if [ $? -eq 0 ]; then
        print_success "LVGL library built successfully"
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
echo "To rebuild LVGL with FreeType support, run:"
echo "  ./setup.sh --rebuild"
echo ""