#!/bin/bash

# Webcam Application Build Script
# This script builds the webcam application with proper configuration

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

# Function to check dependencies
check_dependencies() {
    print_status "Checking dependencies..."
    
    if ! command -v cmake &> /dev/null; then
        print_error "cmake is not installed. Please install cmake first."
        exit 1
    fi
    
    if ! command -v make &> /dev/null; then
        print_error "make is not installed. Please install make first."
        exit 1
    fi
    
    if ! pkg-config --exists opencv4; then
        print_error "OpenCV is not installed. Please install libopencv-dev"
        exit 1
    fi
    
    print_success "All dependencies are available"
}

# Function to build the application
build_application() {
    local build_type=${1:-Release}
    
    print_status "Building webcam application (${build_type})..."
    
    # Create build directory
    mkdir -p build
    cd build
    
    # Configure with CMake
    cmake -DCMAKE_BUILD_TYPE=${build_type} ..
    
    # Build
    make -j$(nproc)
    
    print_success "Build completed successfully"
}

# Function to clean build artifacts
clean_build() {
    print_status "Cleaning build artifacts..."
    rm -rf build/
    print_success "Clean completed"
}

# Function to run tests
run_tests() {
    print_status "Running tests..."
    cd build
    make test_webcam
    print_success "Tests completed"
}

# Function to install
install_app() {
    print_status "Installing webcam application..."
    cd build
    sudo make install
    print_success "Installation completed"
}

# Function to show help
show_help() {
    echo "Usage: $0 [OPTION]"
    echo ""
    echo "Options:"
    echo "  build [type]    Build the application (default: Release)"
    echo "  clean           Clean build artifacts"
    echo "  test            Run tests"
    echo "  install         Install the application"
    echo "  all             Build, test, and install"
    echo "  help            Show this help message"
    echo ""
    echo "Build types:"
    echo "  Release         Optimized release build (default)"
    echo "  Debug           Debug build with symbols"
    echo ""
    echo "Examples:"
    echo "  $0 build        # Build with Release configuration"
    echo "  $0 build Debug  # Build with Debug configuration"
    echo "  $0 clean        # Clean build artifacts"
    echo "  $0 test         # Run tests"
    echo "  $0 install      # Install the application"
}

# Main function
main() {
    case "${1:-build}" in
        "build")
            check_dependencies
            build_application "${2:-Release}"
            ;;
        "clean")
            clean_build
            ;;
        "test")
            if [ ! -d "build" ]; then
                print_error "Build directory not found. Run 'build' first."
                exit 1
            fi
            run_tests
            ;;
        "install")
            if [ ! -d "build" ]; then
                print_error "Build directory not found. Run 'build' first."
                exit 1
            fi
            install_app
            ;;
        "all")
            check_dependencies
            build_application Release
            run_tests
            print_warning "Skipping install (requires sudo). Run '$0 install' separately if needed."
            ;;
        "help"|"-h"|"--help")
            show_help
            ;;
        *)
            print_error "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
}

# Run main function
main "$@"
