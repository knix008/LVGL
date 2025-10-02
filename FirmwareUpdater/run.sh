#!/bin/bash

# Run script for Device Simulator

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default values
DEVICE_ID="DEVICE-SIM-001"
PORT=8443
VERSION="1.0.0"
CERT_FILE="certs/device.crt"
KEY_FILE="certs/device.key"

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --device-id)
            DEVICE_ID="$2"
            shift 2
            ;;
        --port)
            PORT="$2"
            shift 2
            ;;
        --version)
            VERSION="$2"
            shift 2
            ;;
        --help)
            echo "Device Simulator Runner"
            echo ""
            echo "Usage: $0 [options]"
            echo ""
            echo "Options:"
            echo "  --device-id ID    Device ID (default: DEVICE-SIM-001)"
            echo "  --port PORT       Port to listen on (default: 8443)"
            echo "  --version VER     Initial firmware version (default: 1.0.0)"
            echo "  --help            Show this help message"
            echo ""
            echo "Examples:"
            echo "  $0"
            echo "  $0 --device-id DEVICE-001 --port 9443"
            echo ""
            exit 0
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            echo "Run '$0 --help' for usage information"
            exit 1
            ;;
    esac
done

echo -e "${BLUE}============================================${NC}"
echo -e "${BLUE}Device Simulator${NC}"
echo -e "${BLUE}============================================${NC}"
echo ""

# Check if executable exists
if [ ! -f "build/device_simulator" ]; then
    echo -e "${RED}Error: device_simulator not found!${NC}"
    echo "Please build the project first:"
    echo -e "  ${GREEN}./build.sh${NC}"
    exit 1
fi

# Check if certificates exist
if [ ! -f "$CERT_FILE" ] || [ ! -f "$KEY_FILE" ]; then
    echo -e "${RED}Error: Certificates not found!${NC}"
    echo "Generating certificates..."
    ./generate_certs.sh
    echo ""
fi

# Check if port is already in use
if lsof -Pi :$PORT -sTCP:LISTEN -t >/dev/null 2>&1; then
    echo -e "${YELLOW}Warning: Port $PORT is already in use${NC}"
    echo "You may want to:"
    echo "  1. Stop the existing process: pkill device_simulator"
    echo "  2. Use a different port: $0 --port 9443"
    echo ""
    read -p "Continue anyway? (y/N) " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

echo -e "${GREEN}Starting Device Simulator...${NC}"
echo ""
echo -e "${BLUE}Configuration:${NC}"
echo "  Device ID:      $DEVICE_ID"
echo "  Port:           $PORT"
echo "  Version:        $VERSION"
echo "  Certificate:    $CERT_FILE"
echo "  Private Key:    $KEY_FILE"
echo ""
echo -e "${BLUE}Endpoints:${NC}"
echo "  Firmware Update: https://localhost:$PORT/api/v1.0/updatefirmware"
echo "  Status:          https://localhost:$PORT/api/v1.0/status"
echo "  Health Check:    https://localhost:$PORT/api/v1.0/health"
echo ""
echo -e "${YELLOW}Press Ctrl+C to stop the simulator${NC}"
echo ""
echo -e "${BLUE}============================================${NC}"
echo ""

# Run the simulator
cd build
exec ./device_simulator \
    --device-id "$DEVICE_ID" \
    --port "$PORT" \
    --version "$VERSION" \
    --cert "../$CERT_FILE" \
    --key "../$KEY_FILE"
