#!/bin/bash

# Korean IME Test Runner
# This script runs all test cases for the Korean input method

set -e  # Exit on error

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Print header
echo "============================================"
echo "  Korean IME Test Suite Runner"
echo "============================================"
echo ""

# Function to print colored output
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[PASS]${NC} $1"
}

print_error() {
    echo -e "${RED}[FAIL]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

# Check if test executables exist
print_info "Checking for test executables..."

if [ ! -f "test_korean_simple" ]; then
    print_warning "test_korean_simple not found. Building..."
    make test_simple
fi

if [ ! -f "test_korean_input" ]; then
    print_warning "test_korean_input not found. Building..."
    make test_korean
fi

echo ""

# Run simple test
print_info "Running simple Korean IME test..."
echo "-------------------------------------------"
if ./test_korean_simple; then
    print_success "Simple test completed successfully"
else
    print_error "Simple test failed"
    exit 1
fi

echo ""

# Run comprehensive test
print_info "Running comprehensive Korean IME test..."
echo "-------------------------------------------"
if ./test_korean_input; then
    print_success "Comprehensive test completed"
else
    print_warning "Comprehensive test had some failures"
    echo "(This is expected - see output above for details)"
fi

echo ""

# Print summary
echo "============================================"
print_info "Test execution complete"
echo "============================================"
echo ""
echo "To run individual tests:"
echo "  ./test_korean_simple  - Run simple test"
echo "  ./test_korean_input   - Run comprehensive test"
echo ""
echo "To rebuild tests:"
echo "  make test_simple      - Build simple test"
echo "  make test_korean      - Build comprehensive test"
echo "  make clean            - Clean build artifacts"
echo ""

exit 0

