#!/bin/bash

# Webcam Application Build Script
# This script builds the webcam application with proper configuration
# Updated for ONNX Runtime 1.16.3

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
    
    # Check for local OpenCV installation
    if [ ! -d "opencv" ]; then
        print_warning "OpenCV directory not found."
        print_status "Building OpenCV locally..."
        if [ -f "build_opencv.sh" ]; then
            chmod +x build_opencv.sh
            ./build_opencv.sh
        else
            print_error "build_opencv.sh not found. Please build OpenCV manually."
            exit 1
        fi
    fi
    
    if [ ! -f "opencv/lib/cmake/opencv4/OpenCVConfig.cmake" ]; then
        print_error "OpenCV CMake configuration not found. Please rebuild OpenCV."
        exit 1
    fi
    
    # Check for ONNX Runtime installation
    if [ ! -d "onnxruntime-linux-x64-1.16.3" ]; then
        print_warning "ONNX Runtime 1.16.3 not found."
        print_status "Installing ONNX Runtime..."
        if [ -f "install_onnxruntime.sh" ]; then
            chmod +x install_onnxruntime.sh
            ./install_onnxruntime.sh
        else
            print_error "install_onnxruntime.sh not found. Please install ONNX Runtime manually."
            exit 1
        fi
    fi
    
    if [ ! -f "onnxruntime-linux-x64-1.16.3/lib/libonnxruntime.so" ]; then
        print_error "ONNX Runtime library not found. Please run ./install_onnxruntime.sh"
        exit 1
    fi
    
    if [ ! -f "onnxruntime-linux-x64-1.16.3/include/onnxruntime_c_api.h" ]; then
        print_error "ONNX Runtime headers not found. Please run ./install_onnxruntime.sh"
        exit 1
    fi
    
    # Check for model files
    if [ ! -f "models/yolov8n-face.onnx" ]; then
        print_warning "YOLOv8 face detection model not found in models/yolov8n-face.onnx"
        print_status "Please ensure the model file is available for face detection to work."
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

# Function to run the application
run_application() {
    print_status "Running webcam application..."
    cd build
    make run-webcam
}

# Function to install
install_app() {
    print_status "Installing webcam application..."
    cd build
    sudo make install
    print_success "Installation completed"
}

# Function to show build information
show_info() {
    print_status "Build information:"
    cd build
    make info
}

# Function to show help
show_help() {
    echo "Usage: $0 [OPTION]"
    echo ""
    echo "Options:"
    echo "  build [type]    Build the application (default: Release)"
    echo "  clean           Clean build artifacts"
    echo "  test            Run tests"
    echo "  run             Run the application"
    echo "  install         Install the application"
    echo "  info            Show build information"
    echo "  all             Build, test, and run"
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
    echo "  $0 run          # Run the application"
    echo "  $0 install      # Install the application"
    echo "  $0 info         # Show build information"
    echo ""
    echo "Dependencies:"
    echo "  - OpenCV (auto-built locally with self-contained libraries)"
    echo "  - ONNX Runtime 1.16.3 (auto-installed if missing)"
    echo "  - YOLOv8 face detection model (models/yolov8n-face.onnx)"
    echo ""
    echo "System Dependencies (auto-installed):"
    echo "  - libgstreamer1.0-dev"
    echo "  - libgstreamer-plugins-base1.0-dev"
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
        "run")
            if [ ! -d "build" ]; then
                print_error "Build directory not found. Run 'build' first."
                exit 1
            fi
            run_application
            ;;
        "install")
            if [ ! -d "build" ]; then
                print_error "Build directory not found. Run 'build' first."
                exit 1
            fi
            install_app
            ;;
        "info")
            if [ ! -d "build" ]; then
                print_error "Build directory not found. Run 'build' first."
                exit 1
            fi
            show_info
            ;;
        "all")
            check_dependencies
            build_application Release
            run_tests
            print_status "Build and tests completed. Run '$0 run' to start the application."
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
