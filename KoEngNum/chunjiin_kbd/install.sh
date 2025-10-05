#!/bin/bash

# Chunjiin Keyboard Installation Script

set -e

echo "=== Chunjiin Korean Keyboard Installation ==="
echo ""

# Detect OS
if [ -f /etc/os-release ]; then
    . /etc/os-release
    OS=$ID
else
    echo "Cannot detect OS. Please install dependencies manually."
    exit 1
fi

echo "Detected OS: $OS"
echo ""

# Check dependencies
echo "Step 1: Checking system dependencies..."
MISSING_DEPS=()

command -v cmake >/dev/null 2>&1 || MISSING_DEPS+=("cmake")
command -v git >/dev/null 2>&1 || MISSING_DEPS+=("git")
command -v gcc >/dev/null 2>&1 || MISSING_DEPS+=("gcc")
pkg-config --exists sdl2 2>/dev/null || MISSING_DEPS+=("libsdl2-dev")
pkg-config --exists freetype2 2>/dev/null || MISSING_DEPS+=("libfreetype6-dev")

if [ ${#MISSING_DEPS[@]} -eq 0 ]; then
    echo "✓ All dependencies are installed"
else
    echo "⚠ Missing dependencies: ${MISSING_DEPS[*]}"
    echo ""
    echo "Please install them manually:"
    case $OS in
        ubuntu|debian|linuxmint)
            echo "  sudo apt-get install build-essential cmake git libsdl2-dev libfreetype6-dev pkg-config fonts-nanum"
            ;;
        fedora|rhel|centos)
            echo "  sudo dnf install gcc cmake git SDL2-devel freetype-devel pkg-config google-nanum-fonts"
            ;;
        arch|manjaro)
            echo "  sudo pacman -S base-devel cmake git sdl2 freetype2 pkg-config ttf-nanum"
            ;;
    esac
    echo ""
    echo "Continuing with setup..."
fi
echo ""

# Setup fonts directory
echo "Step 2: Setting up Korean fonts..."
mkdir -p fonts

# Find NanumGothic font
FONT_FOUND=0
FONT_PATHS=(
    "/usr/share/fonts/truetype/nanum/NanumGothic.ttf"
    "/usr/share/fonts/nanum/NanumGothic.ttf"
    "/usr/share/fonts/TTF/NanumGothic.ttf"
    "/usr/share/fonts/truetype/nanum-gothic/NanumGothic.ttf"
)

for font_path in "${FONT_PATHS[@]}"; do
    if [ -f "$font_path" ]; then
        echo "Found NanumGothic font: $font_path"
        cp "$font_path" fonts/NanumGothic.ttf
        FONT_FOUND=1
        break
    fi
done

if [ $FONT_FOUND -eq 0 ]; then
    echo "Warning: NanumGothic.ttf not found in standard locations"
    echo "Attempting to download from alternative source..."

    # Try to download font
    if command -v wget &> /dev/null; then
        wget -O fonts/NanumGothic.ttf "https://github.com/google/fonts/raw/main/ofl/nanumgothic/NanumGothic-Regular.ttf" 2>/dev/null || true
    elif command -v curl &> /dev/null; then
        curl -L -o fonts/NanumGothic.ttf "https://github.com/google/fonts/raw/main/ofl/nanumgothic/NanumGothic-Regular.ttf" 2>/dev/null || true
    fi

    if [ -f "fonts/NanumGothic.ttf" ]; then
        echo "✓ Font downloaded successfully"
    else
        echo "✗ Could not download font automatically"
        echo "Please manually download NanumGothic.ttf and place it in the fonts/ directory"
    fi
else
    echo "✓ Font copied successfully"
fi

echo ""

# Clone LVGL if needed
echo "Step 3: Setting up LVGL library..."
if [ ! -d "lvgl" ]; then
    echo "Cloning LVGL library..."
    git clone --depth 1 --branch release/v9.3 https://github.com/lvgl/lvgl.git
    echo "✓ LVGL cloned"
else
    echo "✓ LVGL already exists"
fi

echo ""
echo "=== Installation Complete ==="
echo ""
echo "Font location: $(pwd)/fonts/NanumGothic.ttf"
echo ""
echo "Next steps:"
echo "  1. Build: ./build.sh"
echo "  2. Run:   ./run.sh"
echo ""
