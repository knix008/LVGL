#!/bin/bash
mkdir -p Source/build

# Run the LVGL Korean Tab application
cd Source/build

# Check if Makefile exists, if not run cmake
if [ ! -f "Makefile" ]; then
    echo "Makefile not found, running cmake..."
    cmake ..
fi

# Check command line arguments for build mode
BUILD_MODE="full"
if [ "$1" = "quick" ]; then
    BUILD_MODE="quick"
    echo "Quick build mode: rebuilding only main application (keeping LVGL library)"
elif [ "$1" = "full" ]; then
    BUILD_MODE="full"
    echo "Full build mode: rebuilding everything including LVGL library"
else
    echo "Build modes:"
    echo "  ./run.sh quick  - Rebuild only main application (faster)"
    echo "  ./run.sh full   - Rebuild everything including LVGL library"
    echo "  ./run.sh        - Default: full rebuild"
    echo ""
fi

if [ "$BUILD_MODE" = "full" ]; then
    # Clean everything and rebuild
    echo "Cleaning previous build..."
    make clean-all 2>/dev/null || make clean 2>/dev/null || echo "No previous build to clean"
    
    echo "Building application (including LVGL library)..."
    make -j$(nproc)
else
    # Quick build: clean only main sources, keep LVGL library
    echo "Cleaning main application files..."
    make clean-main 2>/dev/null || echo "No main files to clean"
    
    echo "Building application (using existing LVGL library)..."
    make -j$(nproc)
fi

if [ $? -eq 0 ]; then
    echo "Build successful! Running application..."
    ./main
else
    echo "Build failed!"
    exit 1
fi 