#!/bin/bash

# Build script for KorEngNumQWerty LVGL Application
# This script checks for dependencies and builds the application

set -e  # Exit on error

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Print functions
print_info() {
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

# Check if running as root
check_sudo() {
    if [ "$EUID" -eq 0 ]; then
        print_warning "Running as root. This is not recommended."
        SUDO=""
    else
        SUDO="sudo"
    fi
}

# Detect package manager
detect_package_manager() {
    if command -v apt-get &> /dev/null; then
        PKG_MANAGER="apt-get"
        PKG_UPDATE="$SUDO apt-get update"
        PKG_INSTALL="$SUDO apt-get install -y"
    elif command -v dnf &> /dev/null; then
        PKG_MANAGER="dnf"
        PKG_UPDATE="$SUDO dnf check-update || true"
        PKG_INSTALL="$SUDO dnf install -y"
    elif command -v yum &> /dev/null; then
        PKG_MANAGER="yum"
        PKG_UPDATE="$SUDO yum check-update || true"
        PKG_INSTALL="$SUDO yum install -y"
    elif command -v pacman &> /dev/null; then
        PKG_MANAGER="pacman"
        PKG_UPDATE="$SUDO pacman -Sy"
        PKG_INSTALL="$SUDO pacman -S --noconfirm"
    else
        print_error "No supported package manager found (apt-get, dnf, yum, pacman)"
        exit 1
    fi
    print_info "Detected package manager: $PKG_MANAGER"
}

# Check and install a package
check_and_install() {
    local package_name=$1
    local command_name=${2:-$1}

    if command -v "$command_name" &> /dev/null; then
        print_success "$package_name is already installed"
        return 0
    fi

    print_warning "$package_name not found. Installing..."

    case $PKG_MANAGER in
        apt-get)
            $PKG_INSTALL "$package_name"
            ;;
        dnf|yum)
            $PKG_INSTALL "$package_name"
            ;;
        pacman)
            $PKG_INSTALL "$package_name"
            ;;
    esac

    if [ $? -eq 0 ]; then
        print_success "$package_name installed successfully"
    else
        print_error "Failed to install $package_name"
        exit 1
    fi
}

# Check and install library packages
check_and_install_lib() {
    local lib_name=$1
    local pkg_name=$2

    print_info "Checking for $lib_name..."

    case $PKG_MANAGER in
        apt-get)
            if ! dpkg -l | grep -q "$pkg_name"; then
                print_warning "$lib_name not found. Installing..."
                $PKG_INSTALL "$pkg_name"
            else
                print_success "$lib_name is already installed"
            fi
            ;;
        dnf|yum)
            if ! rpm -qa | grep -q "$pkg_name"; then
                print_warning "$lib_name not found. Installing..."
                $PKG_INSTALL "$pkg_name"
            else
                print_success "$lib_name is already installed"
            fi
            ;;
        pacman)
            if ! pacman -Q "$pkg_name" &> /dev/null; then
                print_warning "$lib_name not found. Installing..."
                $PKG_INSTALL "$pkg_name"
            else
                print_success "$lib_name is already installed"
            fi
            ;;
    esac
}

# Main installation function
install_dependencies() {
    print_info "Checking and installing dependencies..."

    # Update package list
    print_info "Updating package list..."
    $PKG_UPDATE

    # Essential build tools
    check_and_install "build-essential" "gcc" || check_and_install "base-devel" "gcc" || check_and_install "gcc" "gcc"
    check_and_install "cmake" "cmake"
    check_and_install "pkg-config" "pkg-config"
    check_and_install "git" "git"

    # SDL2 libraries
    print_info "Installing SDL2 libraries..."
    case $PKG_MANAGER in
        apt-get)
            check_and_install_lib "SDL2" "libsdl2-dev"
            ;;
        dnf|yum)
            check_and_install_lib "SDL2" "SDL2-devel"
            ;;
        pacman)
            check_and_install_lib "SDL2" "sdl2"
            ;;
    esac

    # FreeType libraries
    print_info "Installing FreeType libraries..."
    case $PKG_MANAGER in
        apt-get)
            check_and_install_lib "FreeType" "libfreetype6-dev"
            ;;
        dnf|yum)
            check_and_install_lib "FreeType" "freetype-devel"
            ;;
        pacman)
            check_and_install_lib "FreeType" "freetype2"
            ;;
    esac

    # OpenSSL for SQLCipher
    print_info "Installing OpenSSL libraries..."
    case $PKG_MANAGER in
        apt-get)
            check_and_install_lib "OpenSSL" "libssl-dev"
            ;;
        dnf|yum)
            check_and_install_lib "OpenSSL" "openssl-devel"
            ;;
        pacman)
            check_and_install_lib "OpenSSL" "openssl"
            ;;
    esac

    print_success "All dependencies installed successfully!"
}

# Build the application
build_application() {
    print_info "Building the application..."

    # Navigate to Source directory
    cd Source

    # Create build directory
    if [ -d "build" ]; then
        print_info "Build directory exists. Cleaning..."
        rm -rf build
    fi

    mkdir build
    cd build

    # Run CMake
    print_info "Running CMake..."
    if ! cmake ..; then
        print_error "CMake configuration failed"
        exit 1
    fi

    # Build
    print_info "Compiling..."
    if ! make -j$(nproc); then
        print_error "Compilation failed"
        exit 1
    fi

    print_success "Build completed successfully!"
    print_info "Executable location: $(pwd)/main"

    cd ../..
}

# Run the application
run_application() {
    print_info "Running the application..."
    cd Source/build
    ./main
}

# Display help
show_help() {
    cat << EOF
Usage: $0 [OPTIONS]

Build script for KorEngNumQWerty LVGL Application

OPTIONS:
    -h, --help          Show this help message
    -d, --deps-only     Install dependencies only (don't build)
    -b, --build-only    Build only (skip dependency check)
    -r, --run           Build and run the application
    -c, --clean         Clean build directory before building
    --skip-deps         Skip dependency installation (use with caution)

EXAMPLES:
    $0                  Install dependencies and build
    $0 -r               Install dependencies, build, and run
    $0 -d               Install dependencies only
    $0 -b               Build only (assumes dependencies are installed)
    $0 -c               Clean and build

EOF
}

# Parse command line arguments
DEPS_ONLY=false
BUILD_ONLY=false
RUN_APP=false
CLEAN_BUILD=false
SKIP_DEPS=false

while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            exit 0
            ;;
        -d|--deps-only)
            DEPS_ONLY=true
            shift
            ;;
        -b|--build-only)
            BUILD_ONLY=true
            shift
            ;;
        -r|--run)
            RUN_APP=true
            shift
            ;;
        -c|--clean)
            CLEAN_BUILD=true
            shift
            ;;
        --skip-deps)
            SKIP_DEPS=true
            shift
            ;;
        *)
            print_error "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

# Main execution
main() {
    print_info "KorEngNumQWerty LVGL Application Build Script"
    print_info "=============================================="

    check_sudo
    detect_package_manager

    # Install dependencies if needed
    if [ "$BUILD_ONLY" = false ] && [ "$SKIP_DEPS" = false ]; then
        install_dependencies
    fi

    # Exit if only installing dependencies
    if [ "$DEPS_ONLY" = true ]; then
        print_success "Dependencies installation completed!"
        exit 0
    fi

    # Clean if requested
    if [ "$CLEAN_BUILD" = true ]; then
        print_info "Cleaning build directory..."
        rm -rf Source/build
        print_success "Build directory cleaned"
    fi

    # Build the application
    build_application

    # Run if requested
    if [ "$RUN_APP" = true ]; then
        run_application
    else
        print_info "To run the application, execute: ./Source/build/main"
        print_info "Or use: $0 -r"
    fi

    print_success "All done!"
}

# Run main function
main
