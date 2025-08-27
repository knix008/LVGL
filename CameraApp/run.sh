#!/bin/bash

# Webcam Application Runner
# This script builds and runs the webcam application with AI face detection

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
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

print_highlight() {
    echo -e "${PURPLE}[HIGHLIGHT]${NC} $1"
}

print_webcam() {
    echo -e "${CYAN}[WEBCAM]${NC} $1"
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
    
    if ! command -v pkg-config &> /dev/null; then
        print_error "pkg-config is not installed. Please install pkg-config first."
        exit 1
    fi
    
    # Check for OpenCV
    if ! pkg-config --exists opencv4; then
        print_error "OpenCV is not installed. Webcam functionality requires OpenCV."
        print_error "Install with: sudo apt-get install libopencv-dev"
        exit 1
    else
        print_success "OpenCV found"
    fi
    
    # Check for camera access
    if [ ! -e "/dev/video0" ] && [ ! -e "/dev/video1" ]; then
        print_warning "No camera devices found. Application will run in simulation mode."
    else
        print_success "Camera devices found"
    fi
    
    print_success "All dependencies are available"
}

# Function to build the webcam application
build_webcam() {
    print_status "Building webcam application..."
    
    cd Source
    
    # Use the new build script if available
    if [ -f "build_webcam.sh" ]; then
        print_status "Using build_webcam.sh script..."
        chmod +x build_webcam.sh
        ./build_webcam.sh build "${1:-Release}" || {
            print_error "Webcam build failed"
            exit 1
        }
    else
        # Fallback to direct CMake build
        print_status "Using direct CMake build..."
        mkdir -p build
        cd build
        cmake -DCMAKE_BUILD_TYPE="${1:-Release}" .. || {
            print_error "CMake configuration failed"
            exit 1
        }
        make -j$(nproc) || {
            print_error "Build failed"
            exit 1
        }
        cd ..
    fi
    
    print_success "Webcam application built successfully"
    cd ..
}

# Function to run the webcam application
run_webcam() {
    print_webcam "Starting webcam application with AI face detection..."
    
    cd Source/build
    
    # Check if the executable exists
    if [ ! -f "webcam_app" ]; then
        print_error "Executable 'webcam_app' not found. Please build the application first."
        exit 1
    fi
    
    # Check if model exists
    if [ ! -f "models/yolov8n-face.onnx" ]; then
        print_error "YOLOv8 face detection model not found."
        print_error "Expected: models/yolov8n-face.onnx"
        exit 1
    fi
    
    print_highlight "=== Webcam Application Starting ==="
    print_webcam "Model: YOLOv8 Face Detection"
    print_webcam "Camera: Auto-detection (0, 2, 3, 1)"
    print_webcam "Processing: Real-time AI inference"
    print_webcam "Controls: Press ESC or Q to stop"
    print_highlight "====================================="
    echo ""
    
    # Run the webcam application
    ./webcam_app --model models/yolov8n-face.onnx
}

# Function to run webcam in background
run_webcam_background() {
    print_webcam "Starting webcam application in background..."
    
    cd Source/build
    
    # Check if the executable exists
    if [ ! -f "webcam_app" ]; then
        print_error "Executable 'webcam_app' not found. Please build the application first."
        exit 1
    fi
    
    # Run in background
    nohup ./webcam_app --model models/yolov8n-face.onnx > webcam.log 2>&1 &
    local pid=$!
    
    print_success "Webcam application started in background (PID: $pid)"
    print_status "Log file: Source/build/webcam.log"
    print_status "To stop: kill $pid"
    
    echo $pid > webcam.pid
}

# Function to stop background webcam
stop_webcam() {
    print_webcam "Stopping background webcam application..."
    
    cd Source/build
    
    if [ -f "webcam.pid" ]; then
        local pid=$(cat webcam.pid)
        if kill -0 $pid 2>/dev/null; then
            kill $pid
            print_success "Webcam application stopped (PID: $pid)"
        else
            print_warning "Webcam application was not running"
        fi
        rm -f webcam.pid
    else
        print_warning "No webcam PID file found"
    fi
}

# Function to show webcam status
show_webcam_status() {
    print_webcam "Checking webcam application status..."
    
    cd Source/build
    
    if [ -f "webcam.pid" ]; then
        local pid=$(cat webcam.pid)
        if kill -0 $pid 2>/dev/null; then
            print_success "Webcam application is running (PID: $pid)"
            print_status "Log file: Source/build/webcam.log"
        else
            print_warning "Webcam application is not running (stale PID file)"
            rm -f webcam.pid
        fi
    else
        print_warning "Webcam application is not running"
    fi
}

# Function to test webcam
test_webcam() {
    print_webcam "Testing webcam application..."
    
    cd Source
    
    if [ -f "build_webcam.sh" ]; then
        ./build_webcam.sh test
    else
        cd build
        make test_webcam
        cd ..
    fi
}

# Function to clean build artifacts
clean_build() {
    print_status "Cleaning build artifacts..."
    cd Source
    rm -rf build
    print_success "Build artifacts cleaned"
    cd ..
}

# Function to show system information
show_system_info() {
    print_highlight "=== System Information ==="
    print_status "OS: $(uname -s) $(uname -r)"
    print_status "Architecture: $(uname -m)"
    print_status "OpenCV Version: $(pkg-config --modversion opencv4 2>/dev/null || echo 'Not found')"
    print_status "Camera Devices:"
    ls -la /dev/video* 2>/dev/null || print_warning "No camera devices found"
    print_highlight "=========================="
}

# Function to show help
show_help() {
    echo "Webcam Application Runner"
    echo ""
    echo "Usage: $0 [OPTION]"
    echo ""
    echo "Options:"
    echo "  run, -r, --run              Build and run webcam application (default)"
    echo "  build, -b, --build [type]   Build webcam application only"
    echo "  test, -t, --test            Test webcam application"
    echo "  background, -bg, --bg       Run webcam in background"
    echo "  stop, -s, --stop            Stop background webcam"
    echo "  status, -st, --status       Show webcam status"
    echo "  clean, -c, --clean          Clean build artifacts"
    echo "  info, -i, --info            Show system information"
    echo "  help, -h, --help            Show this help message"
    echo ""
    echo "Build types:"
    echo "  Release                     Optimized release build (default)"
    echo "  Debug                       Debug build with symbols"
    echo ""
    echo "Examples:"
    echo "  $0                          # Build and run webcam"
    echo "  $0 build                    # Build only (Release)"
    echo "  $0 build Debug              # Build with Debug configuration"
    echo "  $0 test                     # Test webcam application"
    echo "  $0 background               # Run webcam in background"
    echo "  $0 stop                     # Stop background webcam"
    echo "  $0 status                   # Show webcam status"
    echo "  $0 clean                    # Clean build artifacts"
    echo "  $0 info                     # Show system information"
    echo ""
    echo "Features:"
    echo "  • Real-time AI face detection using YOLOv8"
    echo "  • Automatic camera detection"
    echo "  • High-performance inference with ONNX Runtime"
    echo "  • Background processing support"
    echo "  • Comprehensive logging and monitoring"
    echo ""
}

# Main script logic
main() {
    case "${1:-run}" in
        "run"|"-r"|"--run")
            check_dependencies
            build_webcam
            run_webcam
            ;;
        "build"|"-b"|"--build")
            check_dependencies
            build_webcam "${2:-Release}"
            ;;
        "test"|"-t"|"--test")
            check_dependencies
            test_webcam
            ;;
        "background"|"-bg"|"--bg")
            check_dependencies
            build_webcam
            run_webcam_background
            ;;
        "stop"|"-s"|"--stop")
            stop_webcam
            ;;
        "status"|"-st"|"--status")
            show_webcam_status
            ;;
        "clean"|"-c"|"--clean")
            clean_build
            ;;
        "info"|"-i"|"--info")
            show_system_info
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
