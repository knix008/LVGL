#!/bin/bash

# Build script for clock test
echo "Building clock test..."

# Compile the test
gcc -o test_clock test_clock.c

if [ $? -eq 0 ]; then
    echo "Clock test built successfully!"
    echo "Running clock test..."
    ./test_clock
else
    echo "Failed to build clock test"
    exit 1
fi 