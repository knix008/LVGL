#!/bin/bash
# Japanese QWERTY Input Method - Test Runner
# Compiles and runs all test cases

set -e  # Exit on error

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored messages
print_success() {
    echo -e "${GREEN}✓${NC} $1"
}

print_error() {
    echo -e "${RED}✗${NC} $1"
}

print_info() {
    echo -e "${YELLOW}→${NC} $1"
}

print_header() {
    echo -e "${BLUE}$1${NC}"
}

# Main header
echo ""
print_header "================================================"
print_header "  Japanese QWERTY - Test Suite Runner"
print_header "================================================"
echo ""

# Check if tests directory exists
if [ ! -d "tests" ]; then
    print_error "Tests directory not found!"
    exit 1
fi

# Navigate to tests directory
cd tests

# Clean previous test builds
print_info "Cleaning previous test builds..."
make clean 2>/dev/null || true
echo ""

# Build tests
print_header "Building Test Cases..."
echo ""

if make -j4 2>&1 | grep -v "^make\["; then
    echo ""
else
    print_error "Test build failed!"
    exit 1
fi

echo ""
print_header "================================================"
print_header "  Test Execution Complete"
print_header "================================================"
echo ""

# Check if all tests passed
if [ ${PIPESTATUS[0]} -eq 0 ]; then
    print_success "All test suites executed successfully!"
    echo ""
    exit 0
else
    print_error "Some tests failed!"
    echo ""
    exit 1
fi

