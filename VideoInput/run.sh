#!/bin/bash

# LVGL VideoInput Program - Build and Run Script
# Version: 1.0
# Author: LVGL VideoInput Team

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_header() {
    echo -e "${BLUE}================================${NC}"
    echo -e "${BLUE}  LVGL VideoInput Program${NC}"
    echo -e "${BLUE}================================${NC}"
}

# Function to check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to check dependencies
check_dependencies() {
    print_status "Checking system dependencies..."
    
    local missing_deps=()
    
    # Check for required commands
    if ! command_exists cmake; then
        missing_deps+=("cmake")
    fi
    
    if ! command_exists make; then
        missing_deps+=("make")
    fi
    
    if ! command_exists git; then
        missing_deps+=("git")
    fi
    
    if ! command_exists pkg-config; then
        missing_deps+=("pkg-config")
    fi
    
    # Check for required libraries
    if ! pkg-config --exists sdl2; then
        missing_deps+=("libsdl2-dev")
    fi
    
    if ! pkg-config --exists freetype2; then
        missing_deps+=("libfreetype6-dev")
    fi
    
    if ! pkg-config --exists libavformat; then
        missing_deps+=("libavformat-dev")
    fi
    
    if ! pkg-config --exists libavcodec; then
        missing_deps+=("libavcodec-dev")
    fi
    
    if ! pkg-config --exists libswscale; then
        missing_deps+=("libswscale-dev")
    fi
    
    if ! pkg-config --exists libavutil; then
        missing_deps+=("libavutil-dev")
    fi
    
    # Note: SQLCipher is built from source, no system package needed
    
    if [ ${#missing_deps[@]} -ne 0 ]; then
        print_error "Missing dependencies:"
        for dep in "${missing_deps[@]}"; do
            echo "  - $dep"
        done
        echo ""
        print_warning "Please install missing dependencies:"
        echo "sudo apt update && sudo apt install -y ${missing_deps[*]}"
        return 1
    fi
    
    print_status "All dependencies are installed."
    return 0
}

# Function to build OpenCV
build_opencv() {
    print_status "Checking OpenCV installation..."
    
    if [ -d "Source/opencv_install" ]; then
        print_status "OpenCV already built locally."
        return 0
    fi
    
    print_status "Building OpenCV locally..."
    
    cd Source || exit 1
    
    # Clone OpenCV if not exists
    if [ ! -d "opencv" ]; then
        print_status "Cloning OpenCV repository..."
        git clone --depth 1 --branch 4.8.0 https://github.com/opencv/opencv.git
    fi
    
    # Create build directory
    mkdir -p opencv_build
    cd opencv_build || exit 1
    
    print_status "Configuring OpenCV..."
    cmake ../opencv \
        -DBUILD_TESTS=OFF \
        -DBUILD_PERF_TESTS=OFF \
        -DBUILD_EXAMPLES=OFF \
        -DBUILD_opencv_apps=OFF \
        -DBUILD_opencv_java=OFF \
        -DBUILD_opencv_python=OFF \
        -DBUILD_opencv_python2=OFF \
        -DBUILD_opencv_python3=OFF \
        -DBUILD_opencv_js=OFF \
        -DBUILD_opencv_ts=OFF \
        -DBUILD_opencv_world=OFF \
        -DCMAKE_INSTALL_PREFIX=../opencv_install
    
    if [ $? -ne 0 ]; then
        print_error "OpenCV configuration failed."
        return 1
    fi
    
    print_status "Building OpenCV (this may take several minutes)..."
    make -j$(nproc)
    
    if [ $? -ne 0 ]; then
        print_error "OpenCV build failed."
        return 1
    fi
    
    print_status "Installing OpenCV..."
    make install
    
    if [ $? -ne 0 ]; then
        print_error "OpenCV installation failed."
        return 1
    fi
    
    cd ../..
    print_status "OpenCV built and installed successfully."
    return 0
}

# Function to build the main program
build_program() {
    print_status "Building LVGL VideoInput program..."
    
    cd Source || exit 1
    
    # Create build directory
    mkdir -p build
    cd build || exit 1
    
    print_status "Configuring build..."
    cmake ..
    
    if [ $? -ne 0 ]; then
        print_error "CMake configuration failed."
        return 1
    fi
    
    print_status "Building program..."
    make main
    
    if [ $? -ne 0 ]; then
        print_error "Build failed."
        return 1
    fi
    
    cd ../..
    print_status "Program built successfully."
    return 0
}

# Function to run the program
run_program() {
    print_status "Starting LVGL VideoInput program..."
    
    cd Source/build || exit 1
    
    if [ ! -f "main" ]; then
        print_error "Executable 'main' not found. Please build the program first."
        return 1
    fi
    
    print_status "Launching program..."
    echo ""
    print_warning "Press Ctrl+C to exit the program."
    echo ""
    
    ./main
    
    return $?
}

# Function to show help
show_help() {
    echo "Usage: $0 [OPTION]"
    echo ""
    echo "Options:"
    echo "  build     Build the program (check dependencies, build OpenCV, build program)"
    echo "  run       Run the program (must be built first)"
    echo "  clean     Clean build files"
    echo "  deps      Check system dependencies"
    echo "  opencv    Build OpenCV only"
    echo "  help      Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0 build    # Build everything"
    echo "  $0 run      # Run the program"
    echo "  $0          # Build and run (default)"
    echo ""
    echo "For detailed documentation, see:"
    echo "  - Source/QUICK_START.md"
    echo "  - Source/HOW_TO_USE.md"
    echo "  - README.md"
}

# Function to clean build files
clean_build() {
    print_status "Cleaning build files..."
    
    if [ -d "Source/build" ]; then
        rm -rf Source/build
        print_status "Build directory cleaned."
    fi
    
    if [ -d "Source/opencv_build" ]; then
        rm -rf Source/opencv_build
        print_status "OpenCV build directory cleaned."
    fi
    
    print_status "Clean completed."
}

# Main script logic
main() {
    print_header
    
    case "${1:-}" in
        "build")
            if ! check_dependencies; then
                exit 1
            fi
            if ! build_opencv; then
                exit 1
            fi
            if ! build_program; then
                exit 1
            fi
            print_status "Build completed successfully!"
            ;;
        "run")
            run_program
            ;;
        "clean")
            clean_build
            ;;
        "deps")
            check_dependencies
            ;;
        "opencv")
            build_opencv
            ;;
        "help"|"-h"|"--help")
            show_help
            ;;
        "")
            # Default: build and run
            if ! check_dependencies; then
                exit 1
            fi
            if ! build_opencv; then
                exit 1
            fi
            if ! build_program; then
                exit 1
            fi
            print_status "Build completed successfully!"
            echo ""
            run_program
            ;;
        *)
            print_error "Unknown option: $1"
            echo ""
            show_help
            exit 1
            ;;
    esac
}

# Run main function with all arguments
main "$@" 