#!/bin/bash

# SDL2 Build Script
set -e

echo "=== Building SDL2 ==="

# Configuration
SDL2_VERSION="2.28.5"
SDL2_SOURCE="SDL2-${SDL2_VERSION}"
SDL2_ARCHIVE="${SDL2_SOURCE}.tar.gz"
SDL2_URL="https://github.com/libsdl-org/SDL/releases/download/release-${SDL2_VERSION}/${SDL2_ARCHIVE}"
LIB_DIR="lib"

# Create lib directory structure
mkdir -p ${LIB_DIR}/include
mkdir -p ${LIB_DIR}/lib

# Download and extract SDL2 if not exists
if [ ! -d "${SDL2_SOURCE}" ]; then
    echo "Downloading SDL2 ${SDL2_VERSION}..."
    if [ ! -f "${SDL2_ARCHIVE}" ]; then
        wget "${SDL2_URL}" || {
            echo "Error: Failed to download SDL2"
            exit 1
        }
    fi
    
    echo "Extracting SDL2..."
    tar -xf "${SDL2_ARCHIVE}" || {
        echo "Error: Failed to extract SDL2"
        exit 1
    }
fi

# Create build directory
BUILD_DIR="sdl2_build"
rm -rf "${BUILD_DIR}"
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

# Configure SDL2
echo "Configuring SDL2..."
cmake ../${SDL2_SOURCE} \
    -DCMAKE_INSTALL_PREFIX="$(pwd)/install" \
    -DSDL_STATIC=ON \
    -DSDL_SHARED=OFF \
    -DSDL_TEST=OFF \
    -DSDL_VIDEO_X11=ON \
    -DSDL_VIDEO_WAYLAND=OFF \
    -DSDL_AUDIO_ALSA=ON \
    -DSDL_AUDIO_PULSEAUDIO=OFF \
    -DSDL_AUDIO_JACK=OFF \
    -DSDL_AUDIO_ESD=OFF \
    -DSDL_AUDIO_ARTSC=OFF \
    -DSDL_AUDIO_NAS=OFF \
    -DSDL_AUDIO_SNDIO=OFF \
    -DSDL_AUDIO_FUSIONSOUND=OFF \
    -DSDL_AUDIO_DISK=OFF \
    -DSDL_AUDIO_DUMMY=OFF \
    -DSDL_VIDEO_OPENGL=OFF \
    -DSDL_VIDEO_OPENGLES=OFF \
    -DSDL_VIDEO_VULKAN=OFF \
    -DSDL_VIDEO_METAL=OFF \
    -DSDL_VIDEO_KMSDRM=OFF \
    -DSDL_VIDEO_RPI=OFF \
    -DSDL_VIDEO_VIVANTE=OFF \
    -DSDL_VIDEO_COCOA=OFF \
    -DSDL_VIDEO_DIRECTFB=OFF \
    -DSDL_VIDEO_PS2=OFF \
    -DSDL_VIDEO_PSP=OFF \
    -DSDL_VIDEO_NGAGE=OFF \
    -DSDL_VIDEO_WINDOWS=OFF \
    -DSDL_VIDEO_WINRT=OFF \
    -DSDL_VIDEO_EMSCRIPTEN=OFF \
    -DSDL_VIDEO_OS2=OFF \
    -DSDL_VIDEO_QNX=OFF \
    -DSDL_VIDEO_RISCOS=OFF \
    -DSDL_VIDEO_HAIKU=OFF \
    -DSDL_VIDEO_ANDROID=OFF \
    -DSDL_VIDEO_VITA=OFF \
    -DSDL_VIDEO_N3DS=OFF \
    -DSDL_VIDEO_UIKIT=OFF \
    -DSDL_VIDEO_TVOS=OFF \
    -DSDL_VIDEO_WAYLAND_QT_TOUCH=OFF \
    -DSDL_VIDEO_X11_XCURSOR=OFF \
    -DSDL_VIDEO_X11_XDBE=OFF \
    -DSDL_VIDEO_X11_XINERAMA=OFF \
    -DSDL_VIDEO_X11_XINPUT=OFF \
    -DSDL_VIDEO_X11_XRANDR=OFF \
    -DSDL_VIDEO_X11_XSCRNSAVER=OFF \
    -DSDL_VIDEO_X11_XSHAPE=OFF \
    -DSDL_VIDEO_X11_XVM=OFF \
    -DSDL_VIDEO_X11_XF86VM=OFF \
    -DSDL_VIDEO_X11_XME=OFF \
    -DSDL_VIDEO_X11_XREFRESH=OFF \
    -DSDL_VIDEO_X11_XSYNC=OFF \
    -DSDL_VIDEO_X11_XSS=OFF \
    -DSDL_VIDEO_X11_XVIDMODE=OFF \
    -DSDL_VIDEO_X11_XCURSOR=OFF \
    -DSDL_VIDEO_X11_XDBE=OFF \
    -DSDL_VIDEO_X11_XINERAMA=OFF \
    -DSDL_VIDEO_X11_XINPUT=OFF \
    -DSDL_VIDEO_X11_XRANDR=OFF \
    -DSDL_VIDEO_X11_XSCRNSAVER=OFF \
    -DSDL_VIDEO_X11_XSHAPE=OFF \
    -DSDL_VIDEO_X11_XVM=OFF \
    -DSDL_VIDEO_X11_XF86VM=OFF \
    -DSDL_VIDEO_X11_XME=OFF \
    -DSDL_VIDEO_X11_XREFRESH=OFF \
    -DSDL_VIDEO_X11_XSYNC=OFF \
    -DSDL_VIDEO_X11_XSS=OFF \
    -DSDL_VIDEO_X11_XVIDMODE=OFF

# Build SDL2
echo "Building SDL2..."
make -j$(nproc)

# Install to build directory
echo "Installing SDL2..."
make install

# Copy to lib directory
echo "Copying to lib directory..."
cp -r install/include/* ../${LIB_DIR}/include/
cp install/lib/libSDL2.a ../${LIB_DIR}/lib/

# Cleanup
cd ..
rm -rf "${BUILD_DIR}"

# Clean up downloaded archive and source directory
rm -f "${SDL2_ARCHIVE}"
rm -rf "${SDL2_SOURCE}"

echo "=== SDL2 build completed successfully ==="
echo "Libraries installed to: ${LIB_DIR}/lib/libSDL2.a"
echo "Headers installed to: ${LIB_DIR}/include/SDL2/"