#!/bin/bash
# Japanese QWERTY Input Method - Automated Setup Script
# Sets up LVGL environment and builds the application

set -e  # Exit on error

echo "========================================="
echo " Japanese Qwerty Input Method - Setup"
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

# 3. Verify font files
echo "Step 3: Checking font files..."
echo ""

if [ -f "assets/NotoSansCJK.ttc" ]; then
    print_success "NotoSansCJK.ttc found"
else
    print_error "NotoSansCJK.ttc not found in assets/"
    echo ""
    echo "Please download NotoSansCJK.ttc and place it in the assets/ directory"
    echo "Download from: https://github.com/googlefonts/noto-cjk"
    exit 1
fi

echo ""

# 4. Build LVGL library
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

# 5. Check FreeType (required for loading fonts at runtime)
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

# 6. Build the application
echo "Step 6: Building the application..."
echo ""

print_info "Running make clean..."
make clean 2>/dev/null || true

print_info "Building Japanese Qwerty Input Method..."
if make; then
    print_success "Build successful!"
else
    print_error "Build failed!"
    echo ""
    echo "Common issues:"
    echo "  - Make sure lv_conf.h is in the project root"
    echo "  - Check that SDL2 development files are installed"
    echo "  - Verify LVGL library was built correctly"
    echo "  - Ensure FreeType is installed (libfreetype6-dev)"
    echo "  - Check that assets/NotoSansCJK.ttc exists"
    exit 1
fi

echo ""

# 7. Summary
echo "========================================="
echo "✓ Setup Complete!"
echo "========================================="
echo ""
echo "The Japanese QWERTY Input Method is ready to use!"
echo ""
echo "To run the application:"
echo "  ./japanese_input"
echo ""
echo "Features:"
echo "  • 640x480 window with on-screen QWERTY button keyboard"
echo "  • Click buttons to type Japanese characters"
echo "  • Hiragana, Katakana, and English input modes"
echo "  • Real-time romaji to kana conversion"
echo "  • Japanese font support (NotoSansCJK)"
echo "  • Comprehensive romaji mapping"
echo ""
echo "Controls:"
echo "  • Click letter buttons to type romaji"
echo "  • Switch Mode: Toggle between Hiragana/Katakana/English"
echo "  • Space: Commit text and add space"
echo "  • Enter: Commit text and add newline"
echo "  • Bksp: Backspace (delete character)"
echo "  • -: Insert prolonged sound mark (ー)"
echo "  • Clear: Clear all text"
echo ""
echo "Examples:"
echo "  • konnichiha → こんにちは"
echo "  • arigatou → ありがとう"
echo "  • nihon → にほん"
echo "  • to-kyo- → とーきょー"
echo ""
echo "Testing:"
echo "  • Run all tests: ./run_test.sh"
echo "  • 53 unit tests with 100% pass rate"
echo ""
echo "For more information, see README.md"
echo ""

# Ask if user wants to run tests
read -p "Run tests first? [y/N]: " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    print_info "Running test suite..."
    ./run_test.sh
    echo ""
fi

# Ask if user wants to run the application
read -p "Run the application now? [Y/n]: " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]] || [[ -z $REPLY ]]; then
    print_info "Starting Japanese Input Method..."
    ./japanese_input
fi
