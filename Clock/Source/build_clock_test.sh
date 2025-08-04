#!/bin/bash

# Build script for clock logic test only
echo "Building clock logic test..."

# Compile the existing test file with clock.c
gcc -I./include -o clock_test_simple ./src/test_clock_logic.c ./src/clock.c

if [ $? -eq 0 ]; then
    echo "Clock logic test built successfully!"
    echo "Running clock logic test..."
    ./clock_test_simple
    echo "Clock test completed!"
    
    # Clean up
    rm -f clock_test_simple
else
    echo "Failed to build clock logic test"
    exit 1
fi 