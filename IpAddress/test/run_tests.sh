#!/bin/bash

# Test Runner Script for IP Address Validation
# ============================================

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to run a test and capture results
run_test() {
    local test_name="$1"
    local test_executable="$2"
    
    print_status "Running $test_name..."
    
    if [ -f "$test_executable" ]; then
        if ./"$test_executable"; then
            print_success "$test_name passed"
            return 0
        else
            print_error "$test_name failed"
            return 1
        fi
    else
        print_error "$test_executable not found"
        return 1
    fi
}

# Main execution
main() {
    echo "=========================================="
    echo "IP Address Validation Test Suite"
    echo "=========================================="
    echo ""
    
    # Change to test directory
    cd "$(dirname "$0")"
    
    # Build all tests
    print_status "Building test executables..."
    if make clean && make all; then
        print_success "Build completed successfully"
    else
        print_error "Build failed"
        exit 1
    fi
    
    echo ""
    echo "=========================================="
    echo "Running Tests"
    echo "=========================================="
    echo ""
    
    # Track test results
    local total_tests=0
    local passed_tests=0
    local failed_tests=0
    
    # Run individual tests
    local tests=(
        "IPv4 Validation:test_ipv4"
        "IPv6 Validation:test_ipv6"
        "Comprehensive Tests:test_all"
    )
    
    for test_info in "${tests[@]}"; do
        IFS=':' read -r test_name test_executable <<< "$test_info"
        total_tests=$((total_tests + 1))
        
        if run_test "$test_name" "$test_executable"; then
            passed_tests=$((passed_tests + 1))
        else
            failed_tests=$((failed_tests + 1))
        fi
        
        echo ""
    done
    
    # Summary
    echo "=========================================="
    echo "Test Summary"
    echo "=========================================="
    echo "Total tests: $total_tests"
    echo "Passed: $passed_tests"
    echo "Failed: $failed_tests"
    
    if [ $failed_tests -eq 0 ]; then
        print_success "All tests passed! 🎉"
        exit 0
    else
        print_error "Some tests failed! ❌"
        exit 1
    fi
}

# Handle command line arguments
case "${1:-}" in
    "ipv4")
        print_status "Running IPv4 tests only..."
        make test_ipv4 && ./test_ipv4
        ;;
    "ipv6")
        print_status "Running IPv6 tests only..."
        make test_ipv6 && ./test_ipv6
        ;;
    "comprehensive")
        print_status "Running comprehensive tests only..."
        make test_all && ./test_all
        ;;
    "clean")
        print_status "Cleaning test files..."
        make clean
        print_success "Clean completed"
        ;;
    "help"|"-h"|"--help")
        echo "Usage: $0 [option]"
        echo ""
        echo "Options:"
        echo "  (no option)  - Run all tests"
        echo "  ipv4        - Run IPv4 tests only"
        echo "  ipv6        - Run IPv6 tests only"
        echo "  comprehensive - Run comprehensive tests only"
        echo "  clean       - Clean test files"
        echo "  help        - Show this help"
        ;;
    "")
        main
        ;;
    *)
        print_error "Unknown option: $1"
        echo "Use '$0 help' for usage information"
        exit 1
        ;;
esac
