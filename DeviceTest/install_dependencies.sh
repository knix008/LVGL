#!/bin/bash

# Device Test Program - Dependencies Installation Script
# This script installs all required dependencies for the Device Test Program

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

# Function to check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to detect OS
detect_os() {
    if [[ -f /etc/os-release ]]; then
        . /etc/os-release
        OS=$NAME
        VER=$VERSION_ID
    elif type lsb_release >/dev/null 2>&1; then
        OS=$(lsb_release -si)
        VER=$(lsb_release -sr)
    elif [[ -f /etc/lsb-release ]]; then
        . /etc/lsb-release
        OS=$DISTRIB_ID
        VER=$DISTRIB_RELEASE
    elif [[ -f /etc/debian_version ]]; then
        OS=Debian
        VER=$(cat /etc/debian_version)
    elif [[ -f /etc/SuSe-release ]]; then
        OS=SuSE
    elif [[ -f /etc/redhat-release ]]; then
        OS=RedHat
    else
        OS=$(uname -s)
        VER=$(uname -r)
    fi
    echo "$OS"
}

# Function to install dependencies for Ubuntu/Debian
install_ubuntu_debian() {
    print_status "Installing dependencies for Ubuntu/Debian..."
    
    # Update package list
    print_status "Updating package list..."
    sudo apt update
    
    # Essential build tools
    print_status "Installing essential build tools..."
    sudo apt install -y build-essential cmake pkg-config git
    
    # OpenCV dependencies
    print_status "Installing OpenCV dependencies..."
    sudo apt install -y libopencv-dev libopencv-contrib-dev
    
    # V4L2 dependencies
    print_status "Installing V4L2 dependencies..."
    sudo apt install -y libv4l-dev v4l-utils
    
    # ALSA dependencies for speaker testing
    print_status "Installing ALSA dependencies..."
    sudo apt install -y libasound2-dev alsa-utils
    
    # Bluetooth dependencies
    print_status "Installing Bluetooth dependencies..."
    sudo apt install -y libbluetooth-dev bluez bluez-tools
    
    # Network testing dependencies
    print_status "Installing network testing dependencies..."
    sudo apt install -y net-tools iputils-ping
    
    # Serial/UART dependencies
    print_status "Installing serial/UART dependencies..."
    sudo apt install -y minicom
    
    # GPIO dependencies (for LED and Wiegand testing)
    print_status "Installing GPIO dependencies..."
    sudo apt install -y gpiod
    
    # Display/DRM dependencies (for LCD testing)
    print_status "Installing display/DRM dependencies..."
    sudo apt install -y libdrm-dev
    
    # GTK dependencies (optional, for GUI)
    print_status "Installing GTK dependencies..."
    sudo apt install -y libgtk-3-dev libgdk-3-dev
    
    # Additional utilities
    print_status "Installing additional utilities..."
    sudo apt install -y hwinfo lshw
    
    print_success "Ubuntu/Debian dependencies installed successfully!"
}

# Function to install dependencies for CentOS/RHEL/Fedora
install_centos_rhel_fedora() {
    print_status "Installing dependencies for CentOS/RHEL/Fedora..."
    
    # Update package list
    print_status "Updating package list..."
    sudo yum update -y || sudo dnf update -y
    
    # Essential build tools
    print_status "Installing essential build tools..."
    sudo yum groupinstall -y "Development Tools" || sudo dnf groupinstall -y "Development Tools"
    sudo yum install -y cmake pkg-config git || sudo dnf install -y cmake pkg-config git
    
    # OpenCV dependencies
    print_status "Installing OpenCV dependencies..."
    sudo yum install -y opencv-devel || sudo dnf install -y opencv-devel
    
    # V4L2 dependencies
    print_status "Installing V4L2 dependencies..."
    sudo yum install -y v4l-utils || sudo dnf install -y v4l-utils
    
    # ALSA dependencies
    print_status "Installing ALSA dependencies..."
    sudo yum install -y alsa-lib-devel || sudo dnf install -y alsa-lib-devel
    
    # Bluetooth dependencies
    print_status "Installing Bluetooth dependencies..."
    sudo yum install -y bluez-libs-devel bluez-tools || sudo dnf install -y bluez-libs-devel bluez-tools
    
    # Network testing dependencies
    print_status "Installing network testing dependencies..."
    sudo yum install -y net-tools iputils || sudo dnf install -y net-tools iputils
    
    # Serial/UART dependencies
    print_status "Installing serial/UART dependencies..."
    sudo yum install -y minicom || sudo dnf install -y minicom
    
    # Display/DRM dependencies
    print_status "Installing display/DRM dependencies..."
    sudo yum install -y libdrm-devel || sudo dnf install -y libdrm-devel
    
    # GTK dependencies
    print_status "Installing GTK dependencies..."
    sudo yum install -y gtk3-devel gdk3-devel || sudo dnf install -y gtk3-devel gdk3-devel
    
    print_success "CentOS/RHEL/Fedora dependencies installed successfully!"
}

# Function to install dependencies for Arch Linux
install_arch() {
    print_status "Installing dependencies for Arch Linux..."
    
    # Update package list
    print_status "Updating package list..."
    sudo pacman -Syu --noconfirm
    
    # Essential build tools
    print_status "Installing essential build tools..."
    sudo pacman -S --noconfirm base-devel cmake pkg-config git
    
    # OpenCV dependencies
    print_status "Installing OpenCV dependencies..."
    sudo pacman -S --noconfirm opencv
    
    # V4L2 dependencies
    print_status "Installing V4L2 dependencies..."
    sudo pacman -S --noconfirm v4l-utils
    
    # ALSA dependencies
    print_status "Installing ALSA dependencies..."
    sudo pacman -S --noconfirm alsa-lib
    
    # Bluetooth dependencies
    print_status "Installing Bluetooth dependencies..."
    sudo pacman -S --noconfirm bluez bluez-utils
    
    # Network testing dependencies
    print_status "Installing network testing dependencies..."
    sudo pacman -S --noconfirm net-tools iputils
    
    # Serial/UART dependencies
    print_status "Installing serial/UART dependencies..."
    sudo pacman -S --noconfirm minicom
    
    # Display/DRM dependencies
    print_status "Installing display/DRM dependencies..."
    sudo pacman -S --noconfirm libdrm
    
    # GTK dependencies
    print_status "Installing GTK dependencies..."
    sudo pacman -S --noconfirm gtk3
    
    print_success "Arch Linux dependencies installed successfully!"
}

# Function to install dependencies for Alpine Linux
install_alpine() {
    print_status "Installing dependencies for Alpine Linux..."
    
    # Update package list
    print_status "Updating package list..."
    sudo apk update
    
    # Essential build tools
    print_status "Installing essential build tools..."
    sudo apk add build-base cmake pkgconfig git
    
    # OpenCV dependencies
    print_status "Installing OpenCV dependencies..."
    sudo apk add opencv-dev
    
    # V4L2 dependencies
    print_status "Installing V4L2 dependencies..."
    sudo apk add v4l-utils
    
    # ALSA dependencies
    print_status "Installing ALSA dependencies..."
    sudo apk add alsa-lib-dev
    
    # Bluetooth dependencies
    print_status "Installing Bluetooth dependencies..."
    sudo apk add bluez-dev bluez
    
    # Network testing dependencies
    print_status "Installing network testing dependencies..."
    sudo apk add net-tools iputils
    
    # Serial/UART dependencies
    print_status "Installing serial/UART dependencies..."
    sudo apk add minicom
    
    # Display/DRM dependencies
    print_status "Installing display/DRM dependencies..."
    sudo apk add libdrm-dev
    
    # GTK dependencies
    print_status "Installing GTK dependencies..."
    sudo apk add gtk+3.0-dev
    
    print_success "Alpine Linux dependencies installed successfully!"
}

# Function to check if running as root
check_root() {
    if [[ $EUID -eq 0 ]]; then
        print_error "This script should not be run as root. Please run as a regular user with sudo privileges."
        exit 1
    fi
}

# Function to check sudo privileges
check_sudo() {
    if ! sudo -n true 2>/dev/null; then
        print_error "This script requires sudo privileges. Please run with sudo or ensure you have sudo access."
        exit 1
    fi
}

# Function to verify installations
verify_installations() {
    print_status "Verifying installations..."
    
    local missing_packages=()
    
    # Check essential tools
    if ! command_exists cmake; then
        missing_packages+=("cmake")
    fi
    
    if ! command_exists pkg-config; then
        missing_packages+=("pkg-config")
    fi
    
    if ! command_exists git; then
        missing_packages+=("git")
    fi
    
    # Check OpenCV
    if ! pkg-config --exists opencv4; then
        missing_packages+=("opencv4")
    fi
    
    # Check V4L2
    if ! pkg-config --exists libv4l2; then
        missing_packages+=("libv4l2")
    fi
    
    # Check ALSA
    if ! pkg-config --exists alsa; then
        missing_packages+=("alsa")
    fi
    
    # Check Bluetooth
    if ! pkg-config --exists bluez; then
        missing_packages+=("bluez")
    fi
    
    if [[ ${#missing_packages[@]} -eq 0 ]]; then
        print_success "All dependencies verified successfully!"
        return 0
    else
        print_warning "Some packages may not be properly installed:"
        for package in "${missing_packages[@]}"; do
            echo "  - $package"
        done
        return 1
    fi
}

# Function to show usage
show_usage() {
    echo "Device Test Program - Dependencies Installation Script"
    echo ""
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -h, --help          Show this help message"
    echo "  -v, --verify        Only verify installations (don't install)"
    echo "  -f, --force         Force installation even if packages exist"
    echo "  -y, --yes           Answer yes to all prompts"
    echo ""
    echo "This script will automatically detect your operating system and install"
    echo "all required dependencies for the Device Test Program."
    echo ""
    echo "Supported operating systems:"
    echo "  - Ubuntu/Debian"
    echo "  - CentOS/RHEL/Fedora"
    echo "  - Arch Linux"
    echo "  - Alpine Linux"
    echo ""
    echo "Dependencies that will be installed:"
    echo "  - Build tools (gcc, make, cmake, pkg-config)"
    echo "  - OpenCV (for camera testing)"
    echo "  - V4L2 (for video device testing)"
    echo "  - ALSA (for speaker testing)"
    echo "  - Bluetooth libraries (for Bluetooth testing)"
    echo "  - Network tools (for network testing)"
    echo "  - Serial tools (for UART testing)"
    echo "  - GPIO tools (for LED/Wiegand testing)"
    echo "  - Display libraries (for LCD testing)"
    echo "  - GTK (for optional GUI)"
}

# Main script
main() {
    local verify_only=false
    local force_install=false
    local auto_yes=false
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_usage
                exit 0
                ;;
            -v|--verify)
                verify_only=true
                shift
                ;;
            -f|--force)
                force_install=true
                shift
                ;;
            -y|--yes)
                auto_yes=true
                shift
                ;;
            *)
                print_error "Unknown option: $1"
                show_usage
                exit 1
                ;;
        esac
    done
    
    echo "=========================================="
    echo "Device Test Program - Dependencies Installer"
    echo "=========================================="
    echo ""
    
    # Check if running as root
    check_root
    
    # Check sudo privileges
    check_sudo
    
    # Detect operating system
    OS=$(detect_os)
    print_status "Detected operating system: $OS"
    
    if [[ "$verify_only" == true ]]; then
        verify_installations
        exit $?
    fi
    
    # Ask for confirmation unless auto-yes is set
    if [[ "$auto_yes" != true ]]; then
        echo ""
        echo "This script will install the following dependencies:"
        echo "  - Build tools (gcc, make, cmake, pkg-config)"
        echo "  - OpenCV (for camera testing)"
        echo "  - V4L2 (for video device testing)"
        echo "  - ALSA (for speaker testing)"
        echo "  - Bluetooth libraries (for Bluetooth testing)"
        echo "  - Network tools (for network testing)"
        echo "  - Serial tools (for UART testing)"
        echo "  - GPIO tools (for LED/Wiegand testing)"
        echo "  - Display libraries (for LCD testing)"
        echo "  - GTK (for optional GUI)"
        echo ""
        read -p "Do you want to continue? (y/N): " -n 1 -r
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            print_status "Installation cancelled."
            exit 0
        fi
    fi
    
    # Install dependencies based on OS
    case "$OS" in
        *"Ubuntu"*|*"Debian"*|*"Linux Mint"*)
            install_ubuntu_debian
            ;;
        *"CentOS"*|*"Red Hat"*|*"Fedora"*|*"RHEL"*)
            install_centos_rhel_fedora
            ;;
        *"Arch"*|*"Manjaro"*)
            install_arch
            ;;
        *"Alpine"*)
            install_alpine
            ;;
        *)
            print_error "Unsupported operating system: $OS"
            print_status "Please install dependencies manually or contact support."
            exit 1
            ;;
    esac
    
    # Verify installations
    echo ""
    verify_installations
    
    if [[ $? -eq 0 ]]; then
        echo ""
        print_success "All dependencies installed successfully!"
        echo ""
        echo "You can now build the Device Test Program:"
        echo "  ./build.sh"
        echo ""
        echo "Or run the automated test suite:"
        echo "  ./build/bin/DeviceTest -t auto"
    else
        echo ""
        print_warning "Some dependencies may not be properly installed."
        print_status "Please check the output above and install missing packages manually."
    fi
}

# Run main function
main "$@"
