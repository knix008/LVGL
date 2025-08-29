# Device Test Program - Installation Guide

This guide provides instructions for installing all dependencies required by the Device Test Program.

## Quick Installation

For automatic installation on supported systems, run:

```bash
./install_dependencies.sh
```

This script will automatically detect your operating system and install all required dependencies.

## Manual Installation

If you prefer to install dependencies manually or if your system is not supported by the automatic script, follow the instructions below.

### Prerequisites

- Linux operating system
- sudo privileges
- Internet connection

### Ubuntu/Debian Systems

```bash
# Update package list
sudo apt update

# Essential build tools
sudo apt install -y build-essential cmake pkg-config git

# OpenCV dependencies (for camera testing)
sudo apt install -y libopencv-dev libopencv-contrib-dev

# V4L2 dependencies (for video device testing)
sudo apt install -y libv4l-dev v4l-utils

# ALSA dependencies (for speaker testing)
sudo apt install -y libasound2-dev alsa-utils

# Bluetooth dependencies (for Bluetooth testing)
sudo apt install -y libbluetooth-dev bluez bluez-tools

# Network testing dependencies
sudo apt install -y net-tools iputils-ping

# Serial/UART dependencies (for UART testing)
sudo apt install -y minicom

# GPIO dependencies (for LED and Wiegand testing)
sudo apt install -y gpiod

# Display/DRM dependencies (for LCD testing)
sudo apt install -y libdrm-dev

# GTK dependencies (optional, for GUI)
sudo apt install -y libgtk-3-dev libgdk-3-dev

# Additional utilities
sudo apt install -y hwinfo lshw
```

### CentOS/RHEL/Fedora Systems

```bash
# Update package list
sudo yum update -y  # or sudo dnf update -y

# Essential build tools
sudo yum groupinstall -y "Development Tools"  # or sudo dnf groupinstall -y "Development Tools"
sudo yum install -y cmake pkg-config git  # or sudo dnf install -y cmake pkg-config git

# OpenCV dependencies
sudo yum install -y opencv-devel  # or sudo dnf install -y opencv-devel

# V4L2 dependencies
sudo yum install -y v4l-utils  # or sudo dnf install -y v4l-utils

# ALSA dependencies
sudo yum install -y alsa-lib-devel  # or sudo dnf install -y alsa-lib-devel

# Bluetooth dependencies
sudo yum install -y bluez-libs-devel bluez-tools  # or sudo dnf install -y bluez-libs-devel bluez-tools

# Network testing dependencies
sudo yum install -y net-tools iputils  # or sudo dnf install -y net-tools iputils

# Serial/UART dependencies
sudo yum install -y minicom  # or sudo dnf install -y minicom

# Display/DRM dependencies
sudo yum install -y libdrm-devel  # or sudo dnf install -y libdrm-devel

# GTK dependencies
sudo yum install -y gtk3-devel gdk3-devel  # or sudo dnf install -y gtk3-devel gdk3-devel
```

### Arch Linux

```bash
# Update package list
sudo pacman -Syu --noconfirm

# Essential build tools
sudo pacman -S --noconfirm base-devel cmake pkg-config git

# OpenCV dependencies
sudo pacman -S --noconfirm opencv

# V4L2 dependencies
sudo pacman -S --noconfirm v4l-utils

# ALSA dependencies
sudo pacman -S --noconfirm alsa-lib

# Bluetooth dependencies
sudo pacman -S --noconfirm bluez bluez-utils

# Network testing dependencies
sudo pacman -S --noconfirm net-tools iputils

# Serial/UART dependencies
sudo pacman -S --noconfirm minicom

# Display/DRM dependencies
sudo pacman -S --noconfirm libdrm

# GTK dependencies
sudo pacman -S --noconfirm gtk3
```

### Alpine Linux

```bash
# Update package list
sudo apk update

# Essential build tools
sudo apk add build-base cmake pkgconfig git

# OpenCV dependencies
sudo apk add opencv-dev

# V4L2 dependencies
sudo apk add v4l-utils

# ALSA dependencies
sudo apk add alsa-lib-dev

# Bluetooth dependencies
sudo apk add bluez-dev bluez

# Network testing dependencies
sudo apk add net-tools iputils

# Serial/UART dependencies
sudo apk add minicom

# Display/DRM dependencies
sudo apk add libdrm-dev

# GTK dependencies
sudo apk add gtk+3.0-dev
```

## Dependency Details

### Core Dependencies

| Package | Purpose | Required |
|---------|---------|----------|
| `build-essential` | C/C++ compiler and build tools | Yes |
| `cmake` | Build system generator | Yes |
| `pkg-config` | Package configuration tool | Yes |
| `git` | Version control (for cloning) | Optional |

### Device Testing Dependencies

| Package | Purpose | Device Type |
|---------|---------|-------------|
| `libopencv-dev` | Computer vision library | Camera |
| `libv4l-dev` | Video4Linux2 API | Camera |
| `libasound2-dev` | ALSA audio library | Speaker |
| `libbluetooth-dev` | Bluetooth library | Bluetooth |
| `libdrm-dev` | Direct Rendering Manager | LCD |
| `gpiod` | GPIO control tools | LED, Wiegand |

### Testing Tools

| Package | Purpose | Device Type |
|---------|---------|-------------|
| `v4l-utils` | Video4Linux2 utilities | Camera |
| `alsa-utils` | ALSA utilities | Speaker |
| `bluez-tools` | Bluetooth utilities | Bluetooth |
| `net-tools` | Network utilities | Network |
| `iputils-ping` | Ping utility | Network |
| `minicom` | Serial communication | Serial/UART |

### Optional Dependencies

| Package | Purpose | Feature |
|---------|---------|---------|
| `libgtk-3-dev` | GTK3 GUI library | GUI support |
| `hwinfo` | Hardware information | System info |
| `lshw` | Hardware lister | System info |

## Verification

After installation, verify that all dependencies are properly installed:

```bash
# Check if core tools are available
cmake --version
pkg-config --version

# Check if libraries are available
pkg-config --exists opencv4 && echo "OpenCV: OK" || echo "OpenCV: Missing"
pkg-config --exists libv4l2 && echo "V4L2: OK" || echo "V4L2: Missing"
pkg-config --exists alsa && echo "ALSA: OK" || echo "ALSA: Missing"
pkg-config --exists bluez && echo "Bluetooth: OK" || echo "Bluetooth: Missing"
```

## Building the Program

After installing dependencies, build the program:

```bash
./build.sh
```

## Troubleshooting

### Common Issues

1. **CMake not found**: Install `cmake` package for your distribution
2. **OpenCV not found**: Install `libopencv-dev` or equivalent
3. **Permission denied**: Ensure you have sudo privileges
4. **Package not found**: Update your package list first

### Missing Packages

If a package is not available in your distribution's repositories:

1. Check if the package name is different in your distribution
2. Look for alternative packages that provide the same functionality
3. Consider building from source if necessary

### Build Errors

If you encounter build errors:

1. Ensure all dependencies are installed
2. Check that you have sufficient disk space
3. Verify that your compiler supports C++11 or later
4. Check the build output for specific error messages

## Support

If you encounter issues not covered in this guide:

1. Check the project's README.md for additional information
2. Review the build output for specific error messages
3. Ensure your system meets the minimum requirements
4. Consider using the automatic installation script instead
