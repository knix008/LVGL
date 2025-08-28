#!/bin/bash

# LVGL Face Recognition Application Runner
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
    print_status "Checking system dependencies..."
    
    if ! command -v cmake &> /dev/null; then
        print_error "cmake is not installed. Please install cmake first."
        exit 1
    fi
    
    if ! command -v make &> /dev/null; then
        print_error "make is not installed. Please install make first."
        exit 1
    fi
    
    if ! command -v pkg-config &> /dev/null; then
        print_error "pkg-config is not installed. Please install pkg-config first."
        exit 1
    fi
    
    # Check for system libraries that are still needed
    if ! pkg-config --exists libavformat libavcodec libavutil libswscale libswresample; then
        print_warning "FFmpeg system libraries not found. Video playback may not work."
        print_warning "Install libavformat-dev libavcodec-dev libavutil-dev libswscale-dev libswresample-dev"
    else
        print_success "FFmpeg system libraries found"
    fi
    
    print_success "All system dependencies are available"
    print_status "Note: All other dependencies (FreeType2, SDL2, OpenSSL, MariaDB, LVGL) will be built from source"
}

# Function to build all libraries
build_libraries() {
    print_status "Building all libraries from source..."
    
    cd Source
    
    # Check if build_all_libs.sh exists
    if [ ! -f "build_all_libs.sh" ]; then
        print_error "build_all_libs.sh not found in Source directory"
        exit 1
    fi
    
    # Make the script executable and run it
    chmod +x build_all_libs.sh
    ./build_all_libs.sh all || {
        print_error "Library build failed"
        exit 1
    }
    
    print_success "All libraries built successfully from source"
    print_status "Source directories have been cleaned up to save disk space"
    cd ..
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
    make || {
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
    print_status "HTTPS interface will be available at: https://localhost:8443"
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
    echo "LVGL Face Recognition Application Runner"
    echo ""
    echo "Usage: $0 [OPTION]"
    echo ""
    echo "Options:"
    echo "  run, -r, --run      Build all libraries, application and run (default)"
    echo "  build, -b, --build  Build all libraries and application only"
    echo "  libs, -l, --libs    Build all libraries only"
    echo "  clean, -c, --clean  Clean build artifacts"
    echo "  help, -h, --help    Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0                 # Build everything and run"
    echo "  $0 build          # Build libraries and application"
    echo "  $0 libs           # Build libraries only"
    echo "  $0 clean          # Clean build artifacts"
    echo ""
    echo "Features:"
    echo "  - Automatic library building from source"
    echo "  - Source cleanup after successful builds"
    echo "  - Database operations with MariaDB"
    echo "  - Web interface with HTTPS support"
    echo "  - Korean input methods"
    echo "  - Video playback with FFmpeg"
    echo ""
}

# Main script logic
main() {
    case "${1:-run}" in
        "run"|"-r"|"--run")
            check_dependencies
            build_libraries
            build_application
            run_application
            ;;
        "build"|"-b"|"--build")
            check_dependencies
            build_libraries
            build_application
            ;;
        "libs"|"-l"|"--libs")
            check_dependencies
            build_libraries
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
