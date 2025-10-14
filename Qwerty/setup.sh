#!/bin/bash

# Setup script for Korean/English QWERTY Keypad with LVGL
# This script sets up the build environment and dependencies

set -e  # Exit on error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}LVGL Korean Keyboard Setup Script${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to check if a package is installed (Debian/Ubuntu)
package_installed() {
    dpkg -l "$1" 2>/dev/null | grep -q "^ii"
}

# Detect OS
echo -e "${YELLOW}Detecting operating system...${NC}"
if [ -f /etc/os-release ]; then
    . /etc/os-release
    OS=$ID
    echo -e "${GREEN}Detected OS: $OS${NC}"
else
    echo -e "${RED}Cannot detect OS. This script supports Debian/Ubuntu-based systems.${NC}"
    exit 1
fi

# Check for required build tools
echo ""
echo -e "${YELLOW}Checking for required build tools...${NC}"

MISSING_TOOLS=()

if ! command_exists gcc; then
    MISSING_TOOLS+=("gcc")
fi

if ! command_exists make; then
    MISSING_TOOLS+=("make")
fi

if ! command_exists pkg-config; then
    MISSING_TOOLS+=("pkg-config")
fi

# Check for SDL2 development libraries
if ! pkg-config --exists sdl2; then
    MISSING_TOOLS+=("libsdl2-dev")
fi

# Install missing dependencies
if [ ${#MISSING_TOOLS[@]} -gt 0 ]; then
    echo -e "${YELLOW}Missing dependencies: ${MISSING_TOOLS[*]}${NC}"
    echo -e "${YELLOW}Attempting to install missing dependencies...${NC}"
    
    if [ "$OS" = "ubuntu" ] || [ "$OS" = "debian" ]; then
        echo -e "${BLUE}Running: sudo apt-get update${NC}"
        sudo apt-get update
        
        PACKAGES=""
        for tool in "${MISSING_TOOLS[@]}"; do
            if [ "$tool" = "gcc" ] || [ "$tool" = "make" ]; then
                PACKAGES="$PACKAGES build-essential"
            else
                PACKAGES="$PACKAGES $tool"
            fi
        done
        
        echo -e "${BLUE}Running: sudo apt-get install -y $PACKAGES${NC}"
        sudo apt-get install -y $PACKAGES
    else
        echo -e "${RED}Unsupported OS for automatic package installation.${NC}"
        echo -e "${RED}Please install the following manually: ${MISSING_TOOLS[*]}${NC}"
        exit 1
    fi
else
    echo -e "${GREEN}All required build tools are installed.${NC}"
fi

# Check for LVGL
echo ""
echo -e "${YELLOW}Checking for LVGL library...${NC}"

LVGL_FOUND=0

# Check if LVGL is in the project directory
if [ -d "lvgl" ]; then
    echo -e "${GREEN}Found LVGL in project directory: ./lvgl${NC}"
    LVGL_FOUND=1
fi

# Check if LVGL is installed system-wide
if pkg-config --exists lvgl 2>/dev/null; then
    LVGL_VERSION=$(pkg-config --modversion lvgl)
    echo -e "${GREEN}Found LVGL installed system-wide (version: $LVGL_VERSION)${NC}"
    LVGL_FOUND=1
fi

# If LVGL is not found, clone it
if [ $LVGL_FOUND -eq 0 ]; then
    echo -e "${YELLOW}LVGL not found. Cloning from GitHub...${NC}"
    
    if command_exists git; then
        git clone https://github.com/lvgl/lvgl.git
        cd lvgl
        echo -e "${BLUE}Checking out version 9.x...${NC}"
        git checkout release/v9.2
        cd ..
        echo -e "${GREEN}LVGL cloned successfully.${NC}"
    else
        echo -e "${RED}Git is not installed. Please install git to clone LVGL.${NC}"
        echo -e "${YELLOW}Alternatively, download LVGL manually from: https://github.com/lvgl/lvgl${NC}"
        exit 1
    fi
fi

# Verify lv_conf.h exists
echo ""
echo -e "${YELLOW}Verifying LVGL configuration...${NC}"
if [ -f "lv_conf.h" ]; then
    echo -e "${GREEN}Found lv_conf.h in project directory.${NC}"
else
    echo -e "${RED}lv_conf.h not found!${NC}"
    echo -e "${YELLOW}Creating default lv_conf.h from LVGL template...${NC}"
    
    if [ -f "lvgl/lv_conf_template.h" ]; then
        cp lvgl/lv_conf_template.h lv_conf.h
        # Enable the configuration
        sed -i 's/#if 0 \/\* Set this to "1" to enable content \*\//#if 1 \/* Set this to "1" to enable content *\//' lv_conf.h
        echo -e "${GREEN}Created lv_conf.h. You may need to customize it.${NC}"
    else
        echo -e "${RED}Cannot find LVGL template configuration.${NC}"
        exit 1
    fi
fi

# Create assets directory if it doesn't exist
echo ""
echo -e "${YELLOW}Setting up assets directory...${NC}"
mkdir -p assets
echo -e "${GREEN}Assets directory ready.${NC}"

# Check for font converter tool
echo ""
echo -e "${YELLOW}Checking for font conversion tools...${NC}"
if command_exists lv_font_conv; then
    echo -e "${GREEN}lv_font_conv is installed.${NC}"
    echo -e "${BLUE}You can generate Korean fonts using:${NC}"
    echo -e "  lv_font_conv --font <font.ttf> --size 20 --bpp 4 \\"
    echo -e "    --format lvgl --no-compress \\"
    echo -e "    --range 0x20-0x7E,0xAC00-0xD7A3,0x1100-0x11FF,0x3130-0x318F \\"
    echo -e "    -o assets/korean_font_20.c"
else
    echo -e "${YELLOW}lv_font_conv not found.${NC}"
    echo -e "${BLUE}To install lv_font_conv:${NC}"
    echo -e "  npm install -g lv_font_conv"
    echo ""
    echo -e "${BLUE}Or use the online font converter:${NC}"
    echo -e "  https://lvgl.io/tools/fontconverter"
fi

# Summary
echo ""
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}Setup Complete!${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""
echo -e "${BLUE}Next steps:${NC}"
echo -e "1. ${YELLOW}Generate Korean fonts${NC} (see assets/README.md for instructions)"
echo -e "2. ${YELLOW}Build the application:${NC} make"
echo -e "3. ${YELLOW}Run the application:${NC} ./qwerty"
echo ""
echo -e "${BLUE}Useful commands:${NC}"
echo -e "  make        - Build the application"
echo -e "  make clean  - Clean build files"
echo -e "  make run    - Build and run"
echo -e "  make help   - Show Makefile help"
echo ""

exit 0

