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
    
    local missing_deps=()
    
    # Check for essential build tools
    if ! command -v gcc &> /dev/null; then
        missing_deps+=("gcc")
    fi
    
    if ! command -v g++ &> /dev/null; then
        missing_deps+=("g++")
    fi
    
    if ! command -v cmake &> /dev/null; then
        missing_deps+=("cmake")
    fi
    
    if ! command -v make &> /dev/null; then
        missing_deps+=("make")
    fi
    
    if ! command -v pkg-config &> /dev/null; then
        missing_deps+=("pkg-config")
    fi
    
    # Check for system libraries
    if ! pkg-config --exists openssl; then
        missing_deps+=("libssl-dev")
    fi
    
    # Check for FFmpeg core libraries
    local ffmpeg_missing=()
    if ! pkg-config --exists libavformat; then
        ffmpeg_missing+=("libavformat-dev")
    fi
    if ! pkg-config --exists libavcodec; then
        ffmpeg_missing+=("libavcodec-dev")
    fi
    if ! pkg-config --exists libavutil; then
        ffmpeg_missing+=("libavutil-dev")
    fi
    if ! pkg-config --exists libswscale; then
        ffmpeg_missing+=("libswscale-dev")
    fi
    if ! pkg-config --exists libswresample; then
        ffmpeg_missing+=("libswresample-dev")
    fi
    
    # Check for enhanced codec libraries
    if ! pkg-config --exists x264; then
        ffmpeg_missing+=("libx264-dev")
    fi
    if ! pkg-config --exists x265; then
        ffmpeg_missing+=("libx265-dev")
    fi
    if ! pkg-config --exists fdk-aac; then
        ffmpeg_missing+=("libfdk-aac-dev")
    fi
    
    # Add FFmpeg packages to missing dependencies
    if [ ${#ffmpeg_missing[@]} -gt 0 ]; then
        missing_deps+=("ffmpeg: ${ffmpeg_missing[*]}")
    fi
    
    if [ ${#missing_deps[@]} -gt 0 ]; then
        print_error "Missing system dependencies: ${missing_deps[*]}"
        echo ""
        print_status "Install dependencies manually using:"
        echo "  sudo apt update && sudo apt install -y build-essential cmake wget pkg-config libssl-dev ffmpeg libavformat-dev libavcodec-dev libavutil-dev libswscale-dev libswresample-dev libx264-dev libx265-dev libfdk-aac-dev"
        echo ""
        print_status "Or use the OpenSSL installation script:"
        echo "  cd Source"
        echo "  chmod +x install_openssl.sh"
        echo "  ./install_openssl.sh"
        echo ""
        exit 1
    fi
    
    print_success "All system dependencies are available"
    print_status "Note: All other dependencies (FreeType2, SDL2, MariaDB, LVGL) will be built from source. OpenSSL uses system library."
}

# Function to install system dependencies
install_system_dependencies() {
    print_status "Installing system dependencies..."
    
    cd Source
    
    # Check if install_dependencies.sh exists
    if [ -f "install_dependencies.sh" ]; then
        print_status "Using install_dependencies.sh script..."
        chmod +x install_dependencies.sh
        ./install_dependencies.sh || {
            print_error "System dependency installation failed"
            exit 1
        }
    else
        print_warning "install_dependencies.sh not found, using manual installation..."
        print_status "Installing essential build tools and libraries..."
        
        # Detect distribution and install dependencies
        if command -v apt &> /dev/null; then
            # Ubuntu/Debian
            sudo apt update
            sudo apt install -y \
                build-essential \
                cmake \
                wget \
                pkg-config \
                libssl-dev \
                ffmpeg \
                libavformat-dev \
                libavcodec-dev \
                libavutil-dev \
                libswscale-dev \
                libswresample-dev \
                libx264-dev \
                libx265-dev \
                libfdk-aac-dev \
                git \
                curl \
                unzip \
                libjpeg-dev \
                libpng-dev \
                libfreetype6-dev \
                libharfbuzz-dev \
                libsqlite3-dev \
                libmysqlclient-dev \
                libmariadb-dev \
                libmariadb-dev-compat
        elif command -v dnf &> /dev/null; then
            # Fedora/RHEL 8+
            sudo dnf update -y
            sudo dnf groupinstall -y "Development Tools"
            sudo dnf install -y \
                cmake \
                wget \
                pkgconfig \
                openssl-devel \
                ffmpeg-devel \
                git \
                curl \
                unzip \
                libjpeg-devel \
                libpng-devel \
                freetype-devel \
                harfbuzz-devel \
                sqlite-devel \
                mariadb-devel \
                mariadb-connector-c-devel
        elif command -v pacman &> /dev/null; then
            # Arch Linux
            sudo pacman -Syu --noconfirm
            sudo pacman -S --noconfirm \
                base-devel \
                cmake \
                wget \
                pkg-config \
                openssl \
                ffmpeg \
                git \
                curl \
                unzip \
                libjpeg-turbo \
                libpng \
                freetype2 \
                harfbuzz \
                sqlite \
                mariadb-libs \
                mariadb-dev
        else
            print_error "Unsupported package manager. Please install dependencies manually."
            print_status "Required packages: build-essential, cmake, wget, pkg-config, libssl-dev, ffmpeg, and FFmpeg development libraries"
            exit 1
        fi
        
        print_success "System dependencies installed successfully using package manager"
    fi
    
    cd ..
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

# Function to clean all (libraries and build artifacts)
clean_all() {
    print_status "Cleaning all artifacts (libraries and build)..."
    cd Source
    
    # Clean build directory
    if [ -d "build" ]; then
        rm -rf build
        print_success "Build artifacts cleaned"
    fi
    
    # Clean lib directory
    if [ -d "lib" ]; then
        rm -rf lib
        print_success "Library artifacts cleaned"
    fi
    
    cd ..
    print_success "All artifacts cleaned. Run './run.sh libs' to rebuild libraries."
}

# Function to check FFmpeg installation specifically
check_ffmpeg() {
    print_status "Checking FFmpeg installation..."
    
    local ffmpeg_status="✅"
    local missing_libs=()
    
    # Check core FFmpeg libraries
    if pkg-config --exists libavformat; then
        echo "  ${ffmpeg_status} libavformat"
    else
        missing_libs+=("libavformat-dev")
        ffmpeg_status="❌"
    fi
    
    if pkg-config --exists libavcodec; then
        echo "  ${ffmpeg_status} libavcodec"
    else
        missing_libs+=("libavcodec-dev")
        ffmpeg_status="❌"
    fi
    
    if pkg-config --exists libavutil; then
        echo "  ${ffmpeg_status} libavutil"
    else
        missing_libs+=("libavutil-dev")
        ffmpeg_status="❌"
    fi
    
    if pkg-config --exists libswscale; then
        echo "  ${ffmpeg_status} libswscale"
    else
        missing_libs+=("libswscale-dev")
        ffmpeg_status="❌"
    fi
    
    if pkg-config --exists libswresample; then
        echo "  ${ffmpeg_status} libswresample"
    else
        missing_libs+=("libswresample-dev")
        ffmpeg_status="❌"
    fi
    
    # Check enhanced codec libraries
    if pkg-config --exists x264; then
        echo "  ${ffmpeg_status} H.264 encoding (libx264-dev)"
    else
        missing_libs+=("libx264-dev")
        ffmpeg_status="❌"
    fi
    
    if pkg-config --exists x265; then
        echo "  ${ffmpeg_status} H.265 encoding (libx265-dev)"
    else
        missing_libs+=("libx265-dev")
        ffmpeg_status="❌"
    fi
    
    if pkg-config --exists fdk-aac; then
        echo "  ${ffmpeg_status} High-quality AAC (libfdk-aac-dev)"
    else
        missing_libs+=("libfdk-aac-dev")
        ffmpeg_status="❌"
    fi
    
    echo ""
    if [ ${#missing_libs[@]} -eq 0 ]; then
        print_success "FFmpeg installation complete with all codecs!"
    else
        print_warning "FFmpeg installation incomplete. Missing: ${missing_libs[*]}"
        print_status "Run './run.sh deps' to install missing packages."
    fi
    echo ""
}

# Function to check project setup
check_setup() {
    print_status "Checking project setup..."
    
    cd Source
    
    if [ -f "check_setup.sh" ]; then
        chmod +x check_setup.sh
        ./check_setup.sh
    else
        print_warning "check_setup.sh not found, running basic checks..."
        
        if [ -d "lib" ] && [ -d "lib/include" ] && [ -d "lib/lib" ]; then
            print_success "Basic project structure looks good"
        else
            print_error "Project structure incomplete. Run './run.sh libs' to build libraries."
            exit 1
        fi
    fi
    
    cd ..
}

# Function to show help
show_help() {
    echo "LVGL Face Recognition Application Runner"
    echo ""
    echo "Usage: $0 [OPTION]"
    echo ""
    echo "Options:"
    echo "  run, -r, --run              Build all libraries, application and run (default)"
    echo "  build, -b, --build          Build all libraries and application only"
    echo "  libs, -l, --libs            Build all libraries only"
    echo "  deps, -d, --deps            Install system dependencies only"
    echo "  check, --check              Check project setup"
    echo "  ffmpeg, --ffmpeg            Check FFmpeg installation specifically"
    echo "  clean, -c, --clean          Clean build artifacts"
    echo "  clean-all, --clean-all      Clean all artifacts (libraries + build)"
    echo "  help, -h, --help            Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0                 # Build everything and run"
    echo "  $0 deps           # Install system dependencies"
    echo "  $0 libs           # Build libraries only"
    echo "  $0 build          # Build libraries and application"
    echo "  $0 check          # Check project setup"
    echo "  $0 ffmpeg         # Check FFmpeg installation"
    echo "  $0 clean          # Clean build artifacts"
    echo "  $0 clean-all      # Clean everything"
    echo ""
    echo "Features:"
    echo "  - Automatic system dependency installation (multi-distro support)"
    echo "  - Comprehensive FFmpeg library checking"
    echo "  - Automatic library building from source"
    echo "  - Source cleanup after successful builds"
    echo "  - Database operations with MariaDB"
    echo "  - Web interface with HTTPS support"
    echo "  - Korean input methods"
    echo "  - Video playback with FFmpeg (H.264, H.265, AAC)"
    echo ""
    echo "First time setup:"
    echo "  1. $0 deps        # Install system dependencies (automatic detection)"
    echo "  2. $0 libs        # Build all libraries"
    echo "  3. $0             # Build and run everything"
    echo ""
    echo "Dependency installation:"
    echo "  - Automatic: $0 deps (detects package manager and installs)"
    echo "  - Manual: Use your system's package manager"
    echo "  - OpenSSL: cd Source && ./install_openssl.sh"
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
        "deps"|"-d"|"--deps")
            install_system_dependencies
            ;;
        "check"|"--check")
            check_setup
            ;;
        "ffmpeg"|"--ffmpeg")
            check_ffmpeg
            ;;
        "clean"|"-c"|"--clean")
            clean_build
            ;;
        "clean-all"|"--clean-all")
            clean_all
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
