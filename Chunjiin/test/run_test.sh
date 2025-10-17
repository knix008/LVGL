#!/bin/bash
# Run all Chunjiin test cases in the test directory
set -e

echo "Building tests..."
make

echo "Running test_chunjiin..."
./test_chunjiin

echo "Running test_chunjiin_extra..."
./test_chunjiin_extra

echo "All tests completed."
