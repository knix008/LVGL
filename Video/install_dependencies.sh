#!/bin/bash

# Video Player Dependencies Installation Script

echo "=================================="
echo "Video Player Dependencies Installer"
echo "=================================="
echo ""

# Detect OS
if [ -f /etc/os-release ]; then
    . /etc/os-release
    OS=$ID
else
    echo "Cannot detect OS. Please install dependencies manually."
    echo ""
    echo "Required packages:"
    echo "  - SDL2 development libraries"
    echo "  - FreeType development libraries"
    echo "  - FFmpeg development libraries (libavcodec, libavformat, libavutil, libswscale, libswresample)"
    exit 1
fi

echo "Detected OS: $OS"
echo ""

case "$OS" in
    ubuntu|debian|linuxmint)
        echo "Installing dependencies for Ubuntu/Debian..."
        echo ""
        echo "This will run:"
        echo "  sudo apt-get update"
        echo "  sudo apt-get install -y libsdl2-dev libfreetype6-dev libavcodec-dev libavformat-dev libavutil-dev libswscale-dev libswresample-dev"
        echo ""
        read -p "Continue? (y/N) " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            sudo apt-get update
            sudo apt-get install -y libsdl2-dev libfreetype6-dev libavcodec-dev libavformat-dev libavutil-dev libswscale-dev libswresample-dev
            echo ""
            echo "Dependencies installed successfully!"
        else
            echo "Installation cancelled."
            exit 1
        fi
        ;;
    
    fedora|rhel|centos)
        echo "Installing dependencies for Fedora/RHEL..."
        echo ""
        echo "This will run:"
        echo "  sudo dnf install -y SDL2-devel freetype-devel ffmpeg-devel"
        echo ""
        read -p "Continue? (y/N) " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            sudo dnf install -y SDL2-devel freetype-devel ffmpeg-devel
            echo ""
            echo "Dependencies installed successfully!"
        else
            echo "Installation cancelled."
            exit 1
        fi
        ;;
    
    arch|manjaro)
        echo "Installing dependencies for Arch Linux..."
        echo ""
        echo "This will run:"
        echo "  sudo pacman -S sdl2 freetype2 ffmpeg"
        echo ""
        read -p "Continue? (y/N) " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            sudo pacman -S sdl2 freetype2 ffmpeg
            echo ""
            echo "Dependencies installed successfully!"
        else
            echo "Installation cancelled."
            exit 1
        fi
        ;;
    
    *)
        echo "Unsupported OS: $OS"
        echo ""
        echo "Please install these packages manually:"
        echo "  - SDL2 development libraries"
        echo "  - FreeType development libraries"
        echo "  - FFmpeg development libraries (libavcodec, libavformat, libavutil, libswscale, libswresample)"
        exit 1
        ;;
esac

echo ""
echo "=================================="
echo "Verifying installation..."
echo "=================================="
echo ""

# Verify pkg-config can find the libraries
if pkg-config --exists sdl2; then
    echo "✓ SDL2 found ($(pkg-config --modversion sdl2))"
else
    echo "✗ SDL2 not found via pkg-config"
fi

if pkg-config --exists libavcodec; then
    echo "✓ FFmpeg libavcodec found ($(pkg-config --modversion libavcodec))"
else
    echo "✗ FFmpeg libavcodec not found via pkg-config"
fi

if pkg-config --exists libavformat; then
    echo "✓ FFmpeg libavformat found ($(pkg-config --modversion libavformat))"
else
    echo "✗ FFmpeg libavformat not found via pkg-config"
fi

if pkg-config --exists libavutil; then
    echo "✓ FFmpeg libavutil found ($(pkg-config --modversion libavutil))"
else
    echo "✗ FFmpeg libavutil not found via pkg-config"
fi

if pkg-config --exists libswscale; then
    echo "✓ FFmpeg libswscale found ($(pkg-config --modversion libswscale))"
else
    echo "✗ FFmpeg libswscale not found via pkg-config"
fi

if pkg-config --exists libswresample; then
    echo "✓ FFmpeg libswresample found ($(pkg-config --modversion libswresample))"
else
    echo "✗ FFmpeg libswresample not found via pkg-config"
fi

if pkg-config --exists freetype2; then
    echo "✓ FreeType found ($(pkg-config --modversion freetype2))"
else
    echo "✗ FreeType not found via pkg-config"
fi

echo ""
echo "=================================="
echo "Installation complete!"
echo "=================================="
echo ""
echo "Next steps:"
echo "  1. Build LVGL (if not already done): ./setup.sh"
echo "  2. Build the video player: make"
echo "  3. Run the video player: ./main"
echo ""

