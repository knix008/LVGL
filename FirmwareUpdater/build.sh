#!/bin/bash

# Build script for Device Simulator and Firmware Uploader

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to show usage
show_usage() {
    echo "Build Script for Device Simulator & Firmware Uploader"
    echo ""
    echo "Usage: $0 [command]"
    echo ""
    echo "Commands:"
    echo "  (none)    Build the project (default)"
    echo "  clean     Clean build artifacts and certificates"
    echo "  rebuild   Clean and rebuild everything"
    echo "  help      Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0              # Build the project"
    echo "  $0 clean        # Clean everything"
    echo "  $0 rebuild      # Clean and rebuild"
    echo ""
}

# Function to clean the project
clean_project() {
    echo -e "${BLUE}============================================${NC}"
    echo -e "${BLUE}Cleaning Project${NC}"
    echo -e "${BLUE}============================================${NC}"
    echo ""

    # Stop any running simulator
    if pgrep -x "device_simulator" > /dev/null; then
        echo -e "${YELLOW}Stopping running device simulator...${NC}"
        pkill -9 device_simulator 2>/dev/null || true
        echo -e "${GREEN}✓ Stopped device simulator${NC}"
    fi

    # Remove build directory
    if [ -d "build" ]; then
        echo -e "${YELLOW}Removing build directory...${NC}"
        rm -rf build
        echo -e "${GREEN}✓ Removed build/${NC}"
    fi

    # Remove certificates
    if [ -d "certs" ]; then
        echo -e "${YELLOW}Removing certificates...${NC}"
        rm -rf certs
        echo -e "${GREEN}✓ Removed certs/${NC}"
    fi

    # Remove test files
    if [ -d "test_firmwares" ]; then
        echo -e "${YELLOW}Removing test firmware directory...${NC}"
        rm -rf test_firmwares
        echo -e "${GREEN}✓ Removed test_firmwares/${NC}"
    fi

    # Remove received firmwares
    if [ -d "received_firmwares" ]; then
        echo -e "${YELLOW}Removing received firmware directory...${NC}"
        rm -rf received_firmwares
        echo -e "${GREEN}✓ Removed received_firmwares/${NC}"
    fi

    # Remove other generated files
    echo -e "${YELLOW}Removing temporary files...${NC}"
    rm -f test_firmware*.bin 2>/dev/null || true
    rm -f *.bin 2>/dev/null || true
    rm -f *.log 2>/dev/null || true
    echo -e "${GREEN}✓ Removed temporary files${NC}"

    echo ""
    echo -e "${GREEN}============================================${NC}"
    echo -e "${GREEN}✓ Clean Complete!${NC}"
    echo -e "${GREEN}============================================${NC}"
    echo ""
}

# Parse command line arguments
if [ "$1" = "clean" ]; then
    clean_project
    exit 0
elif [ "$1" = "rebuild" ]; then
    clean_project
    echo ""
    # Continue to build
elif [ "$1" = "help" ] || [ "$1" = "--help" ] || [ "$1" = "-h" ]; then
    show_usage
    exit 0
elif [ -n "$1" ]; then
    echo -e "${RED}Error: Unknown command '$1'${NC}"
    echo ""
    show_usage
    exit 1
fi

echo -e "${BLUE}============================================${NC}"
echo -e "${BLUE}Building Device Simulator & Firmware Uploader${NC}"
echo -e "${BLUE}============================================${NC}"
echo ""

# Check if dependencies are installed
echo -e "${YELLOW}Checking dependencies...${NC}"

if ! command -v cmake &> /dev/null; then
    echo -e "${RED}Error: cmake is not installed${NC}"
    echo "Run: ./install_dependencies.sh"
    exit 1
fi

if ! command -v g++ &> /dev/null; then
    echo -e "${RED}Error: g++ is not installed${NC}"
    echo "Run: ./install_dependencies.sh"
    exit 1
fi

if ! pkg-config --exists jsoncpp 2>/dev/null; then
    echo -e "${YELLOW}Warning: jsoncpp might not be properly configured${NC}"
    echo "Attempting to build anyway..."
fi

echo -e "${GREEN}✓ Dependencies OK${NC}"
echo ""

# Generate certificates if they don't exist
if [ ! -f "certs/device.crt" ] || [ ! -f "certs/device.key" ]; then
    echo -e "${YELLOW}Certificates not found. Generating...${NC}"
    ./generate_certs.sh
    echo ""
fi

# Create build directory
echo -e "${YELLOW}Creating build directory...${NC}"
mkdir -p build
cd build

# Run CMake
echo -e "${YELLOW}Running CMake...${NC}"
cmake .. || {
    echo -e "${RED}CMake configuration failed!${NC}"
    exit 1
}
echo ""

# Build
echo -e "${YELLOW}Building project...${NC}"
make -j$(nproc) || {
    echo -e "${RED}Build failed!${NC}"
    exit 1
}
echo ""

# Check if executables were created
if [ -f "device_simulator" ] && [ -f "firmware_uploader" ]; then
    echo -e "${GREEN}============================================${NC}"
    echo -e "${GREEN}✓ Build Successful!${NC}"
    echo -e "${GREEN}============================================${NC}"
    echo ""
    echo "Built executables:"
    ls -lh device_simulator firmware_uploader
    echo ""
    echo -e "${BLUE}Next steps:${NC}"
    echo "  1. Run device simulator:"
    echo -e "     ${GREEN}./run.sh${NC}"
    echo ""
    echo "  2. In another terminal, upload firmware:"
    echo -e "     ${GREEN}cd build${NC}"
    echo -e "     ${GREEN}./firmware_uploader --version 2.0.0 --url https://example.com/fw.bin${NC}"
    echo ""
    echo "  3. Or run automated tests:"
    echo -e "     ${GREEN}cd build && ./test_upload.sh${NC}"
    echo ""
else
    echo -e "${RED}Build completed but executables not found!${NC}"
    exit 1
fi
