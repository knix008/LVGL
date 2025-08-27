# FaceRecognition2.0 - System Library Dependencies

This document outlines all the system library dependencies required for the FaceRecognition2.0 project.

## Overview

The FaceRecognition2.0 project uses a hybrid approach where some libraries are built locally and others are system dependencies. This ensures consistent builds across different environments while leveraging system-optimized libraries where appropriate.

## Local Libraries (Built from Source)

These libraries are built locally and stored in the `Source/lib/` directory:

### 1. FreeType 2.13.2
- **Purpose**: Font rendering and text display
- **Build Script**: `Source/build_freetype.sh`
- **Location**: `Source/lib/lib/libfreetype.a`
- **Headers**: `Source/lib/include/freetype2/`
- **Configuration**: Static build, no external dependencies (harfbuzz, brotli, bzip2, png, zlib disabled)

### 2. SDL2 2.28.5
- **Purpose**: Cross-platform multimedia library for audio, video, and input handling
- **Build Script**: `Source/build_sdl2.sh`
- **Location**: `Source/lib/lib/libSDL2.a`
- **Headers**: `Source/lib/include/SDL2/`
- **Configuration**: Static build, X11 video backend, ALSA audio backend

### 3. OpenSSL 3.0.16
- **Purpose**: Cryptographic functions and SSL/TLS support
- **Build Script**: `Source/build_openssl.sh`
- **Location**: `Source/lib/lib/libcrypto.a`, `Source/lib/lib/libssl.a`
- **Headers**: `Source/lib/include/openssl/`
- **Configuration**: Static build, no shared libraries, no hardware acceleration

### 4. SQLCipher 4.5.4
- **Purpose**: Encrypted SQLite database with OpenSSL backend
- **Build Script**: `Source/build_sqlcipher.sh`
- **Location**: `Source/lib/lib/libsqlcipher.a`
- **Headers**: `Source/lib/include/sqlcipher/`
- **Configuration**: Static build, OpenSSL crypto backend, SQLite extensions enabled

### 5. LVGL (Light and Versatile Graphics Library)
- **Purpose**: Embedded graphics library for UI components
- **Build Script**: `Source/build_lvgl.sh`
- **Location**: `Source/lib/lib/liblvgl_lib.a`
- **Headers**: `Source/lib/include/lvgl_src/`, `Source/lib/include/lvgl_drivers/`
- **Configuration**: Static build, SDL driver, FreeType integration

## System Libraries (Required)

These libraries must be installed on the system:

### FFmpeg Libraries
- **Purpose**: Video and audio processing, codec support
- **Required Libraries**:
  - `libavcodec` (version 58.134.100) - FFmpeg codec library
  - `libavformat` (version 58.76.100) - FFmpeg container format library
  - `libavutil` (version 56.70.100) - FFmpeg utility library
  - `libswscale` (version 5.9.100) - FFmpeg image rescaling library
  - `libswresample` (version 3.9.100) - FFmpeg audio resampling library

### System Libraries (Unix/Linux)
- **pthread** - POSIX threads library
- **dl** - Dynamic linking library
- **m** - Math library
- **crypto** - Cryptographic library (OpenSSL compatibility)

## Build System Dependencies

### CMake
- **Version**: 3.10 or higher
- **Purpose**: Build system configuration

### pkg-config
- **Purpose**: Package configuration and discovery
- **Used for**: FFmpeg library detection

### Build Tools
- **make** - Build automation
- **gcc/g++** - C/C++ compiler
- **wget** - File download utility
- **tar** - Archive extraction

## Installation Instructions

### Ubuntu/Debian System Dependencies

```bash
# Install system libraries
sudo apt update
sudo apt install -y \
    build-essential \
    cmake \
    pkg-config \
    wget \
    libavcodec-dev \
    libavformat-dev \
    libavutil-dev \
    libswscale-dev \
    libswresample-dev \
    libx11-dev \
    libasound2-dev
```

### Building Local Libraries

```bash
# Navigate to Source directory
cd Source

# Build all local libraries
./build_all_libs.sh

# Or build individual libraries
./build_freetype.sh
./build_sdl2.sh
./build_openssl.sh
./build_sqlcipher.sh
./build_lvgl.sh
```

### Building the Project

```bash
# Navigate to Source directory
cd Source

# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake ..

# Build the project
make
```

## Configuration Details

### CMake Configuration
The project uses CMake with the following key configurations:
- C standard: C99
- C++ standard: C++14
- Static linking for local libraries
- System library linking for FFmpeg components

### Compile Definitions
- `MG_TLS=MG_TLS_OPENSSL` - Mongoose TLS with OpenSSL
- `SQLITE_HAS_CODEC=1` - SQLCipher encryption support
- `SQLCIPHER_CRYPTO_OPENSSL=1` - SQLCipher OpenSSL backend
- Various SQLite extensions enabled (FTS5, JSON1, RTREE, etc.)

## Directory Structure

```
Source/
├── lib/                    # Local libraries
│   ├── include/           # Header files
│   └── lib/              # Static libraries
├── build_*.sh            # Build scripts for local libraries
├── CMakeLists.txt        # Main build configuration
└── src/                  # Application source code
```

## Troubleshooting

### Common Issues

1. **FFmpeg libraries not found**
   - Install FFmpeg development packages: `sudo apt install libavcodec-dev libavformat-dev libavutil-dev libswscale-dev libswresample-dev`

2. **OpenSSL build failures**
   - Ensure you have build tools installed: `sudo apt install build-essential`

3. **SDL2 X11 dependencies**
   - Install X11 development: `sudo apt install libx11-dev`

4. **Audio backend issues**
   - Install ALSA development: `sudo apt install libasound2-dev`

### Version Compatibility

- All local library versions are pinned to specific releases for consistency
- FFmpeg libraries should be version 58.x or higher for compatibility
- CMake 3.10+ is required for proper build configuration

## Notes

- Local libraries are built statically to avoid runtime dependency issues
- System FFmpeg libraries are used to leverage system optimizations
- The build system automatically handles library discovery and linking
- All build scripts include cleanup procedures to remove temporary files
