#!/bin/bash

# Calendar Test Runner
echo "=========================================="
echo "Calendar Test Suite Runner"
echo "=========================================="

# Check if test executable exists
if [ ! -f "calendar_test" ]; then
    echo "Test executable not found. Building first..."
    ./build_calendar_test.sh
    if [ $? -ne 0 ]; then
        echo "Build failed!"
        exit 1
    fi
fi

# Run the tests
echo ""
echo "Running calendar tests..."
echo "=========================================="
./calendar_test

# Check exit code
if [ $? -eq 0 ]; then
    echo ""
    echo "=========================================="
    echo "✓ All calendar tests passed!"
    echo "=========================================="
else
    echo ""
    echo "=========================================="
    echo "✗ Some calendar tests failed!"
    echo "=========================================="
    exit 1
fi 