#!/bin/bash

# MiniGUI Installation Script for Chunjiin Korean Input Method
# This script installs MiniGUI development libraries and dependencies

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
LOG_FILE="$SCRIPT_DIR/install.log"

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging function
log() {
    echo -e "$1" | tee -a "$LOG_FILE"
}

log_info() {
    log "${BLUE}[INFO]${NC} $1"
}

log_success() {
    log "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    log "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    log "${RED}[ERROR]${NC} $1"
}

# Check if running as root
check_root() {
    if [[ $EUID -eq 0 ]]; then
        log_warning "This script should not be run as root for security reasons."
        log_info "Please run as a regular user. sudo will be used when needed."
        exit 1
    fi
}

# Check system requirements
check_system() {
    log_info "Checking system requirements..."
    
    # Check if we're on a supported Linux distribution
    if ! command -v apt-get &> /dev/null && ! command -v yum &> /dev/null && ! command -v pacman &> /dev/null; then
        log_error "Unsupported package manager. This script supports apt, yum, and pacman."
        exit 1
    fi
    
    # Check available disk space (need at least 500MB)
    available_space=$(df . | tail -1 | awk '{print $4}')
    required_space=512000  # 500MB in KB
    
    if [[ $available_space -lt $required_space ]]; then
        log_error "Insufficient disk space. Need at least 500MB free."
        exit 1
    fi
    
    log_success "System requirements check passed"
}

# Install system dependencies
install_dependencies() {
    log_info "Installing system dependencies..."
    log_info "This will install:"
    log_info "  - Build tools (gcc, make, cmake, autotools)"
    log_info "  - X11 libraries and development files"
    log_info "  - Graphics libraries (FreeType, PNG, JPEG, DRM)"
    log_info "  - Input libraries (libinput, udev)"
    log_info "  - Text rendering (HarfBuzz, ICU)"
    log_info "  - Korean fonts (Nanum, Noto CJK)"

    if command -v apt-get &> /dev/null; then
        # Debian/Ubuntu
        log_info "Detected Debian/Ubuntu system"
        
        sudo apt-get update
        
        # Basic build tools
        sudo apt-get install -y \
            build-essential \
            cmake \
            git \
            pkg-config \
            autoconf \
            automake \
            libtool \
            wget \
            curl
        
        # Graphics and input libraries
        sudo apt-get install -y \
            libx11-dev \
            libxext-dev \
            libxft-dev \
            libfreetype6-dev \
            libfontconfig1-dev \
            libjpeg-dev \
            libpng-dev \
            libgif-dev \
            zlib1g-dev

        # Advanced graphics and hardware support
        sudo apt-get install -y \
            libdrm-dev \
            libgbm-dev \
            libegl1-mesa-dev \
            libgles2-mesa-dev \
            libinput-dev \
            libudev-dev \
            libpciaccess-dev

        # Text rendering and internationalization
        sudo apt-get install -y \
            libharfbuzz-dev \
            libicu-dev \
            gettext

        # X11 additional libraries
        sudo apt-get install -y \
            x11-apps \
            xserver-xorg-core \
            xserver-xorg-video-all

        # Korean font support
        sudo apt-get install -y \
            fonts-nanum \
            fonts-nanum-coding \
            fonts-nanum-extra
            
    elif command -v yum &> /dev/null; then
        # RHEL/CentOS/Fedora
        log_info "Detected RHEL/CentOS/Fedora system"

        sudo yum groupinstall -y "Development Tools"
        sudo yum install -y \
            cmake \
            git \
            pkgconfig \
            autoconf \
            automake \
            libtool \
            wget \
            curl \
            libX11-devel \
            libXext-devel \
            libXft-devel \
            freetype-devel \
            fontconfig-devel \
            libjpeg-devel \
            libpng-devel \
            giflib-devel \
            zlib-devel \
            libdrm-devel \
            mesa-libEGL-devel \
            mesa-libGLES-devel \
            libinput-devel \
            systemd-devel \
            libpciaccess-devel \
            harfbuzz-devel \
            libicu-devel \
            gettext \
            xorg-x11-apps \
            google-noto-sans-cjk-fonts
            
    elif command -v pacman &> /dev/null; then
        # Arch Linux
        log_info "Detected Arch Linux system"

        sudo pacman -Syu --noconfirm
        sudo pacman -S --noconfirm \
            base-devel \
            cmake \
            git \
            pkgconf \
            autoconf \
            automake \
            libtool \
            wget \
            curl \
            libx11 \
            libxext \
            libxft \
            freetype2 \
            fontconfig \
            libjpeg-turbo \
            libpng \
            giflib \
            zlib \
            libdrm \
            mesa \
            libinput \
            systemd-libs \
            libpciaccess \
            harfbuzz \
            icu \
            gettext \
            xorg-apps \
            ttf-nanum \
            noto-fonts-cjk
    fi
    
    log_success "System dependencies installed successfully"
}

# Download and install MiniGUI
install_minigui() {
    log_info "Installing MiniGUI library from source..."
    log_info "This will download, compile, and install:"
    log_info "  - libMiniGUI (core GUI library)"
    log_info "  - mGUtils (utility library)"
    log_info ""

    # Create build directory
    BUILD_DIR="$SCRIPT_DIR/build"
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"

    # MiniGUI version to install
    MINIGUI_VERSION="5.0.12"

    # Install libminiGUI
    if [[ ! -d "libminigui-gpl-$MINIGUI_VERSION" ]]; then
        log_info "Downloading libMiniGUI $MINIGUI_VERSION from GitHub..."
        wget "https://github.com/VincentWei/minigui/releases/download/ver-$MINIGUI_VERSION/libminigui-gpl-$MINIGUI_VERSION.tar.gz"
        tar -xzf "libminigui-gpl-$MINIGUI_VERSION.tar.gz"
    else
        log_info "libMiniGUI source already downloaded, skipping download..."
    fi
    
    cd "libminigui-gpl-$MINIGUI_VERSION"

    log_info "Configuring libMiniGUI with the following options:"
    log_info "  - Video: DRM and X11 support (for hardware and desktop)"
    log_info "  - Runtime: Threads mode (for multi-threaded applications)"
    log_info "  - Features: Advanced 2D API, Complex scripts, FreeType fonts"
    log_info "  - UI: Mouse cursor, clipboard, double-click support"

    ./configure \
        --enable-videofbcon=no \
        --enable-videoqvfb=no \
        --enable-videovirtfb=no \
        --enable-videodrm=yes \
        --enable-videox11=yes \
        --with-targetname=external \
        --with-runtime-mode=ths \
        --enable-splash=no \
        --enable-screensaver=no \
        --enable-dblclk=yes \
        --enable-cursor=yes \
        --enable-clipboard=yes \
        --enable-adv2dapi=yes \
        --enable-complexscripts=yes \
        --enable-freetype=yes \
        --prefix=/usr/local
    
    log_info "Building libMiniGUI (this may take several minutes)..."
    make -j$(nproc)

    log_info "Installing libMiniGUI to /usr/local..."
    sudo make install

    # Update library cache so the system can find libminigui
    log_info "Updating library cache..."
    sudo ldconfig

    log_success "libMiniGUI $MINIGUI_VERSION installed successfully"

    cd "$BUILD_DIR"

    # Install mGUtils (provides additional controls and utilities)
    MGUTILS_VERSION="1.2.6"
    log_info ""
    log_info "Installing mGUtils $MGUTILS_VERSION (provides additional UI controls)..."

    if [[ ! -d "mgutils-$MGUTILS_VERSION" ]]; then
        log_info "Downloading mGUtils $MGUTILS_VERSION from GitHub..."
        wget "https://github.com/VincentWei/mgutils/releases/download/ver-$MGUTILS_VERSION/mgutils-$MGUTILS_VERSION.tar.gz"
        tar -xzf "mgutils-$MGUTILS_VERSION.tar.gz"
    else
        log_info "mGUtils source already downloaded, skipping download..."
    fi

    cd "mgutils-$MGUTILS_VERSION"
    log_info "Configuring and building mGUtils..."
    ./configure --prefix=/usr/local
    make -j$(nproc)
    log_info "Installing mGUtils to /usr/local..."
    sudo make install
    sudo ldconfig

    log_success "MiniGUI and mGUtils installed successfully"
}

# Setup MiniGUI configuration
setup_config() {
    log_info "Setting up MiniGUI configuration..."
    
    # Copy MiniGUI.cfg to the appropriate location
    if [[ -f "$SCRIPT_DIR/MiniGUI.cfg" ]]; then
        # Copy to home directory as .MiniGUI.cfg (highest priority for user)
        cp "$SCRIPT_DIR/MiniGUI.cfg" "$HOME/.MiniGUI.cfg"
        log_success "MiniGUI configuration copied to $HOME/.MiniGUI.cfg"
        
        # Also copy to /usr/local/etc for system-wide access
        sudo mkdir -p /usr/local/etc
        sudo cp "$SCRIPT_DIR/MiniGUI.cfg" /usr/local/etc/MiniGUI.cfg
        log_success "MiniGUI configuration copied to /usr/local/etc/MiniGUI.cfg"
    else
        log_warning "MiniGUI.cfg not found in current directory"
    fi
    
    # Create required directories
    mkdir -p "$HOME/.minigui"
    
    log_success "MiniGUI configuration setup completed"
}

# Verify installation
verify_installation() {
    log_info "Verifying MiniGUI installation..."
    
    # Check if libminigui is installed
    if ldconfig -p | grep -q libminigui; then
        log_success "libMiniGUI library found"
    else
        log_error "libMiniGUI library not found"
        return 1
    fi
    
    # Check if header files are installed
    if [[ -f "/usr/local/include/minigui/common.h" ]]; then
        log_success "MiniGUI header files found"
    else
        log_error "MiniGUI header files not found"
        return 1
    fi
    
    # Check pkg-config
    if pkg-config --exists minigui; then
        log_success "MiniGUI pkg-config found"
        log_info "MiniGUI version: $(pkg-config --modversion minigui)"
        log_info "MiniGUI CFLAGS: $(pkg-config --cflags minigui)"
        log_info "MiniGUI LIBS: $(pkg-config --libs minigui)"
    else
        log_warning "MiniGUI pkg-config not found, but this might be normal"
    fi
    
    log_success "Installation verification completed"
}

# Main installation process
main() {
    log_info "Starting MiniGUI installation for Chunjiin Korean Input Method"
    log_info "Installation log: $LOG_FILE"
    
    check_root
    check_system
    install_dependencies
    install_minigui
    setup_config
    verify_installation
    
    log_success "================================================"
    log_success "MiniGUI installation completed successfully!"
    log_success "================================================"
    log_info ""
    log_info "Installed components:"
    log_info "  ✓ All system dependencies (build tools, X11, graphics libs)"
    log_info "  ✓ libMiniGUI $MINIGUI_VERSION (core GUI library)"
    log_info "  ✓ mGUtils $MGUTILS_VERSION (additional UI controls)"
    log_info "  ✓ Korean fonts (Nanum/Noto CJK)"
    log_info "  ✓ MiniGUI configuration files"
    log_info ""
    log_info "Installation locations:"
    log_info "  - Libraries: /usr/local/lib"
    log_info "  - Headers: /usr/local/include/minigui"
    log_info "  - Config: $HOME/.MiniGUI.cfg"
    log_info ""
    log_info "Next steps:"
    log_info "  1. Run './build.sh' to build the Chunjiin application"
    log_info "  2. Or run 'make' to build using Makefile"
    log_info "  3. Make sure X11 is running (DISPLAY environment variable set)"
    log_info "  4. Run './chunjiin' to start the application"
    log_info ""
    log_info "For more information, see the README.md file"
}

# Cleanup function for interrupted installation
cleanup() {
    log_warning "Installation interrupted. Cleaning up..."
    # Remove any temporary files if needed
    exit 1
}

# Set up signal handlers
trap cleanup SIGINT SIGTERM

# Run main function
main "$@"