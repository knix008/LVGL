#!/bin/bash

# Script to copy HTML files to build directory
echo "Copying HTML files to build directory..."

# Create html directory if it doesn't exist
mkdir -p build/html

# Copy HTML files
cp html/*.html build/html/ 2>/dev/null || echo "No HTML files found"
cp html/*.css build/html/ 2>/dev/null || echo "No CSS files found"
cp html/*.js build/html/ 2>/dev/null || echo "No JS files found"

echo "HTML files copied successfully!"
