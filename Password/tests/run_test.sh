#!/bin/bash
#
# run_test.sh - Run all tests in the tests directory
#

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Test results
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Function to print colored message
print_color() {
    local color=$1
    local message=$2
    echo -e "${color}${message}${NC}"
}

# Function to run a single test
run_test() {
    local test_name=$1
    local test_executable=$2
    
    echo ""
    print_color "$BLUE" "================================================"
    print_color "$BLUE" "Running: $test_name"
    print_color "$BLUE" "================================================"
    echo ""
    
    if [ ! -f "$test_executable" ]; then
        print_color "$RED" "❌ Test executable not found: $test_executable"
        print_color "$YELLOW" "Run 'make' to build tests first"
        return 1
    fi
    
    if [ ! -x "$test_executable" ]; then
        chmod +x "$test_executable"
    fi
    
    # Run the test and capture exit code
    ./"$test_executable"
    local exit_code=$?
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    if [ $exit_code -eq 0 ]; then
        print_color "$GREEN" "✅ $test_name PASSED"
        PASSED_TESTS=$((PASSED_TESTS + 1))
        return 0
    else
        print_color "$RED" "❌ $test_name FAILED (exit code: $exit_code)"
        FAILED_TESTS=$((FAILED_TESTS + 1))
        return 1
    fi
}

# Main script
print_color "$BLUE" ""
print_color "$BLUE" "========================================"
print_color "$BLUE" "     PASSWORD LOGIN TEST SUITE"
print_color "$BLUE" "========================================"
print_color "$BLUE" ""

# Change to tests directory
cd "$(dirname "$0")"

# Check if tests are built
if [ ! -f "test_password_validation" ] && [ ! -f "test_userid_validation" ]; then
    print_color "$YELLOW" "⚠️  Tests not built. Building now..."
    make all
    echo ""
fi

# Run all test executables
run_test "Password Validation Tests" "test_password_validation"
run_test "User ID Validation Tests" "test_userid_validation"

# Print summary
echo ""
print_color "$BLUE" "========================================"
print_color "$BLUE" "         TEST SUMMARY"
print_color "$BLUE" "========================================"
echo ""
echo "Total test suites: $TOTAL_TESTS"
print_color "$GREEN" "Passed: $PASSED_TESTS"
if [ $FAILED_TESTS -gt 0 ]; then
    print_color "$RED" "Failed: $FAILED_TESTS"
else
    echo "Failed: 0"
fi
echo ""

# Exit with appropriate code
if [ $FAILED_TESTS -eq 0 ]; then
    print_color "$GREEN" "✅ ALL TEST SUITES PASSED!"
    echo ""
    exit 0
else
    print_color "$RED" "❌ SOME TEST SUITES FAILED!"
    echo ""
    exit 1
fi

