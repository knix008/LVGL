#!/bin/bash

# Build script for SQLCipher amalgamation files
# This script generates sqlite3.h and sqlite3.c from the SQLCipher source
# TCL is only required if the amalgamation files don't already exist

set -e

echo "Checking SQLCipher amalgamation files..."

# Change to SQLCipher directory
cd sqlcipher

# Function to check and rebuild a binary if needed
check_and_rebuild_binary() {
    local binary_name=$1
    local makefile_target=$2
    local need_rebuild=0

    if [ ! -f "$binary_name" ]; then
        echo "$binary_name not found, will build it..."
        need_rebuild=1
    elif ! ./$binary_name --version &> /dev/null && ! ./$binary_name 2>&1 | head -1 | grep -q "usage\|Usage\|error" ; then
        echo "$binary_name is not compatible with current architecture, rebuilding..."
        need_rebuild=1
    fi

    if [ $need_rebuild -eq 1 ]; then
        echo "Building $binary_name for $(uname -m) architecture..."
        rm -f "$binary_name"
        make -f Makefile.linux-generic "$makefile_target"
        if [ $? -ne 0 ]; then
            echo "Error: Failed to build $binary_name"
            cd ..
            exit 1
        fi
        echo "$binary_name built successfully"
    fi
}

# Check and rebuild required binaries for current architecture
echo "Checking binary compatibility..."
check_and_rebuild_binary "jimsh" "jimsh"
check_and_rebuild_binary "mksourceid" "mksourceid"
check_and_rebuild_binary "src-verify" "src-verify"

# Check if amalgamation files already exist
if [ -f "sqlite3.h" ] && [ -f "sqlite3.c" ]; then
    echo "SQLCipher amalgamation files already exist!"
    echo "Files found:"
    echo "  - sqlite3.h"
    echo "  - sqlite3.c"
    echo ""
    echo "If you need to regenerate these files, delete them first and run this script again."
    echo "Note: Regeneration requires TCL to be installed."
    cd ..
    exit 0
fi

echo "SQLCipher amalgamation files not found. Generating them..."

# Check if TCL is available
if ! command -v tclsh &> /dev/null; then
    echo "Error: tclsh is required to generate SQLCipher amalgamation files."
    echo "Please install TCL:"
    echo "  Ubuntu/Debian: sudo apt-get install tcl"
    echo "  CentOS/RHEL: sudo yum install tcl"
    echo ""
    echo "Alternatively, you can download pre-generated amalgamation files from:"
    echo "  https://github.com/sqlcipher/sqlcipher/releases"
    cd ..
    exit 1
fi

# Generate sqlite3.h
echo "Generating sqlite3.h..."
make -f Makefile.linux-generic sqlite3.h

# Generate sqlite3.c
echo "Generating sqlite3.c..."
make -f Makefile.linux-generic sqlite3.c

# Patch sqlite3.c to include stdint.h for uint64_t support
echo "Patching sqlite3.c to include stdint.h..."
if grep -q "#include <sys/resource.h>" sqlite3.c; then
    # Insert stdint.h include after sys/resource.h
    sed -i '/^#include <sys\/resource.h>/a #include <stdint.h> /* needed for uint64_t */' sqlite3.c
    echo "Successfully patched sqlite3.c to include stdint.h"
else
    echo "Warning: Could not find sys/resource.h include in sqlite3.c for patching"
    echo "You may need to manually add '#include <stdint.h>' to sqlite3.c"
fi

echo "SQLCipher amalgamation files generated successfully!"
echo "Files created:"
echo "  - sqlite3.h"
echo "  - sqlite3.c (patched with stdint.h)"

cd .. 