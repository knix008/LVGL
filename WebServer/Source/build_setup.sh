#!/bin/bash

# ============================================================================
# LVGL WebServer - Complete Build Setup Script
# ============================================================================
# This script performs a complete build setup including:
# 1. Detecting system architecture
# 2. Checking and installing dependencies
# 3. Creating build directory
# 4. Running CMake configuration
# 5. Showing build instructions
#
# Usage: ./build_setup.sh [--help] [--skip-deps] [--clean]
# ============================================================================

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"
SKIP_DEPS=false
CLEAN_BUILD=false

# ============================================================================
# FUNCTIONS
# ============================================================================

print_header() {
    echo ""
    echo -e "${BLUE}════════════════════════════════════════════${NC}"
    echo -e "${BLUE}  $1${NC}"
    echo -e "${BLUE}════════════════════════════════════════════${NC}"
}

print_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

print_error() {
    echo -e "${RED}✗ $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠ $1${NC}"
}

print_info() {
    echo -e "${BLUE}ℹ $1${NC}"
}

show_help() {
    cat << EOF
${BLUE}LVGL WebServer - Build Setup Script${NC}

${GREEN}Usage:${NC}
    ./build_setup.sh [OPTIONS]

${GREEN}Options:${NC}
    --help          Show this help message and exit
    --skip-deps     Skip dependency installation check
    --clean         Clean build directory before building

${GREEN}Examples:${NC}
    # Standard setup (recommended for first time)
    ./build_setup.sh

    # Setup without dependency check
    ./build_setup.sh --skip-deps

    # Clean rebuild
    ./build_setup.sh --clean

${GREEN}What This Script Does:${NC}
    1. Detects system architecture and OS
    2. Checks for required dependencies
    3. Installs missing dependencies (with confirmation)
    4. Creates/cleans build directory
    5. Runs CMake configuration
    6. Shows build instructions

${GREEN}After Setup:${NC}
    Run: cd build && make

${BLUE}System Support:${NC}
    • Ubuntu/Debian and derivatives
    • Fedora/RHEL/CentOS
    • Alpine Linux
    • Arch Linux / Manjaro
    • macOS with Homebrew

EOF
}

detect_system() {
    # Detect OS
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        OS="linux"
        if [ -f /etc/os-release ]; then
            . /etc/os-release
            DISTRO=$ID
        else
            DISTRO="unknown"
        fi
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        OS="macos"
        DISTRO="macos"
    else
        OS="unknown"
        DISTRO="unknown"
    fi

    # Detect Architecture
    ARCH=$(uname -m)
    case "$ARCH" in
        aarch64|arm64)
            ARCH_TYPE="aarch64"
            ARCH_DESC="ARM64 (64-bit)"
            ;;
        x86_64|AMD64)
            ARCH_TYPE="x86_64"
            ARCH_DESC="x86-64 (64-bit)"
            ;;
        arm|armv7l)
            ARCH_TYPE="arm"
            ARCH_DESC="ARM 32-bit"
            ;;
        *)
            ARCH_TYPE="unknown"
            ARCH_DESC="Unknown"
            ;;
    esac
}

check_command_exists() {
    if command -v "$1" &> /dev/null; then
        return 0
    else
        return 1
    fi
}

print_system_info() {
    print_header "System Information"
    print_info "Operating System: $OS"
    print_info "Distribution: $DISTRO"
    print_info "Architecture: $ARCH_DESC"
    print_info "Build Directory: $BUILD_DIR"
}

check_cmake() {
    print_header "Checking CMake Installation"

    if ! check_command_exists cmake; then
        print_error "CMake not found!"
        print_warning "This is required for building"
        return 1
    fi

    CMAKE_VERSION=$(cmake --version | head -n1)
    print_success "$CMAKE_VERSION"
    return 0
}

check_compilers() {
    print_header "Checking C/C++ Compilers"

    local all_found=true

    if check_command_exists gcc; then
        GCC_VERSION=$(gcc --version | head -n1)
        print_success "GCC: $GCC_VERSION"
    else
        print_error "GCC not found"
        all_found=false
    fi

    if check_command_exists g++; then
        GPP_VERSION=$(g++ --version | head -n1)
        print_success "G++: $GPP_VERSION"
    else
        print_error "G++ not found"
        all_found=false
    fi

    if [ "$all_found" = false ]; then
        print_warning "C/C++ compilers are required for building"
        return 1
    fi

    return 0
}

check_dependencies() {
    print_header "Checking Dependencies"

    local missing=false

    # Check pkg-config
    if ! check_command_exists pkg-config; then
        print_error "pkg-config not found"
        missing=true
    else
        print_success "pkg-config found"
    fi

    # Check libraries
    if pkg-config --exists sdl2 2>/dev/null; then
        print_success "SDL2 found"
    else
        print_warning "SDL2 not found (libsdl2-dev)"
        missing=true
    fi

    if pkg-config --exists freetype2 2>/dev/null; then
        print_success "FreeType2 found"
    else
        print_warning "FreeType2 not found (libfreetype6-dev)"
        missing=true
    fi

    if pkg-config --exists openssl 2>/dev/null; then
        print_success "OpenSSL found"
    else
        print_warning "OpenSSL not found (libssl-dev)"
        missing=true
    fi

    if pkg-config --exists libavformat libavcodec libavutil libswscale 2>/dev/null; then
        print_success "FFmpeg found"
    else
        print_warning "FFmpeg not found (libavformat-dev, etc.)"
        missing=true
    fi

    if [ "$missing" = true ]; then
        return 1
    fi

    return 0
}

install_dependencies() {
    print_header "Installing Missing Dependencies"

    if [ ! -f "$SCRIPT_DIR/install_dependencies.sh" ]; then
        print_error "install_dependencies.sh not found"
        return 1
    fi

    print_info "Running dependency installer..."
    bash "$SCRIPT_DIR/install_dependencies.sh"

    return $?
}

prepare_build_directory() {
    print_header "Preparing Build Directory"

    if [ -d "$BUILD_DIR" ]; then
        if [ "$CLEAN_BUILD" = true ]; then
            print_info "Cleaning existing build directory..."
            rm -rf "$BUILD_DIR"
            print_success "Build directory cleaned"
        else
            print_info "Build directory already exists: $BUILD_DIR"
        fi
    fi

    if [ ! -d "$BUILD_DIR" ]; then
        print_info "Creating build directory..."
        mkdir -p "$BUILD_DIR"
        print_success "Build directory created: $BUILD_DIR"
    fi
}

run_cmake() {
    print_header "Running CMake Configuration"

    cd "$BUILD_DIR"

    print_info "Current directory: $(pwd)"
    print_info "Configuring with CMake..."
    echo ""

    if cmake ..; then
        print_success "CMake configuration successful"
        cd "$SCRIPT_DIR"
        return 0
    else
        print_error "CMake configuration failed"
        cd "$SCRIPT_DIR"
        return 1
    fi
}

show_build_instructions() {
    print_header "Build Instructions"

    cat << EOF

${GREEN}To build the project:${NC}

    cd $BUILD_DIR
    make

${GREEN}Build options:${NC}

    # Build all targets
    make

    # Build only main application
    make main

    # Build with verbose output
    make VERBOSE=1

    # Build specific test
    make test_chunjiin
    make calendar_test
    make clock_test

${GREEN}After successful build:${NC}

    # Run the application
    ./main

    # The application will:
    • Open an 800x600 SDL window
    • Initialize LVGL GUI with tabs
    • Start web server on http://localhost:8080
    • Press Ctrl+C to exit

${GREEN}Project Information:${NC}

    • Architecture: $ARCH_DESC
    • Build Type: $(cmake -LA "$BUILD_DIR" 2>/dev/null | grep CMAKE_BUILD_TYPE | cut -d'=' -f2 || echo "Debug")
    • Installation: $SCRIPT_DIR

${BLUE}For more information, see README.md${NC}

EOF
}

# ============================================================================
# MAIN SCRIPT
# ============================================================================

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --help)
            show_help
            exit 0
            ;;
        --skip-deps)
            SKIP_DEPS=true
            shift
            ;;
        --clean)
            CLEAN_BUILD=true
            shift
            ;;
        *)
            print_error "Unknown option: $1"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

# Main execution
clear
print_header "LVGL WebServer - Build Setup"

# Detect system
detect_system
print_system_info

# Check basic requirements
if ! check_cmake; then
    print_error "CMake is required. Please install it and try again."
    exit 1
fi

if ! check_compilers; then
    print_error "C/C++ compilers are required. Please install build-essential and try again."
    exit 1
fi

# Check dependencies
if [ "$SKIP_DEPS" = false ]; then
    if ! check_dependencies; then
        echo ""
        read -p "$(echo -e ${YELLOW}Install missing dependencies?${NC} (y/n) ) " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            if ! install_dependencies; then
                print_error "Dependency installation failed"
                print_warning "Please install dependencies manually and try again"
                exit 1
            fi
        else
            print_warning "Skipping dependency installation"
            print_warning "Build may fail due to missing dependencies"
        fi
    else
        print_success "All dependencies are installed!"
    fi
else
    print_info "Dependency check skipped (--skip-deps)"
fi

# Prepare build directory
prepare_build_directory

# Run CMake
if ! run_cmake; then
    print_error "CMake configuration failed"
    print_info "Check the errors above and try again"
    exit 1
fi

# Show build instructions
show_build_instructions

print_success "Setup completed successfully!"
echo ""
