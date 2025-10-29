#!/usr/bin/env bash
set -euo pipefail

echo "Simple Korean Keypad"
echo "======================"

ROOT_DIR=$(cd "$(dirname "$0")" && pwd)
cd "$ROOT_DIR"

if [ ! -x ./simple_korean_keypad ]; then
    echo "Building simple_korean_keypad..."
    make simple_korean_keypad
fi

# UTF-8 and MiniGUI
export LANG=ko_KR.UTF-8
export LC_ALL=ko_KR.UTF-8
export LC_CTYPE=ko_KR.UTF-8
export MG_CHARSET=UTF-8

# Libraries and fonts
export LD_LIBRARY_PATH="$ROOT_DIR/install/lib:$ROOT_DIR/ime:${LD_LIBRARY_PATH:-}"
FONT_DIRS=("$ROOT_DIR/assets" "/usr/share/fonts/truetype/noto" "/usr/share/fonts/truetype/nanum")
MGUI_FONTS_ACC=""
for d in "${FONT_DIRS[@]}"; do
  [ -d "$d" ] && MGUI_FONTS_ACC="$d:${MGUI_FONTS_ACC}"
done
export MGUI_FONTS="$MGUI_FONTS_ACC"

if fc-list | grep -qi "Noto Sans CJK KR"; then
  export MG_DEFAULTFONT="ttf-Noto Sans CJK KR-rrn16-UTF-8"
elif fc-list | grep -qi "NanumGothic" || ls "$ROOT_DIR/assets" 2>/dev/null | grep -qi NanumGothic; then
  export MG_DEFAULTFONT="ttf-NanumGothic-rrn16-UTF-8"
fi

echo "Starting simple_korean_keypad..."
./simple_korean_keypad


