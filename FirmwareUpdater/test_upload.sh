#!/bin/bash

# Test script for firmware upload
# This script creates test firmware files and uploads them to the device simulator

set -e

# Detect if we're in project root or build directory
if [ -f "firmware_uploader" ]; then
    # Running from build directory
    UPLOADER="./firmware_uploader"
    BUILD_DIR="."
elif [ -f "build/firmware_uploader" ]; then
    # Running from project root
    UPLOADER="build/firmware_uploader"
    BUILD_DIR="build"
else
    echo -e "\033[0;31mError: firmware_uploader not found!\033[0m"
    echo "Please build the project first:"
    echo "  ./build.sh"
    exit 1
fi

HOST="localhost"
PORT="8443"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}============================================${NC}"
echo -e "${BLUE}Firmware Upload Test Script${NC}"
echo -e "${BLUE}============================================${NC}"
echo ""

# Function to create test firmware file
create_test_firmware() {
    local size=$1
    local filename=$2

    echo -e "${YELLOW}Creating test firmware: $filename (size: $size)${NC}"

    if [ "$size" = "1KB" ]; then
        dd if=/dev/urandom of="$filename" bs=1024 count=1 2>/dev/null
    elif [ "$size" = "1MB" ]; then
        dd if=/dev/urandom of="$filename" bs=1048576 count=1 2>/dev/null
    elif [ "$size" = "10MB" ]; then
        dd if=/dev/urandom of="$filename" bs=1048576 count=10 2>/dev/null
    elif [ "$size" = "100MB" ]; then
        dd if=/dev/urandom of="$filename" bs=1048576 count=100 2>/dev/null
    elif [ "$size" = "1GB" ]; then
        dd if=/dev/urandom of="$filename" bs=1048576 count=1024 2>/dev/null
    else
        echo "Unknown size: $size"
        exit 1
    fi

    echo -e "${GREEN}✓ Created: $filename ($(du -h "$filename" | cut -f1))${NC}"
    echo ""
}

# Function to run test
run_test() {
    local test_name=$1
    local command=$2

    echo -e "${BLUE}----------------------------------------${NC}"
    echo -e "${BLUE}Test: $test_name${NC}"
    echo -e "${BLUE}----------------------------------------${NC}"

    if eval "$command"; then
        echo -e "${GREEN}✓ Test passed: $test_name${NC}"
    else
        echo -e "${RED}✗ Test failed: $test_name${NC}"
        return 1
    fi
    echo ""
}

# Create test directory
TEST_DIR="test_firmwares"
mkdir -p "$TEST_DIR"

# Test 1: Get device status
echo -e "${BLUE}Test 1: Get Device Status${NC}"
echo -e "${BLUE}----------------------------------------${NC}"
run_test "Get device status" "$UPLOADER --host $HOST --port $PORT --status"

# Test 2: Upload with URL (JSON mode)
echo -e "${BLUE}Test 2: Upload Firmware via URL${NC}"
echo -e "${BLUE}----------------------------------------${NC}"
run_test "Upload via URL" "$UPLOADER --host $HOST --port $PORT --version 1.1.0 --url https://example.com/firmware_v1.1.0.bin"

# Test 3: Upload small file (1KB)
create_test_firmware "1KB" "$TEST_DIR/firmware_1kb.bin"
run_test "Upload 1KB file" "$UPLOADER --host $HOST --port $PORT --file $TEST_DIR/firmware_1kb.bin --version 1.2.0"

# Test 4: Upload medium file (1MB)
create_test_firmware "1MB" "$TEST_DIR/firmware_1mb.bin"
run_test "Upload 1MB file" "$UPLOADER --host $HOST --port $PORT --file $TEST_DIR/firmware_1mb.bin --version 1.3.0"

# Test 5: Upload larger file (10MB)
if [ "$1" = "--full" ]; then
    create_test_firmware "10MB" "$TEST_DIR/firmware_10mb.bin"
    run_test "Upload 10MB file" "$UPLOADER --host $HOST --port $PORT --file $TEST_DIR/firmware_10mb.bin --version 1.4.0"

    # Test 6: Upload very large file (100MB)
    if [ "$1" = "--full" ] && [ "$2" = "--large" ]; then
        create_test_firmware "100MB" "$TEST_DIR/firmware_100mb.bin"
        run_test "Upload 100MB file" "$UPLOADER --host $HOST --port $PORT --file $TEST_DIR/firmware_100mb.bin --version 1.5.0"
    fi
fi

# Get final status
echo -e "${BLUE}Final Device Status${NC}"
echo -e "${BLUE}----------------------------------------${NC}"
$UPLOADER --host $HOST --port $PORT --status

# Cleanup
echo ""
echo -e "${YELLOW}Cleaning up test files...${NC}"
rm -rf "$TEST_DIR"
echo -e "${GREEN}✓ Cleanup complete${NC}"

echo ""
echo -e "${BLUE}============================================${NC}"
echo -e "${GREEN}All tests completed successfully!${NC}"
echo -e "${BLUE}============================================${NC}"
echo ""
echo "Usage:"
echo "  $0              # Run basic tests (1KB, 1MB)"
echo "  $0 --full       # Run full tests including 10MB"
echo "  $0 --full --large  # Run all tests including 100MB"
echo ""
