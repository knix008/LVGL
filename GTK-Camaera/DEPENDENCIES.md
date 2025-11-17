# GTK Webcam Viewer - Dependencies Documentation

## Overview

This document provides complete information about all dependencies required to build and run the GTK Webcam Viewer with Face Recognition application.

## System Requirements

### Minimum Requirements
- **OS**: Linux (Ubuntu, Debian, Fedora, RHEL, Arch, or compatible)
- **Architecture**: x86_64
- **RAM**: 2GB
- **Disk Space**: 500MB free
- **Compiler**: g++ with C++17 support

### Recommended
- **OS**: Ubuntu 20.04 LTS or later
- **RAM**: 4GB+
- **Disk Space**: 1GB+
- **Processor**: Multi-core modern CPU

## Required Dependencies

### Build Tools
| Package | Purpose | Version |
|---------|---------|---------|
| build-essential | Compiler and build tools | Latest |
| cmake | Build configuration (optional) | 3.10+ |
| pkg-config | Package configuration tool | 0.29+ |
| git | Version control | 2.25+ |

### Graphics & UI Libraries
| Package | Purpose | Version | Status |
|---------|---------|---------|--------|
| libgtk-3-dev | GTK3 framework | 3.20+ | ✅ Installed |
| libgdk-pixbuf2.0-dev | Image handling | 2.36+ | ✅ Installed |

### Computer Vision
| Package | Purpose | Version | Status |
|---------|---------|---------|--------|
| libopencv-dev | OpenCV library | 4.0+ | ✅ Installed (4.6.0) |
| libopencv-face-dev | Face recognition module | 4.0+ | ✅ Included |

### Database
| Package | Purpose | Version | Status |
|---------|---------|---------|--------|
| libsqlite3-dev | SQLite3 database | 3.0+ | ✅ Installed |

### System Libraries
| Package | Purpose | Status |
|---------|---------|--------|
| libpthread | Multi-threading | ✅ Built-in |
| libz | Compression | ✅ Installed |
| libcairo | Graphics rendering | ✅ Installed |
| libpango | Text rendering | ✅ Installed |

## Installation Instructions

### Ubuntu / Debian

```bash
# Update package lists
sudo apt-get update

# Install all dependencies
sudo apt-get install -y \
    build-essential \
    cmake \
    git \
    libgtk-3-dev \
    libgdk-pixbuf2.0-dev \
    libopencv-dev \
    libsqlite3-dev \
    pkg-config
```

### Fedora / RHEL / CentOS

```bash
# Install all dependencies
sudo dnf install -y \
    gcc-c++ \
    cmake \
    git \
    gtk3-devel \
    gdk-pixbuf2-devel \
    opencv-devel \
    sqlite-devel \
    pkg-config
```

### Arch Linux

```bash
# Install all dependencies
sudo pacman -S \
    base-devel \
    cmake \
    git \
    gtk3 \
    gdk-pixbuf2 \
    opencv \
    sqlite \
    pkg-config
```

### macOS (for reference)

Note: Not officially supported, but can be adapted using Homebrew:

```bash
brew install gtk+3 opencv sqlite pkg-config
```

## Automated Installation

You can use the provided setup script:

```bash
chmod +x setup.sh
./setup.sh
```

The script will:
1. Detect your Linux distribution
2. Install all required packages
3. Add your user to the video group for camera access
4. Display setup completion instructions

## Dependency Verification

### Check Installed Packages

```bash
# Check GTK3
pkg-config --modversion gtk+-3.0

# Check GdkPixbuf
pkg-config --modversion gdk-pixbuf-2.0

# Check OpenCV
pkg-config --modversion opencv4

# Check SQLite3
sqlite3 --version
```

### Manual Verification Script

```bash
#!/bin/bash
echo "Checking dependencies..."

check_lib() {
    if pkg-config --exists "$1"; then
        echo "✅ $1: $(pkg-config --modversion $1)"
    else
        echo "❌ $1: NOT FOUND"
    fi
}

check_lib gtk+-3.0
check_lib gdk-pixbuf-2.0
check_lib opencv4
check_lib sqlite3

if command -v g++ &> /dev/null; then
    echo "✅ g++: $(g++ --version | head -1)"
else
    echo "❌ g++: NOT FOUND"
fi
```

## Runtime Dependencies

After building, the application requires these runtime libraries:

### Essential Runtime Libraries
- libgtk-3.so.0
- libgdk-pixbuf-2.0.so.0
- libopencv_core.so
- libopencv_imgproc.so
- libopencv_objdetect.so
- libopencv_face.so
- libsqlite3.so

### Optional (Usually Present)
- libcairo.so
- libpango.so
- libglib-2.0.so
- libgio-2.0.so

## Current System Status

### This System (Verified)
- **OS**: Linux (x86_64)
- **GTK3**: 3.24.41 ✅
- **GdkPixbuf**: 2.42.10 ✅
- **OpenCV**: 4.6.0 ✅
- **SQLite3**: 3.x ✅
- **All Dependencies**: INSTALLED ✅

### Verified Libraries
```
libgtk-3.so.0 → /lib/x86_64-linux-gnu/libgtk-3.so.0
libgdk-pixbuf-2.0.so.0 → /lib/x86_64-linux-gnu/libgdk-pixbuf-2.0.so.0
libopencv_core.so.406 → /lib/x86_64-linux-gnu/libopencv_core.so.406
libsqlite3.so.0 → /lib/x86_64-linux-gnu/libsqlite3.so.0
```

## Build System Dependencies

### Compiler
- **g++** (C++17 support required)
  - Command: `g++ --version`
  - Required flags: `-std=c++17`

### Build Tools
- **Make** or **CMake**
  - Our project uses **Makefile** (pure make)

### Package Configuration
- **pkg-config**
  - Used to find library paths and compiler flags

## Troubleshooting

### Missing GTK3 Development Files

```bash
# Error: gtk+-3.0 not found
# Solution:
sudo apt-get install libgtk-3-dev

# Verify:
pkg-config --modversion gtk+-3.0
```

### Missing OpenCV Development Files

```bash
# Error: opencv4 not found
# Solution:
sudo apt-get install libopencv-dev

# Verify:
pkg-config --modversion opencv4
```

### Missing SQLite3

```bash
# Error: lsqlite3 undefined reference
# Solution:
sudo apt-get install libsqlite3-dev

# Verify:
ls -l /usr/include/sqlite3.h
```

### Permission Issues with Camera

```bash
# Error: Cannot access /dev/video0
# Solution:
sudo usermod -a -G video $USER
# Log out and back in for changes to take effect
```

### Compilation Errors

If you encounter compilation errors:

1. **Verify all dependencies are installed**
   ```bash
   pkg-config --list-all | grep -E "gtk|opencv|sqlite"
   ```

2. **Update your package lists**
   ```bash
   sudo apt-get update
   ```

3. **Try a clean build**
   ```bash
   make clean
   make
   ```

4. **Check compiler version**
   ```bash
   g++ --version  # Should support C++17
   ```

## Version Compatibility Matrix

### Tested Compatible Versions

| Component | Min Version | Tested Version | Max Version |
|-----------|-------------|----------------|-------------|
| GTK3 | 3.20 | 3.24.41 | 4.0+ |
| GdkPixbuf | 2.36 | 2.42.10 | Latest |
| OpenCV | 4.0 | 4.6.0 | 4.8+ |
| SQLite3 | 3.8 | 3.x | 3.45+ |
| GCC | 7.0 | 11.4+ | Latest |

## Advanced Configuration

### Custom Library Paths

If dependencies are installed in non-standard locations:

```bash
# Set custom paths
export PKG_CONFIG_PATH=/custom/lib/pkgconfig:$PKG_CONFIG_PATH
export LD_LIBRARY_PATH=/custom/lib:$LD_LIBRARY_PATH

# Build
make clean
make
```

### Static Linking (Advanced)

To create a more portable executable:

```bash
make STATIC=1
```

Note: Some distributions may require specific flags for static linking.

## Documentation References

- [GTK3 Documentation](https://developer.gnome.org/gtk3/)
- [OpenCV Documentation](https://docs.opencv.org/)
- [SQLite3 Documentation](https://www.sqlite.org/docs.html)
- [pkg-config Documentation](https://people.freedesktop.org/~dbn/pkg-config-guide.html)

## Support

If you encounter dependency issues:

1. Check this documentation
2. See INSTALLATION.md for detailed setup
3. Check ERROR_HANDLING.md for common issues
4. Verify your system meets minimum requirements
5. Ensure all packages are fully installed and up-to-date

## License

This documentation is part of the GTK Webcam Viewer with Face Recognition project.

---

**Last Updated**: November 17, 2025
**Status**: ✅ All dependencies verified and installed
