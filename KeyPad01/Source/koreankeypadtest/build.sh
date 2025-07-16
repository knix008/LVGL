#!/bin/bash

# Build script for Korean Keypad Test Program
# This script builds the koreankeypadtest program separately

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Clean function
clean_project() {
    print_status "Cleaning build artifacts..."
    
    # Remove object files
    if [ -f "korean_test.o" ]; then
        rm -f korean_test.o
        print_status "Removed: korean_test.o"
    fi
    
    if [ -f "korean_hangul.o" ]; then
        rm -f korean_hangul.o
        print_status "Removed: korean_hangul.o"
    fi
    
    # Remove executable
    if [ -f "koreankeypadtest" ]; then
        rm -f koreankeypadtest
        print_status "Removed: koreankeypadtest"
    fi
    
    # Remove build directory if it exists
    if [ -d "build" ]; then
        rm -rf build
        print_status "Removed: build/ directory"
    fi
    
    print_success "Clean completed!"
}

# Handle command line arguments first
case "$1" in
    "clean")
        clean_project
        exit 0
        ;;
    "--help"|"-h")
        echo "Usage: $0 [OPTION]"
        echo ""
        echo "Options:"
        echo "  (no option)  Build the koreankeypadtest program"
        echo "  clean        Remove all build artifacts"
        echo "  --test       Build and run a quick test"
        echo "  --help, -h   Show this help message"
        echo ""
        echo "Examples:"
        echo "  $0           # Build the program"
        echo "  $0 clean     # Clean build artifacts"
        echo "  $0 --test    # Build and test"
        exit 0
        ;;
    "")
        # No arguments, continue with build
        ;;
    *)
        print_error "Unknown option: $1"
        print_status "Use '$0 --help' for usage information"
        exit 1
        ;;
esac

print_status "Building Korean Keypad Test Program..."

# Check if gcc is available
if ! command -v gcc &> /dev/null; then
    print_error "gcc is not installed. Please install gcc first."
    print_status "On Ubuntu/Debian: sudo apt install build-essential"
    print_status "On CentOS/RHEL: sudo yum install gcc"
    exit 1
fi

# Check if source files exist
if [ ! -f "korean_test.c" ]; then
    print_error "Source file not found: korean_test.c"
    exit 1
fi

    if [ ! -f "korean_hangul.c" ]; then
        print_error "Source file not found: korean_hangul.c"
        exit 1
    fi

    if [ ! -f "korean_hangul.h" ]; then
        print_error "Header file not found: korean_hangul.h"
        exit 1
    fi

# Create build directory if it doesn't exist
mkdir -p build

# Set compiler flags
CFLAGS="-Wall -Wextra -std=c99 -O2"
LDFLAGS=""

# Show build configuration
print_status "Build configuration:"
print_status "  Compiler: $(gcc --version | head -n1)"
print_status "  CFLAGS: $CFLAGS"
  print_status "  Source files:"
  print_status "    - korean_test.c"
  print_status "    - korean_hangul.c"
  print_status "    - korean_hangul.h"

# Compile the program
print_status "Compiling source files..."
gcc $CFLAGS \
    korean_test.c \
    korean_hangul.c \
    -o koreankeypadtest \
    $LDFLAGS

# Check if compilation was successful
if [ $? -eq 0 ]; then
    print_success "Build successful!"
    print_status "Executable created: koreankeypadtest"
    print_status "Size: $(ls -lh koreankeypadtest | awk '{print $5}')"
    print_status ""
    print_status "You can run it with:"
    print_status "  ./koreankeypadtest"
    print_status ""
    print_status "Or use the Makefile:"
    print_status "  make run"
else
    print_error "Build failed!"
    exit 1
fi

# Optional: Run a quick test
if [ "$1" = "--test" ]; then
    print_status "Running quick test..."
    echo "rk" | timeout 5s ./koreankeypadtest > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        print_success "Quick test passed!"
    else
        print_warning "Quick test failed or timed out"
    fi
fi 