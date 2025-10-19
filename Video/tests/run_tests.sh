#!/bin/bash

# Video Player Test Runner Script
# This script runs the test suite and generates reports

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Test configuration
TEST_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$TEST_DIR")"
TEST_EXECUTABLE="$TEST_DIR/test_video_player"
TEST_RESULTS="$TEST_DIR/test_results.log"
TEST_REPORT="$TEST_DIR/test_report.html"

# Function to print colored output
print_color() {
    local color=$1
    local message=$2
    echo -e "${color}${message}${NC}"
}

# Function to print header
print_header() {
    echo "=========================================="
    echo "$1"
    echo "=========================================="
}

# Function to check dependencies
check_dependencies() {
    print_header "Checking Dependencies"
    
    # Check if make is available
    if ! command -v make &> /dev/null; then
        print_color $RED "Error: make is not installed"
        exit 1
    fi
    
    # Check if gcc is available
    if ! command -v gcc &> /dev/null; then
        print_color $RED "Error: gcc is not installed"
        exit 1
    fi
    
    # Check if ffmpeg is available (optional)
    if ! command -v ffmpeg &> /dev/null; then
        print_color $YELLOW "Warning: ffmpeg is not installed - some tests may be skipped"
    else
        print_color $GREEN "✓ ffmpeg is available"
    fi
    
    # Check if required libraries are available
    if ! pkg-config --exists sdl2; then
        print_color $RED "Error: SDL2 development libraries are not installed"
        exit 1
    fi
    
    if ! pkg-config --exists libavcodec; then
        print_color $RED "Error: FFmpeg development libraries are not installed"
        exit 1
    fi
    
    print_color $GREEN "✓ All required dependencies are available"
}

# Function to build tests
build_tests() {
    print_header "Building Test Suite"
    
    cd "$TEST_DIR"
    
    # Clean previous build
    make clean
    
    # Build tests
    print_color $BLUE "Building test executable..."
    if make all; then
        print_color $GREEN "✓ Test executable built successfully"
    else
        print_color $RED "✗ Failed to build test executable"
        exit 1
    fi
}

# Function to run tests
run_tests() {
    print_header "Running Test Suite"
    
    cd "$TEST_DIR"
    
    # Run tests and capture output
    print_color $BLUE "Running tests..."
    if ./test_video_player > "$TEST_RESULTS" 2>&1; then
        print_color $GREEN "✓ All tests passed"
        return 0
    else
        print_color $RED "✗ Some tests failed"
        return 1
    fi
}

# Function to generate HTML report
generate_report() {
    print_header "Generating Test Report"
    
    local timestamp=$(date '+%Y-%m-%d %H:%M:%S')
    local total_tests=$(grep -c "Total Tests:" "$TEST_RESULTS" || echo "0")
    local passed_tests=$(grep -c "Passed:" "$TEST_RESULTS" || echo "0")
    local failed_tests=$(grep -c "Failed:" "$TEST_RESULTS" || echo "0")
    
    cat > "$TEST_REPORT" << EOF
<!DOCTYPE html>
<html>
<head>
    <title>Video Player Test Report</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        .header { background-color: #f0f0f0; padding: 20px; border-radius: 5px; }
        .summary { background-color: #e8f5e8; padding: 15px; border-radius: 5px; margin: 20px 0; }
        .error { background-color: #ffe8e8; padding: 15px; border-radius: 5px; margin: 20px 0; }
        .success { background-color: #e8f5e8; padding: 15px; border-radius: 5px; margin: 20px 0; }
        pre { background-color: #f5f5f5; padding: 15px; border-radius: 5px; overflow-x: auto; }
        .timestamp { color: #666; font-size: 0.9em; }
    </style>
</head>
<body>
    <div class="header">
        <h1>Video Player Test Report</h1>
        <p class="timestamp">Generated: $timestamp</p>
    </div>
    
    <div class="summary">
        <h2>Test Summary</h2>
        <p><strong>Total Tests:</strong> $total_tests</p>
        <p><strong>Passed:</strong> $passed_tests</p>
        <p><strong>Failed:</strong> $failed_tests</p>
        <p><strong>Success Rate:</strong> $(echo "scale=1; $passed_tests * 100 / $total_tests" | bc -l)%</p>
    </div>
    
    <h2>Test Output</h2>
    <pre>$(cat "$TEST_RESULTS")</pre>
    
    <h2>Test Configuration</h2>
    <ul>
        <li><strong>Test Directory:</strong> $TEST_DIR</li>
        <li><strong>Project Directory:</strong> $PROJECT_DIR</li>
        <li><strong>Test Executable:</strong> $TEST_EXECUTABLE</li>
        <li><strong>Results Log:</strong> $TEST_RESULTS</li>
    </ul>
</body>
</html>
EOF
    
    print_color $GREEN "✓ HTML report generated: $TEST_REPORT"
}

# Function to show test results
show_results() {
    print_header "Test Results"
    
    if [ -f "$TEST_RESULTS" ]; then
        cat "$TEST_RESULTS"
    else
        print_color $RED "No test results found"
    fi
}

# Function to clean up
cleanup() {
    print_header "Cleaning Up"
    
    cd "$TEST_DIR"
    make clean
    
    print_color $GREEN "✓ Cleanup complete"
}

# Function to show help
show_help() {
    echo "Video Player Test Runner"
    echo "======================="
    echo ""
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -h, --help     Show this help message"
    echo "  -c, --check    Check dependencies only"
    echo "  -b, --build    Build tests only"
    echo "  -r, --run      Run tests only"
    echo "  -g, --report   Generate HTML report"
    echo "  -s, --show     Show test results"
    echo "  -a, --all      Run all steps (default)"
    echo "  --clean        Clean up test files"
    echo ""
    echo "Examples:"
    echo "  $0              # Run all tests"
    echo "  $0 --check      # Check dependencies"
    echo "  $0 --build      # Build tests"
    echo "  $0 --run        # Run tests"
    echo "  $0 --report     # Generate report"
    echo "  $0 --clean      # Clean up"
}

# Main function
main() {
    local action="all"
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_help
                exit 0
                ;;
            -c|--check)
                action="check"
                shift
                ;;
            -b|--build)
                action="build"
                shift
                ;;
            -r|--run)
                action="run"
                shift
                ;;
            -g|--report)
                action="report"
                shift
                ;;
            -s|--show)
                action="show"
                shift
                ;;
            -a|--all)
                action="all"
                shift
                ;;
            --clean)
                action="clean"
                shift
                ;;
            *)
                print_color $RED "Unknown option: $1"
                show_help
                exit 1
                ;;
        esac
    done
    
    # Execute action
    case $action in
        check)
            check_dependencies
            ;;
        build)
            check_dependencies
            build_tests
            ;;
        run)
            check_dependencies
            build_tests
            run_tests
            ;;
        report)
            generate_report
            ;;
        show)
            show_results
            ;;
        clean)
            cleanup
            ;;
        all)
            check_dependencies
            build_tests
            if run_tests; then
                generate_report
                print_color $GREEN "🎉 All tests completed successfully!"
            else
                generate_report
                print_color $RED "❌ Some tests failed!"
                exit 1
            fi
            ;;
    esac
}

# Run main function with all arguments
main "$@"
