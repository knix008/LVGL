#!/bin/bash
# Chunjiin Korean Input Method - Automated Setup Script
# Sets up LVGL environment and builds the application

set -e  # Exit on error

echo "========================================="
echo " Chunjiin Korean Input Method - Setup"
echo " 천지인 한글 입력기"
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

if [ -f "assets/NanumGothic-Regular.ttf" ]; then
    print_success "NanumGothic-Regular.ttf found"
else
    print_error "Font file not found: assets/NanumGothic-Regular.ttf"
    echo ""
    echo "Please ensure the font files are in the assets/ directory"
    exit 1
fi

echo ""

# 4. Build the application
echo "Step 4: Building the application..."
echo ""

print_info "The application uses FreeType to load TrueType fonts at runtime"
print_info "No font conversion needed!"
echo ""

print_info "Running make clean..."
make clean 2>/dev/null || true

print_info "Building Chunjiin..."
if make; then
    print_success "Build successful!"
else
    print_error "Build failed!"
    echo ""
    echo "Common issues:"
    echo "  - Make sure lv_conf.h is in the project root"
    echo "  - Check that SDL2 development files are installed"
    echo "  - Verify LVGL and lv_drivers were cloned correctly"
    exit 1
fi

echo ""

# 5. Summary
echo "========================================="
echo "✓ Setup Complete!"
echo "========================================="
echo ""
echo "The Chunjiin Korean Input Method is ready to use!"
echo ""
echo "To run the application:"
echo "  ./chunjiin"
echo ""
echo "Or use:"
echo "  make run"
echo ""
echo "Features:"
echo "  • Korean input using Chunjiin (천지인) method"
echo "  • Real-time character composition with incomplete character display"
echo "  • Multiple input modes: 한글, 영문, 숫자, 특수문자"
echo "  • Beautiful Korean fonts via FreeType + NanumGothic"
echo "  • LVGL-based GUI with SDL2"
echo ""
echo "For more information, see README.md"
echo ""

# Ask if user wants to run the application
read -p "Run the application now? [Y/n]: " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]] || [[ -z $REPLY ]]; then
    print_info "Starting Chunjiin..."
    ./chunjiin
fi
