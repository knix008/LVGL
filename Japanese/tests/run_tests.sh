#!/bin/bash

# Simple test runner for the tests directory
# This script can be run from within the tests directory

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

print_header() {
    echo -e "${BLUE}================================${NC}"
    echo -e "${BLUE}$1${NC}"
    echo -e "${BLUE}================================${NC}"
}

print_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

print_error() {
    echo -e "${RED}✗ $1${NC}"
}

print_header "Japanese Input Tests"

# Check if Makefile exists
if [ ! -f "Makefile" ]; then
    print_error "Makefile not found in tests directory"
    exit 1
fi

# Clean and build
echo "Building tests..."
make clean
make

print_header "Running All Tests"
make all

print_success "Test execution completed!"
