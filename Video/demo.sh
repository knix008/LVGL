#!/bin/bash

# Demo script for LVGL Video Player

echo "=================================="
echo "LVGL Video Player Demo"
echo "=================================="
echo ""

# Check if video player exists
if [ ! -f "./video_player" ]; then
    echo "Video player not found. Building..."
    echo ""
    
    # Check dependencies
    echo "Checking dependencies..."
    if ! pkg-config --exists sdl2; then
        echo "Installing SDL2..."
        sudo apt-get update
        sudo apt-get install -y libsdl2-dev
    fi
    
    if ! pkg-config --exists freetype2; then
        echo "Installing FreeType..."
        sudo apt-get install -y libfreetype6-dev
    fi
    
    # Generate fonts if needed
    if [ ! -f "nanum_gothic_16.c" ] || [ ! -f "nanum_gothic_bold_16.c" ]; then
        echo "Generating Korean fonts..."
        if command -v lv_font_conv >/dev/null 2>&1; then
            ./generate_fonts.sh
        else
            echo "lv_font_conv not found. Installing..."
            curl -fsSL https://deb.nodesource.com/setup_18.x | sudo -E bash -
            sudo apt-get install -y nodejs
            sudo npm install -g lv_font_conv
            ./generate_fonts.sh
        fi
    fi
    
    # Build LVGL if needed
    if [ ! -f "lvgl/build/liblvgl.a" ]; then
        echo "Building LVGL library..."
        ./setup.sh
    fi
    
    # Build the application
    echo "Building video player..."
    make
fi

echo "Starting video player demo..."
echo ""
echo "Controls:"
echo "  - Click files in the right panel to select them"
echo "  - Use play/pause/stop buttons to control playback"
echo "  - Drag the progress bar to seek"
echo "  - Adjust volume with the slider"
echo ""
echo "Press Ctrl+C to exit"
echo ""

# Run the video player
./video_player
