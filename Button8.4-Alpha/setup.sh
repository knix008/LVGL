#!/bin/bash
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

# Check for FreeType (for loading fonts at runtime)
if ! pkg-config --exists freetype2; then
    print_error "FreeType development libraries not found"
    MISSING_PACKAGES+=("libfreetype6-dev")
else
    print_success "FreeType found (for runtime font loading)"
fi

# PNG support is now using LODEPNG (pure C, no external dependencies needed)

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

# Checking the Korean fonts in the assets directory.

# Font checking function
check_fonts() {
    local missing=0
    local fonts=(
        "./fonts/NanumGothicCoding-Bold.ttf"
        "./fonts/NanumGothicCoding.ttf"
    )
    for font in "${fonts[@]}"; do
        if [ ! -f "$font" ]; then
            print_error "Font file not found: $font"
            missing=1
        else
            print_success "Font file found: $font"
        fi
    done
    if [ $missing -ne 0 ]; then
        echo "Please ensure all required Korean Nanum fonts are present in the assets directory."
        exit 1
    fi
}

# Call font checking after system requirements
check_fonts

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

# 3. Build LVGL library
echo "Step 4: Building LVGL library..."
echo ""

if [ -f "lvgl/lib/liblvgl.a" ]; then
    print_info "LVGL library already exists, skipping build"
    read -p "Rebuild LVGL library? [y/N]: " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        print_info "Rebuilding LVGL library..."
        rm -rf lvgl/lib lvgl/build
    else
        echo ""
        print_info "Using existing LVGL library"
    fi
fi

if [ ! -f "lvgl/lib/liblvgl.a" ]; then
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
            gcc -Wall -Wextra -O2 -I. -Ilvgl -DLV_CONF_INCLUDE_SIMPLE $(pkg-config --cflags sdl2) $(pkg-config --cflags freetype2) -c "$src" -o "$obj"
        fi
    done
    
    print_info "Creating static library..."
    ar rcs lvgl/lib/liblvgl.a lvgl/build/*.o
    print_success "LVGL library built: lvgl/lib/liblvgl.a"
fi

echo ""

# 4. Check FreeType (required for loading fonts at runtime)
echo "Step 5: Checking FreeType support..."
echo ""

if ! pkg-config --exists freetype2; then
    print_error "FreeType not found (already checked earlier)"
    exit 1
else
    print_success "FreeType found - fonts will be loaded at runtime"
    print_info "No font pre-generation needed!"
fi

echo ""

# 6. Summary
echo "========================================="
echo "✓ Setup Complete!"
echo "========================================="
echo ""
echo "To build the application:"
echo "  Run \$ make"
echo ""
echo "========================================="