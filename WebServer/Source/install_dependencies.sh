#!/bin/bash

# ============================================================================
# LVGL WebServer - Dependency Installation Script
# ============================================================================
# This script automatically detects the system architecture and OS,
# and installs all required dependencies for building the LVGL WebServer.
#
# Usage: ./install_dependencies.sh [--help] [--dry-run] [--no-sudo]
# ============================================================================

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
DRY_RUN=false
USE_SUDO=true
HELP=false

# ============================================================================
# FUNCTIONS
# ============================================================================

print_header() {
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE}$1${NC}"
    echo -e "${BLUE}========================================${NC}"
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

detect_system() {
    # Detect OS
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        OS="linux"

        # Detect Linux distribution
        if [ -f /etc/os-release ]; then
            . /etc/os-release
            DISTRO=$ID
            DISTRO_VERSION=$VERSION_ID
            DISTRO_NAME=$PRETTY_NAME
        elif [ -f /etc/lsb-release ]; then
            . /etc/lsb-release
            DISTRO=$DISTRIB_ID
            DISTRO_VERSION=$DISTRIB_RELEASE
            DISTRO_NAME=$DISTRIB_DESCRIPTION
        else
            DISTRO="unknown"
            DISTRO_VERSION="unknown"
            DISTRO_NAME="Unknown Linux Distribution"
        fi
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        OS="macos"
        DISTRO="macos"
        DISTRO_VERSION=$(sw_vers -productVersion)
        DISTRO_NAME="macOS $DISTRO_VERSION"
    else
        OS="unknown"
        DISTRO="unknown"
        DISTRO_VERSION="unknown"
        DISTRO_NAME="Unknown OS"
    fi

    # Detect Architecture
    ARCH=$(uname -m)
    case "$ARCH" in
        aarch64|arm64)
            ARCH_TYPE="aarch64"
            ARCH_BITS="64"
            ARCH_DESC="ARM64 (aarch64)"
            ;;
        x86_64|AMD64)
            ARCH_TYPE="x86_64"
            ARCH_BITS="64"
            ARCH_DESC="x86-64 (AMD64)"
            ;;
        arm|armv7l)
            ARCH_TYPE="arm"
            ARCH_BITS="32"
            ARCH_DESC="ARM 32-bit"
            ;;
        *)
            ARCH_TYPE="unknown"
            ARCH_BITS="unknown"
            ARCH_DESC="Unknown Architecture"
            ;;
    esac
}

print_system_info() {
    print_header "System Information"
    print_info "Operating System: $DISTRO_NAME"
    print_info "Distribution: $DISTRO (v$DISTRO_VERSION)"
    print_info "Architecture: $ARCH_DESC ($ARCH)"
    print_info "Kernel: $(uname -s) $(uname -r)"
    echo ""
}

show_help() {
    cat << EOF
${BLUE}LVGL WebServer - Dependency Installation Script${NC}

${GREEN}Usage:${NC}
    ./install_dependencies.sh [OPTIONS]

${GREEN}Options:${NC}
    --help          Show this help message and exit
    --dry-run       Show what would be installed without actually installing
    --no-sudo       Don't use sudo (assume running as root or with permissions)

${GREEN}Examples:${NC}
    # Install with sudo (default)
    ./install_dependencies.sh

    # Preview what would be installed
    ./install_dependencies.sh --dry-run

    # Install without sudo (must have permissions)
    ./install_dependencies.sh --no-sudo

${GREEN}Supported Systems:${NC}
    • Ubuntu/Debian (apt)
    • Fedora/RHEL/CentOS (dnf/yum)
    • Alpine Linux (apk)
    • Arch Linux (pacman)
    • macOS (Homebrew)

${GREEN}Dependencies Installed:${NC}
    • Build Tools: cmake, make, gcc, g++, pkg-config
    • Graphics: libsdl2-dev, libfreetype6-dev
    • Security: libssl-dev
    • Multimedia: libavformat-dev, libavcodec-dev, libavutil-dev, libswscale-dev
    • Utilities: curl, git

${BLUE}Note:${NC}
    Some packages may have different names on different distributions.
    If installation fails, check the package names for your distribution.

EOF
}

check_command_exists() {
    if command -v "$1" &> /dev/null; then
        return 0
    else
        return 1
    fi
}

install_ubuntu_debian() {
    print_header "Installing Dependencies for Ubuntu/Debian"

    PACKAGES=(
        # Build tools
        "build-essential"
        "cmake"
        "make"
        "pkg-config"

        # Graphics libraries
        "libsdl2-dev"
        "libfreetype6-dev"

        # SSL/TLS
        "libssl-dev"

        # Audio/Video
        "libavformat-dev"
        "libavcodec-dev"
        "libavutil-dev"
        "libswscale-dev"
        "libz-dev"

        # Utilities
        "curl"
        "git"
    )

    print_info "Detected Ubuntu/Debian-based system"
    print_info "Package manager: apt"

    if [ "$DRY_RUN" = true ]; then
        print_warning "DRY RUN MODE - showing what would be installed:"
        echo ""
        for pkg in "${PACKAGES[@]}"; do
            echo "  - $pkg"
        done
        return 0
    fi

    print_info "Updating package manager..."
    if [ "$USE_SUDO" = true ]; then
        sudo apt-get update -y
    else
        apt-get update -y
    fi

    print_info "Installing packages..."
    for pkg in "${PACKAGES[@]}"; do
        if [ "$USE_SUDO" = true ]; then
            sudo apt-get install -y "$pkg" 2>/dev/null && print_success "Installed: $pkg" || print_warning "Failed to install: $pkg"
        else
            apt-get install -y "$pkg" 2>/dev/null && print_success "Installed: $pkg" || print_warning "Failed to install: $pkg"
        fi
    done
}

install_fedora_rhel() {
    print_header "Installing Dependencies for Fedora/RHEL/CentOS"

    PACKAGES=(
        # Build tools
        "gcc"
        "gcc-c++"
        "cmake"
        "make"
        "pkg-config"

        # Graphics libraries
        "SDL2-devel"
        "freetype-devel"

        # SSL/TLS
        "openssl-devel"

        # Audio/Video
        "ffmpeg-devel"

        # Utilities
        "curl"
        "git"
    )

    print_info "Detected Fedora/RHEL/CentOS system"
    print_info "Package manager: dnf/yum"

    if [ "$DRY_RUN" = true ]; then
        print_warning "DRY RUN MODE - showing what would be installed:"
        echo ""
        for pkg in "${PACKAGES[@]}"; do
            echo "  - $pkg"
        done
        return 0
    fi

    # Try dnf first (newer), fall back to yum
    if check_command_exists dnf; then
        PKG_MANAGER="dnf"
    else
        PKG_MANAGER="yum"
    fi

    print_info "Updating package manager..."
    if [ "$USE_SUDO" = true ]; then
        sudo $PKG_MANAGER makecache -y
    else
        $PKG_MANAGER makecache -y
    fi

    print_info "Installing packages..."
    for pkg in "${PACKAGES[@]}"; do
        if [ "$USE_SUDO" = true ]; then
            sudo $PKG_MANAGER install -y "$pkg" 2>/dev/null && print_success "Installed: $pkg" || print_warning "Failed to install: $pkg"
        else
            $PKG_MANAGER install -y "$pkg" 2>/dev/null && print_success "Installed: $pkg" || print_warning "Failed to install: $pkg"
        fi
    done
}

install_alpine() {
    print_header "Installing Dependencies for Alpine Linux"

    PACKAGES=(
        # Build tools
        "build-base"
        "cmake"
        "pkgconfig"

        # Graphics libraries
        "sdl2-dev"
        "freetype-dev"

        # SSL/TLS
        "openssl-dev"

        # Audio/Video
        "ffmpeg-dev"

        # Utilities
        "curl"
        "git"
    )

    print_info "Detected Alpine Linux"
    print_info "Package manager: apk"

    if [ "$DRY_RUN" = true ]; then
        print_warning "DRY RUN MODE - showing what would be installed:"
        echo ""
        for pkg in "${PACKAGES[@]}"; do
            echo "  - $pkg"
        done
        return 0
    fi

    print_info "Installing packages..."
    if [ "$USE_SUDO" = true ]; then
        sudo apk add "${PACKAGES[@]}"
    else
        apk add "${PACKAGES[@]}"
    fi
}

install_arch() {
    print_header "Installing Dependencies for Arch Linux"

    PACKAGES=(
        # Build tools
        "base-devel"
        "cmake"

        # Graphics libraries
        "sdl2"
        "freetype2"

        # SSL/TLS
        "openssl"

        # Audio/Video
        "ffmpeg"

        # Utilities
        "curl"
        "git"
    )

    print_info "Detected Arch Linux"
    print_info "Package manager: pacman"

    if [ "$DRY_RUN" = true ]; then
        print_warning "DRY RUN MODE - showing what would be installed:"
        echo ""
        for pkg in "${PACKAGES[@]}"; do
            echo "  - $pkg"
        done
        return 0
    fi

    print_info "Installing packages..."
    if [ "$USE_SUDO" = true ]; then
        sudo pacman -S --noconfirm "${PACKAGES[@]}"
    else
        pacman -S --noconfirm "${PACKAGES[@]}"
    fi
}

install_macos() {
    print_header "Installing Dependencies for macOS"

    PACKAGES=(
        "cmake"
        "sdl2"
        "freetype"
        "openssl"
        "ffmpeg"
    )

    print_info "Detected macOS"
    print_info "Package manager: Homebrew"

    if ! check_command_exists brew; then
        print_error "Homebrew not found!"
        print_warning "Please install Homebrew first: https://brew.sh"
        return 1
    fi

    if [ "$DRY_RUN" = true ]; then
        print_warning "DRY RUN MODE - showing what would be installed:"
        echo ""
        for pkg in "${PACKAGES[@]}"; do
            echo "  - $pkg"
        done
        return 0
    fi

    print_info "Installing packages..."
    for pkg in "${PACKAGES[@]}"; do
        brew install "$pkg" 2>/dev/null && print_success "Installed: $pkg" || print_warning "Failed to install: $pkg"
    done
}

verify_installation() {
    print_header "Verifying Installation"

    local all_found=true

    # Required build tools
    local build_tools=("cmake" "make" "gcc" "g++")
    for tool in "${build_tools[@]}"; do
        if check_command_exists "$tool"; then
            print_success "Found: $tool ($(which $tool))"
        else
            print_error "Missing: $tool"
            all_found=false
        fi
    done

    echo ""

    # Check pkg-config and libraries
    if check_command_exists pkg-config; then
        print_success "Found: pkg-config"

        # Check SDL2
        if pkg-config --exists sdl2 2>/dev/null; then
            print_success "Found: SDL2 ($(pkg-config --modversion sdl2))"
        else
            print_error "Missing: SDL2 development files"
            all_found=false
        fi

        # Check FreeType
        if pkg-config --exists freetype2 2>/dev/null; then
            print_success "Found: FreeType2 ($(pkg-config --modversion freetype2))"
        else
            print_error "Missing: FreeType2 development files"
            all_found=false
        fi

        # Check OpenSSL
        if pkg-config --exists openssl 2>/dev/null; then
            print_success "Found: OpenSSL ($(pkg-config --modversion openssl))"
        else
            print_error "Missing: OpenSSL development files"
            all_found=false
        fi

        # Check FFmpeg
        if pkg-config --exists libavformat libavcodec libavutil libswscale 2>/dev/null; then
            print_success "Found: FFmpeg"
        else
            print_error "Missing: FFmpeg development files"
            all_found=false
        fi
    else
        print_error "Missing: pkg-config"
        all_found=false
    fi

    echo ""

    if [ "$all_found" = true ]; then
        print_success "All dependencies are installed!"
        return 0
    else
        print_error "Some dependencies are missing!"
        return 1
    fi
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
        --dry-run)
            DRY_RUN=true
            shift
            ;;
        --no-sudo)
            USE_SUDO=false
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
print_header "LVGL WebServer Dependency Installation"

detect_system
print_system_info

# Check if running as root when not using sudo
if [ "$USE_SUDO" = false ] && [ "$EUID" -ne 0 ]; then
    print_warning "Not running as root and --no-sudo specified"
    print_warning "Installation may fail due to insufficient permissions"
    echo ""
fi

# Install dependencies based on distribution
case "$DISTRO" in
    ubuntu|debian|raspbian|linuxmint|pop)
        install_ubuntu_debian
        ;;
    fedora|rhel|centos|rocky|almalinux)
        install_fedora_rhel
        ;;
    alpine)
        install_alpine
        ;;
    arch|manjaro)
        install_arch
        ;;
    macos)
        install_macos
        ;;
    *)
        print_error "Unsupported distribution: $DISTRO"
        print_warning "Please install dependencies manually or submit an issue"
        exit 1
        ;;
esac

echo ""

# Verify installation
if [ "$DRY_RUN" = false ]; then
    verify_installation
else
    print_warning "Dry-run mode: No actual installation performed"
fi

echo ""
print_success "Dependency installation script completed!"
echo ""
