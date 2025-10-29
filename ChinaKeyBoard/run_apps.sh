#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR=$(cd "$(dirname "$0")" && pwd)
cd "$ROOT_DIR"

echo "Korean Keypad Launcher"
echo "======================="
echo "1) Run simple keypad"
echo "2) Run fixed (clickable) keypad"
echo "3) Run test suite"
echo "q) Quit"
echo -n "Select: "
read -r sel

case "$sel" in
  1)
    exec "$ROOT_DIR/run_simple.sh"
    ;;
  2)
    exec "$ROOT_DIR/run_fixed.sh"
    ;;
  3)
    exec "$ROOT_DIR/run_test.sh"
    ;;
  q|Q)
    echo "Bye";
    exit 0
    ;;
  *)
    echo "Unknown selection";
    exit 1
    ;;
esac


