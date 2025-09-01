#!/bin/bash

# Check if the project is properly set up
# This script verifies that all required libraries and headers are present

set -e

echo "=== Project Setup Check ==="
echo ""

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    echo "❌ Error: Please run this script from the Source directory!"
    echo "Current directory: $(pwd)"
    echo "Expected: .../Source/"
    exit 1
fi

echo "✅ Running from correct directory"
echo ""

# Check for required directories
echo "Checking for required directories..."

if [ -d "lib" ]; then
    echo "✅ lib/ directory exists"
else
    echo "❌ lib/ directory missing"
    echo "   Run: ./setup_project.sh"
    exit 1
fi

if [ -d "lib/include" ]; then
    echo "✅ lib/include/ directory exists"
else
    echo "❌ lib/include/ directory missing"
    echo "   Run: ./setup_project.sh"
    exit 1
fi

if [ -d "lib/lib" ]; then
    echo "✅ lib/lib/ directory exists"
else
    echo "❌ lib/lib/ directory missing"
    echo "   Run: ./setup_project.sh"
    exit 1
fi

echo ""

# Check for required headers
echo "Checking for required headers..."

required_headers=(
    "lib/include/lvgl_src/core/lv_obj.h"
    "lib/include/SDL2/SDL.h"
    "lib/include/freetype2/freetype/freetype.h"
    "lib/include/mariadb/mysql.h"
    "lib/include/zlib/zlib.h"
)

for header in "${required_headers[@]}"; do
    if [ -f "$header" ]; then
        echo "✅ $header"
    else
        echo "❌ $header missing"
        missing_headers=true
    fi
done

echo ""

# Check for required libraries
echo "Checking for required libraries..."

required_libs=(
    "lib/lib/liblvgl_lib.a"
    "lib/lib/libSDL2.a"
    "lib/lib/libfreetype.a"
    "lib/lib/libmariadb.a"
    "lib/lib/libz.a"
)

for lib in "${required_libs[@]}"; do
    if [ -f "$lib" ]; then
        echo "✅ $lib"
    else
        echo "❌ $lib missing"
        missing_libs=true
    fi
done

echo ""

# Summary
if [ "$missing_headers" = true ] || [ "$missing_libs" = true ]; then
    echo "❌ Project setup incomplete!"
    echo ""
    echo "To fix this, run:"
    echo "  ./setup_project.sh"
    echo ""
    echo "This will build all required libraries and headers."
    exit 1
else
    echo "✅ Project setup complete!"
    echo ""
    echo "You can now build the main application:"
    echo "  mkdir -p build"
    echo "  cd build"
    echo "  cmake .."
    echo "  make"
    echo ""
    echo "Or use the run script:"
    echo "  ./run.sh"
fi
