#!/bin/bash

# Test runner script for Webcam Capture Application
# Runs all unit tests and integration tests

set -e  # Exit on error

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo "========================================"
echo "  Webcam Capture Application"
echo "  Test Suite Runner"
echo "========================================"
echo ""

# Check if test directory exists
if [ ! -d "test" ]; then
    echo -e "${RED}Error: test directory not found${NC}"
    exit 1
fi

# Build tests
echo -e "${YELLOW}Building tests...${NC}"
cd test
make clean > /dev/null 2>&1 || true
if make all; then
    echo -e "${GREEN}Build successful${NC}"
else
    echo -e "${RED}Build failed${NC}"
    exit 1
fi
echo ""

# Run tests
echo -e "${YELLOW}Running tests...${NC}"
echo "========================================"
echo ""

TEST_FAILED=0

# Run camera unit tests
echo -e "${YELLOW}[1/2] Camera Unit Tests${NC}"
if ./test_camera; then
    echo -e "${GREEN}Camera unit tests passed${NC}"
else
    echo -e "${RED}Camera unit tests failed${NC}"
    TEST_FAILED=1
fi
echo ""

# Run integration tests
echo -e "${YELLOW}[2/2] Integration Tests${NC}"
if ./test_integration; then
    echo -e "${GREEN}Integration tests passed${NC}"
else
    echo -e "${RED}Integration tests failed${NC}"
    TEST_FAILED=1
fi
echo ""

# Summary
echo "========================================"
if [ $TEST_FAILED -eq 0 ]; then
    echo -e "${GREEN}All tests passed!${NC}"
    exit 0
else
    echo -e "${RED}Some tests failed!${NC}"
    exit 1
fi
