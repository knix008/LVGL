#!/bin/bash

# Firmware Builder Test Runner
# Runs all unit and integration tests

set -e

TEST_DIR="tests"
BIN_DIR="bin"
BUILD_DIR="build"
TEMP_DIR="$TEST_DIR/temp"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Cleanup function
cleanup() {
    if [ -d "$TEMP_DIR" ]; then
        rm -rf "$TEMP_DIR"
    fi
}

# Trap exit to cleanup
trap cleanup EXIT

# Create temp directory
mkdir -p "$TEMP_DIR"

echo "╔════════════════════════════════════════════════════════════╗"
echo "║        Firmware Builder - Test Suite Runner              ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""

# Verify binaries exist
if [ ! -f "$BIN_DIR/firmware-builder" ]; then
    echo -e "${RED}❌ Error: Firmware builder not built${NC}"
    echo "   Run: make all"
    exit 1
fi

if [ ! -f "$BIN_DIR/libfirmware.a" ]; then
    echo -e "${RED}❌ Error: Firmware library not built${NC}"
    echo "   Run: make all"
    exit 1
fi

total_passed=0
total_failed=0

run_test() {
    local test_name=$1
    local test_source=$2
    local test_binary="$BIN_DIR/$(basename "$test_source" .c)"

    echo -n "Building $test_name... "

    # Compile test
    if gcc -Wall -Wextra -O2 -std=c99 -I./include "$test_source" \
           -o "$test_binary" "$BIN_DIR/libfirmware.a" -lz -lcrypto -lssl 2>/dev/null; then
        echo -e "${GREEN}✓${NC}"
    else
        echo -e "${RED}✗${NC}"
        echo -e "${RED}  Compilation failed${NC}"
        total_failed=$((total_failed + 1))
        return 1
    fi

    echo -n "Running $test_name... "

    # Run test
    if "$test_binary"; then
        echo ""
        total_passed=$((total_passed + 1))
        return 0
    else
        echo ""
        echo -e "${RED}  Test failed${NC}"
        total_failed=$((total_failed + 1))
        return 1
    fi
}

echo "📋 Unit Tests"
echo "─────────────────────────────────────────────────────────────"
run_test "Compression" "$TEST_DIR/test_compressor.c"
run_test "CRC" "$TEST_DIR/test_crc.c"
run_test "Encryption" "$TEST_DIR/test_encryptor.c"
run_test "Hashing" "$TEST_DIR/test_hasher.c"

echo ""
echo "📋 Integration Tests"
echo "─────────────────────────────────────────────────────────────"
run_test "Integration" "$TEST_DIR/test_integration.c"

echo ""
echo "📋 Downloader Tests"
echo "─────────────────────────────────────────────────────────────"
run_test "Firmware Downloader" "$TEST_DIR/test_downloader.c"

# Summary
echo ""
echo "╔════════════════════════════════════════════════════════════╗"
echo "║                    Test Summary                            ║"
echo "╚════════════════════════════════════════════════════════════╝"

total_tests=$((total_passed + total_failed))

if [ $total_failed -eq 0 ]; then
    echo -e "${GREEN}✅ All $total_passed tests passed!${NC}"
    echo ""
    exit 0
else
    echo -e "${RED}❌ $total_failed/$total_tests tests failed${NC}"
    echo ""
    exit 1
fi
