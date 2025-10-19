#!/bin/bash

# Audio Player Dependencies Installation Script

echo "=================================="
echo "Audio Player Dependencies Installer"
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
    echo "  - SDL2_mixer development libraries"
    echo "  - FreeType development libraries"
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
        echo "  sudo apt-get install -y libsdl2-dev libsdl2-mixer-dev libfreetype6-dev"
        echo ""
        read -p "Continue? (y/N) " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            sudo apt-get update
            sudo apt-get install -y libsdl2-dev libsdl2-mixer-dev libfreetype6-dev
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
        echo "  sudo dnf install -y SDL2-devel SDL2_mixer-devel freetype-devel"
        echo ""
        read -p "Continue? (y/N) " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            sudo dnf install -y SDL2-devel SDL2_mixer-devel freetype-devel
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
        echo "  sudo pacman -S sdl2 sdl2_mixer freetype2"
        echo ""
        read -p "Continue? (y/N) " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            sudo pacman -S sdl2 sdl2_mixer freetype2
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
        echo "  - SDL2_mixer development libraries"
        echo "  - FreeType development libraries"
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

if pkg-config --exists SDL2_mixer; then
    echo "✓ SDL2_mixer found ($(pkg-config --modversion SDL2_mixer))"
else
    echo "✗ SDL2_mixer not found via pkg-config"
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
echo "  2. Build the audio player: make"
echo "  3. Run the audio player: ./audio_player"
echo ""

