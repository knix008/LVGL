#!/bin/bash

# Build script for SQLCipher amalgamation files
# This script generates sqlite3.h and sqlite3.c from the SQLCipher source
# TCL is only required if the amalgamation files don't already exist

set -e

echo "Checking SQLCipher amalgamation files..."

# Change to SQLCipher directory
cd sqlcipher

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

echo "SQLCipher amalgamation files generated successfully!"
echo "Files created:"
echo "  - sqlite3.h"
echo "  - sqlite3.c"

cd .. 