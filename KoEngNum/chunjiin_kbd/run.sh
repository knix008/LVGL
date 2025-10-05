#!/bin/bash

# Run the Chunjiin Keyboard application

if [ ! -f "build/chunjiin_keyboard" ]; then
    echo "ERROR: Executable not found. Please run ./build.sh first"
    exit 1
fi

cd build
./chunjiin_keyboard
