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

# Function to detect package manager and install dependencies
install_system_dependencies() {
    print_status "Installing missing system dependencies..."
    
    # Detect package manager
    local pkg_manager=""
    local install_cmd=""
    local update_cmd=""
    
    if command -v apt &> /dev/null; then
        pkg_manager="apt"
        update_cmd="sudo apt update"
        install_cmd="sudo apt install -y"
    elif command -v dnf &> /dev/null; then
        pkg_manager="dnf"
        update_cmd="sudo dnf check-update || true"  # Don't fail if no updates
        install_cmd="sudo dnf install -y"
    elif command -v yum &> /dev/null; then
        pkg_manager="yum"
        update_cmd="sudo yum check-update || true"  # Don't fail if no updates
        install_cmd="sudo yum install -y"
    elif command -v pacman &> /dev/null; then
        pkg_manager="pacman"
        update_cmd="sudo pacman -Sy"
        install_cmd="sudo pacman -S --noconfirm"
    elif command -v apk &> /dev/null; then
        pkg_manager="apk"
        update_cmd="sudo apk update"
        install_cmd="sudo apk add --no-cache"
    else
        print_error "No supported package manager found (apt, dnf, yum, pacman, apk)"
        return 1
    fi
    
    print_status "Using package manager: $pkg_manager"
    
    # Define packages to install based on package manager
    local build_packages=""
    local ffmpeg_packages=""
    local openssl_packages=""
    
    case $pkg_manager in
        "apt")
            build_packages="build-essential cmake wget pkg-config"
            ffmpeg_packages="libavformat-dev libavcodec-dev libavutil-dev libswscale-dev libswresample-dev"
            openssl_packages="libssl-dev"
            ;;
        "dnf"|"yum")
            build_packages="gcc gcc-c++ cmake wget pkgconfig"
            ffmpeg_packages="ffmpeg-devel"
            openssl_packages="openssl-devel"
            ;;
        "pacman")
            build_packages="base-devel cmake wget pkgconf"
            ffmpeg_packages="ffmpeg"
            openssl_packages="openssl"
            ;;
        "apk")
            build_packages="build-base cmake wget pkgconf"
            ffmpeg_packages="ffmpeg-dev"
            openssl_packages="openssl-dev"
            ;;
    esac
    
    # Update package lists
    print_status "Updating package lists..."
    if ! $update_cmd; then
        print_warning "Package list update failed, continuing with installation..."
    fi
    
    # Install build tools
    if [ -n "$build_packages" ]; then
        print_status "Installing build tools: $build_packages"
        if ! $install_cmd $build_packages; then
            print_error "Failed to install build tools"
            return 1
        fi
    fi
    
    # Install FFmpeg packages
    if [ -n "$ffmpeg_packages" ]; then
        print_status "Installing FFmpeg packages: $ffmpeg_packages"
        if ! $install_cmd $ffmpeg_packages; then
            print_error "Failed to install FFmpeg packages"
            return 1
        fi
    fi
    
    # Install OpenSSL packages
    if [ -n "$openssl_packages" ]; then
        print_status "Installing OpenSSL packages: $openssl_packages"
        if ! $install_cmd $openssl_packages; then
            print_error "Failed to install OpenSSL packages"
            return 1
        fi
    fi
    
    print_success "System dependencies installed successfully"
}

# Function to check if required tools are available
check_dependencies() {
    print_status "Checking system dependencies..."
    
    local missing_deps=()
    local need_install=false
    
    # Check for essential build tools
    if ! command -v gcc &> /dev/null; then
        missing_deps+=("gcc")
        need_install=true
    fi
    
    if ! command -v g++ &> /dev/null; then
        missing_deps+=("g++")
        need_install=true
    fi
    
    if ! command -v cmake &> /dev/null; then
        missing_deps+=("cmake")
        need_install=true
    fi
    
    if ! command -v make &> /dev/null; then
        missing_deps+=("make")
        need_install=true
    fi
    
    if ! command -v pkg-config &> /dev/null; then
        missing_deps+=("pkg-config")
        need_install=true
    fi
    
    # Check for system libraries
    local openssl_found=false
    
    # Method 1: Check pkg-config
    if pkg-config --exists openssl; then
        openssl_found=true
    fi
    
    # Method 2: Check if OpenSSL headers exist
    if [ -f "/usr/include/openssl/ssl.h" ] || [ -f "/usr/local/include/openssl/ssl.h" ]; then
        openssl_found=true
    fi
    
    # Method 3: Check if libssl-dev package is installed (Ubuntu/Debian)
    if command -v dpkg &> /dev/null; then
        if dpkg -l | grep -q "libssl-dev"; then
            openssl_found=true
        fi
    fi
    
    # Method 4: Check if openssl-devel package is installed (RHEL/CentOS)
    if command -v rpm &> /dev/null; then
        if rpm -qa | grep -q "openssl-devel"; then
            openssl_found=true
        fi
    fi
    
    if [ "$openssl_found" = false ]; then
        missing_deps+=("libssl-dev")
        need_install=true
    fi
    
    # Check for essential FFmpeg libraries only
    local ffmpeg_missing=()
    local ffmpeg_found=false
    
    # Check if essential FFmpeg libraries are available via pkg-config
    if pkg-config --exists libavformat libavcodec libavutil libswscale libswresample; then
        ffmpeg_found=true
    fi
    
    # Alternative check: look for FFmpeg headers
    if [ -f "/usr/include/libavformat/avformat.h" ] || [ -f "/usr/local/include/libavformat/avformat.h" ]; then
        ffmpeg_found=true
    fi
    
    # Check if FFmpeg packages are installed (Ubuntu/Debian)
    if command -v dpkg &> /dev/null; then
        if dpkg -l | grep -q "libavformat-dev" && dpkg -l | grep -q "libavcodec-dev"; then
            ffmpeg_found=true
        fi
    fi
    
    # Check if FFmpeg packages are installed (RHEL/CentOS)
    if command -v rpm &> /dev/null; then
        if rpm -qa | grep -q "ffmpeg-devel"; then
            ffmpeg_found=true
        fi
    fi
    
    if [ "$ffmpeg_found" = false ]; then
        ffmpeg_missing+=("libavformat-dev" "libavcodec-dev" "libavutil-dev" "libswscale-dev" "libswresample-dev")
        need_install=true
    fi
    
    # Add FFmpeg packages to missing dependencies
    if [ ${#ffmpeg_missing[@]} -gt 0 ]; then
        missing_deps+=("ffmpeg: ${ffmpeg_missing[*]}")
    fi
    
    # If dependencies are missing, try to install them automatically
    if [ "$need_install" = true ]; then
        print_warning "Missing system dependencies: ${missing_deps[*]}"
        echo ""
        print_status "Attempting to install missing dependencies automatically..."
        
        if install_system_dependencies; then
            print_success "Dependencies installed successfully, re-checking..."
            # Re-check dependencies after installation
            check_dependencies
            return
        else
            print_error "Automatic installation failed. Please install dependencies manually:"
            echo ""
            print_status "Ubuntu/Debian:"
            echo "  sudo apt update && sudo apt install -y build-essential cmake wget pkg-config libssl-dev libavformat-dev libavcodec-dev libavutil-dev libswscale-dev libswresample-dev"
            echo ""
            print_status "CentOS/RHEL/Fedora:"
            echo "  sudo dnf install -y gcc gcc-c++ cmake wget pkgconfig openssl-devel ffmpeg-devel"
            echo ""
            print_status "Arch Linux:"
            echo "  sudo pacman -S --noconfirm base-devel cmake wget pkgconf openssl ffmpeg"
            echo ""
            print_status "Alpine Linux:"
            echo "  sudo apk add --no-cache build-base cmake wget pkgconf openssl-dev ffmpeg-dev"
            echo ""
            exit 1
        fi
    fi
    
    print_success "All system dependencies are available"
    print_status "Note: All other dependencies (FreeType2, SDL2, MariaDB, LVGL, zlib) will be built from source."
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

# Function to debug dependency checking
debug_dependencies() {
    print_status "Debug: Dependency checking information..."
    echo ""
    
    echo "=== System Information ==="
    echo "Distribution: $(lsb_release -d 2>/dev/null | cut -f2 || echo 'Unknown')"
    echo "Architecture: $(uname -m)"
    echo "Package manager: $(command -v apt && echo 'apt' || command -v dnf && echo 'dnf' || command -v yum && echo 'yum' || command -v pacman && echo 'pacman' || echo 'Unknown')"
    echo ""
    
    echo "=== OpenSSL Detection ==="
    echo "pkg-config openssl: $(pkg-config --exists openssl && echo 'Found' || echo 'Not found')"
    echo "Header file /usr/include/openssl/ssl.h: $(test -f /usr/include/openssl/ssl.h && echo 'Exists' || echo 'Missing')"
    echo "Header file /usr/local/include/openssl/ssl.h: $(test -f /usr/local/include/openssl/ssl.h && echo 'Exists' || echo 'Missing')"
    
    if command -v dpkg &> /dev/null; then
        echo "dpkg libssl-dev: $(dpkg -l | grep -q "libssl-dev" && echo 'Installed' || echo 'Not installed')"
    fi
    
    if command -v rpm &> /dev/null; then
        echo "rpm openssl-devel: $(rpm -qa | grep -q "openssl-devel" && echo 'Installed' || echo 'Not installed')"
    fi
    echo ""
    
    echo "=== FFmpeg Detection ==="
    echo "pkg-config FFmpeg: $(pkg-config --exists libavformat libavcodec libavutil libswscale libswresample && echo 'Found' || echo 'Not found')"
    echo "Header /usr/include/libavformat/avformat.h: $(test -f /usr/include/libavformat/avformat.h && echo 'Exists' || echo 'Missing')"
    
    if command -v dpkg &> /dev/null; then
        echo "dpkg FFmpeg dev packages:"
        dpkg -l | grep -E "(libavformat-dev|libavcodec-dev|libavutil-dev|libswscale-dev|libswresample-dev)" || echo "  None found"
    fi
    
    if command -v rpm &> /dev/null; then
        echo "rpm FFmpeg devel: $(rpm -qa | grep -q "ffmpeg-devel" && echo 'Installed' || echo 'Not installed')"
    fi
    echo ""
    
    echo "=== Build Tools ==="
    echo "gcc: $(command -v gcc && echo 'Found' || echo 'Missing')"
    echo "g++: $(command -v g++ && echo 'Found' || echo 'Missing')"
    echo "cmake: $(command -v cmake && echo 'Found' || echo 'Missing')"
    echo "make: $(command -v make && echo 'Found' || echo 'Missing')"
    echo "pkg-config: $(command -v pkg-config && echo 'Found' || echo 'Missing')"
    echo ""
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
    

    
    echo ""
    if [ ${#missing_libs[@]} -eq 0 ]; then
        print_success "FFmpeg installation complete with essential libraries!"
    else
        print_warning "FFmpeg installation incomplete. Missing: ${missing_libs[*]}"
        print_status "Install missing packages manually or check README_SETUP.md"
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
    echo "  mariadb-server, --mariadb-server  Build MariaDB Server from source"
    echo "  check, --check              Check project setup"
    echo "  ffmpeg, --ffmpeg            Check FFmpeg installation specifically"
    echo "  debug, --debug              Debug dependency checking"
    echo "  clean, -c, --clean          Clean build artifacts"
    echo "  clean-all, --clean-all      Clean all artifacts (libraries + build)"
    echo "  help, -h, --help            Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0                 # Build everything and run"
    echo "  $0 libs           # Build libraries only"
    echo "  $0 mariadb-server # Build MariaDB Server from source"
    echo "  $0 build          # Build libraries and application"
    echo "  $0 check          # Check project setup"
    echo "  $0 ffmpeg         # Check FFmpeg installation"
    echo "  $0 debug          # Debug dependency checking"
    echo "  $0 clean          # Clean build artifacts"
    echo "  $0 clean-all      # Clean everything"
    echo ""
    echo "Features:"
    echo "  - Comprehensive FFmpeg library checking"
    echo "  - Automatic library building from source"
    echo "  - Source cleanup after successful builds"
    echo "  - Database operations with MariaDB"
    echo "  - Web interface with HTTPS support"
    echo "  - Korean input methods"
    echo "  - Video playback with FFmpeg (H.264, H.265, AAC)"
    echo ""
    echo "First time setup:"
    echo "  1. $0 libs        # Build all libraries (system deps auto-installed)"
    echo "  2. $0             # Build and run everything"
    echo ""
    echo "Automatic dependency installation:"
    echo "  - System dependencies are automatically installed when missing"
    echo "  - Supports: Ubuntu/Debian, CentOS/RHEL/Fedora, Arch Linux, Alpine Linux"
    echo "  - Libraries built from source: FreeType2, SDL2, LVGL, MariaDB, zlib"
    echo "  - System libraries installed: build tools, FFmpeg, OpenSSL"
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
        "mariadb-server"|"--mariadb-server")
            check_dependencies
            print_status "Building MariaDB Server from source..."
            cd Source
            ./build_mariadb_server.sh
            cd ..
            ;;
        "check"|"--check")
            check_setup
            ;;
        "ffmpeg"|"--ffmpeg")
            check_ffmpeg
            ;;
        "debug"|"--debug")
            debug_dependencies
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
