#!/usr/bin/env bash

set -euo pipefail

echo "Korean QWERTY Keypad Runner"
echo "============================"
echo ""

# Ensure UTF-8 output for Korean in the terminal
export LANG=ko_KR.UTF-8
export LC_ALL=ko_KR.UTF-8
export LC_CTYPE=ko_KR.UTF-8
export MG_CHARSET=UTF-8

# Set MiniGUI font preferences for Korean display
export MGUI_FONTS=./assets/NanumGothic-Regular.ttf
export MG_DEFAULTFONT=NanumGothic-Regular

ROOT_DIR=$(cd "$(dirname "$0")" && pwd)
cd "$ROOT_DIR"

echo "Step 1/2: Building QWERTY Korean keypad (and required libs)..."
make qwerty_korean_keypad
echo "✓ Build complete"
echo ""

echo "Step 2/2: Running QWERTY Korean keypad application"
echo "=================================================="
echo "(Tip: Make sure your display supports Korean fonts)"
echo ""

# Set library path for MiniGUI
export LD_LIBRARY_PATH=./install/lib:./ime:$LD_LIBRARY_PATH

./qwerty_korean_keypad
EXIT_CODE=$?

echo ""
echo "QWERTY Korean keypad finished (exit code: ${EXIT_CODE})"
if [ ${EXIT_CODE} -eq 0 ]; then
    echo "✅ Application closed normally"
else
    echo "❌ Application exited with error"
fi

exit ${EXIT_CODE}
