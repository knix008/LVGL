#!/bin/bash

# Japanese Input Application Test Runner
# This script runs all test cases for the Japanese input application

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
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

print_warning() {
    echo -e "${YELLOW}⚠ $1${NC}"
}

print_info() {
    echo -e "${BLUE}ℹ $1${NC}"
}

# Check if we're in the right directory
if [ ! -f "main.c" ] || [ ! -f "japanese_input.h" ]; then
    print_error "Please run this script from the project root directory"
    exit 1
fi

# Check if tests directory exists
if [ ! -d "tests" ]; then
    print_error "Tests directory not found. Please create tests first."
    exit 1
fi

print_header "Japanese Input Application Test Suite"

# Change to tests directory
cd tests

print_info "Compiling test cases..."

# Clean previous builds
make clean

# Compile and run core tests
print_header "Running Core Logic Tests"
if make test-core; then
    print_success "Core logic tests completed successfully"
else
    print_error "Core logic tests failed"
    exit 1
fi

echo ""

# Compile and run GUI tests
print_header "Running GUI Function Tests"
if make test-gui; then
    print_success "GUI function tests completed successfully"
else
    print_error "GUI function tests failed"
    exit 1
fi

echo ""

# Run all tests together
print_header "Running Complete Test Suite"
if make all; then
    print_success "All tests completed successfully!"
    echo ""
    print_info "Test Summary:"
    echo "  - Core logic functionality: ✓"
    echo "  - GUI function functionality: ✓"
    echo "  - Character mappings: ✓"
    echo "  - Mode switching: ✓"
    echo "  - Flick input: ✓"
    echo "  - UTF-8 conversion: ✓"
    echo "  - Edge cases: ✓"
else
    print_error "Some tests failed!"
    exit 1
fi

echo ""
print_header "Test Results Summary"
print_success "All test cases have been executed"
print_info "Check the output above for detailed results"

# Return to project root
cd ..

print_info "Test execution completed. Return to project root."
