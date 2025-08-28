#!/bin/bash

# Check if LVGL source code is available
echo "=== LVGL Source Code Check ==="
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

# Check for LVGL source directory
if [ -d "lvgl" ]; then
    echo "✅ LVGL source directory exists"
else
    echo "❌ LVGL source directory missing"
    echo "   The lvgl/ directory should be in the repository"
    exit 1
fi

# Check for key LVGL files
key_files=(
    "lvgl/lvgl.h"
    "lvgl/src/core/lv_obj.h"
    "lvgl/src/core/lv_obj_tree.h"
    "lvgl/lv_conf.h"
)

echo ""
echo "Checking for key LVGL files..."

for file in "${key_files[@]}"; do
    if [ -f "$file" ]; then
        echo "✅ $file"
    else
        echo "❌ $file missing"
        missing_files=true
    fi
done

echo ""

# Check LVGL directory size
lvgl_size=$(du -sh lvgl/ | cut -f1)
echo "LVGL directory size: $lvgl_size"

echo ""

# Check if LVGL is tracked by git
echo "Checking git status for LVGL files..."
if git ls-files | grep -q "^lvgl/"; then
    echo "✅ LVGL files are tracked by git"
    lvgl_file_count=$(git ls-files | grep "^lvgl/" | wc -l)
    echo "   Number of LVGL files in git: $lvgl_file_count"
else
    echo "❌ LVGL files are not tracked by git"
    echo "   This might cause issues when pulling on other computers"
fi

echo ""

# Summary
if [ "$missing_files" = true ]; then
    echo "❌ Some LVGL files are missing!"
    echo "   This might cause compilation issues."
    exit 1
else
    echo "✅ LVGL source code is properly available"
    echo ""
    echo "The LVGL source code is ready for compilation."
    echo "If you encounter header errors, make sure to build the LVGL library:"
    echo "  ./build_all_libs.sh lvgl"
fi
