#!/bin/bash

echo "Available input devices:"
echo "========================"

# List all input devices
for device in /dev/input/event*; do
    if [ -e "$device" ]; then
        echo "Device: $device"
        if command -v evtest &> /dev/null; then
            echo "  Info: $(evtest --query $device 2>/dev/null | head -1)"
        fi
        echo ""
    fi
done

echo "DRM devices:"
echo "============"
for device in /dev/dri/card*; do
    if [ -e "$device" ]; then
        echo "Device: $device"
        if command -v drm_info &> /dev/null; then
            echo "  Info: $(drm_info $device 2>/dev/null | head -5)"
        fi
        echo ""
    fi
done

echo "To find the correct device paths:"
echo "1. For mouse/touchpad: Look for devices with 'mouse' or 'touchpad' in the info"
echo "2. For keyboard: Look for devices with 'keyboard' in the info"
echo "3. For DRM: Usually /dev/dri/card0 is the primary graphics card"
echo ""
echo "You may need to run this script with sudo to get full device information." 