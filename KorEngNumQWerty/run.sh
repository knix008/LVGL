#!/bin/bash

# Run the LVGL Korean QWERTY application
# Usage: ./run.sh         - Just run the application (no rebuild)
#        ./run.sh build   - Build then run

cd Source/build

# Check if executable exists
if [ ! -f "KorEngNumQWerty" ]; then
    echo "Executable not found. Building application..."
    cd ..
    if [ ! -d "build" ]; then
        mkdir build
    fi
    cd build
    cmake .. && make -j$(nproc)
fi

# If user wants to rebuild
if [ "$1" = "build" ]; then
    echo "Rebuilding application..."
    make -j$(nproc)
fi

# Run the application
if [ -f "KorEngNumQWerty" ]; then
    echo "Running Korean/English/Number QWERTY Keypad..."
    ./KorEngNumQWerty
else
    echo "Error: Executable not found. Please build first with './build.sh'"
    exit 1
fi 