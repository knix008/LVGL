#!/bin/bash

echo "Installing FreeType development dependencies..."

# Update package list
sudo apt update

# Install FreeType development packages
sudo apt install -y libfreetype6-dev pkg-config

echo "FreeType dependencies installed successfully!"
echo "You can now build your project with:"
echo "cd Source && mkdir -p build && cd build && cmake .. && make" 