#!/bin/bash

# Build script for Device Simulator and Firmware Uploader

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

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
