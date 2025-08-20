#!/bin/bash

# LVGL WebServer Application Runner
# This script builds and runs the LVGL application with embedded web server

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

# Function to check if required tools are available
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
    
    if ! pkg-config --exists freetype2; then
        print_error "FreeType2 is not installed. Please install libfreetype6-dev."
        exit 1
    fi
    
    if ! pkg-config --exists sdl2; then
        print_error "SDL2 is not installed. Please install libsdl2-dev."
        exit 1
    fi
    
    # Check for OpenSSL (required for TLS)
    if ! pkg-config --exists openssl; then
        print_warning "OpenSSL is not installed. TLS will be disabled."
        print_warning "Install libssl-dev to enable TLS 1.3 support."
    else
        print_success "OpenSSL found - TLS 1.3 support available"
    fi
    
    print_success "All dependencies are available"
}

# Function to build the application
build_application() {
    print_status "Building application..."
    
    cd Source
    
    # Create build directory if it doesn't exist
    if [ ! -d "build" ]; then
        print_status "Creating build directory..."
        mkdir -p build
    fi
    
    cd build
    
    # Configure with CMake
    print_status "Configuring with CMake..."
    cmake .. || {
        print_error "CMake configuration failed"
        exit 1
    }
    
    # Build the application
    print_status "Building application..."
    make main || {
        print_error "Build failed"
        exit 1
    }
    
    print_success "Application built successfully"
    cd ../..
}

# Function to run the application
run_application() {
    print_status "Starting LVGL application with web server..."
    
    cd Source/build
    
    # Check if the executable exists
    if [ ! -f "main" ]; then
        print_error "Executable 'main' not found. Please build the application first."
        exit 1
    fi
    
    print_success "Application starting..."
    print_status "Web interface will be available at: http://localhost:8080"
    print_status "Press Ctrl+C to stop the application"
    echo ""
    
    # Run the application
    ./main
}

# Function to clean build artifacts
clean_build() {
    print_status "Cleaning build artifacts..."
    cd Source
    rm -rf build
    print_success "Build artifacts cleaned"
    cd ..
}

# Function to show help
show_help() {
    echo "LVGL WebServer Application Runner"
    echo ""
    echo "Usage: $0 [OPTION]"
    echo ""
    echo "Options:"
    echo "  run, -r, --run      Build and run the application (default)"
    echo "  build, -b, --build  Build the application only"
    echo "  clean, -c, --clean  Clean build artifacts"
    echo "  help, -h, --help    Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0                 # Build and run"
    echo "  $0 build          # Build only"
    echo "  $0 clean          # Clean build artifacts"
    echo ""
}

# Main script logic
main() {
    case "${1:-run}" in
        "run"|"-r"|"--run")
            check_dependencies
            build_application
            run_application
            ;;
        "build"|"-b"|"--build")
            check_dependencies
            build_application
            ;;
        "clean"|"-c"|"--clean")
            clean_build
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

# Run main function with all arguments
main "$@"
