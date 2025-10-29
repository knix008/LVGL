#!/usr/bin/env bash

set -euo pipefail

echo "Korean QWERTY Test Runner"
echo "=========================="
echo ""

# Ensure UTF-8 output for Korean in the terminal
export LANG=ko_KR.UTF-8
export LC_ALL=ko_KR.UTF-8
export LC_CTYPE=ko_KR.UTF-8

ROOT_DIR=$(cd "$(dirname "$0")" && pwd)
cd "$ROOT_DIR"

echo "Step 1/2: Building test binary (and required libs)..."
make tests
echo "✓ Build complete"
echo ""

echo "Step 2/2: Running test suite"
echo "============================"
echo "(Tip: your terminal must support UTF-8 to display Korean correctly)"
echo ""

./test_korean_qwerty
EXIT_CODE=$?

echo ""
echo "Test run finished (exit code: ${EXIT_CODE})"
if [ ${EXIT_CODE} -eq 0 ]; then
    echo "✅ All tests passed"
else
    echo "❌ Some tests failed"
fi

exit ${EXIT_CODE}


