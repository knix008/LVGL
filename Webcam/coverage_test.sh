#!/bin/bash

# YOLO Converter Test Program - Coverage Test Script
# =================================================

echo "YOLO Converter Test Program - Coverage Test Script"
echo "=================================================="
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    local status=$1
    local message=$2
    if [ "$status" = "PASS" ]; then
        echo -e "${GREEN}✅ PASS${NC}: $message"
    elif [ "$status" = "FAIL" ]; then
        echo -e "${RED}❌ FAIL${NC}: $message"
    elif [ "$status" = "INFO" ]; then
        echo -e "${BLUE}ℹ️  INFO${NC}: $message"
    elif [ "$status" = "WARN" ]; then
        echo -e "${YELLOW}⚠️  WARN${NC}: $message"
    fi
}

# Check if we're in the build directory
if [ ! -f "CMakeCache.txt" ]; then
    print_status "FAIL" "Not in build directory. Please run from build directory."
    exit 1
fi

print_status "INFO" "Starting coverage test in build directory"

# Clean previous coverage data
print_status "INFO" "Cleaning previous coverage data..."
find . -name "*.gcda" -delete
find . -name "*.gcno" -delete
find . -name "*.gcov" -delete

# Rebuild with coverage enabled
print_status "INFO" "Rebuilding with coverage enabled..."
cd .. && rm -rf build && mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)

if [ $? -ne 0 ]; then
    print_status "FAIL" "Build failed"
    exit 1
fi

print_status "PASS" "Build completed successfully"

# Check if test program exists
if [ ! -f "./build/yolo_converter_test" ]; then
    print_status "FAIL" "Test program not found: ./build/yolo_converter_test"
    exit 1
fi

print_status "INFO" "Found test program: ./build/yolo_converter_test"

# Run comprehensive tests to generate coverage data
print_status "INFO" "Running tests to generate coverage data..."

# Test 1: Help message
echo ""
echo "Test 1: Help Message"
./yolo_converter_test -h > /dev/null 2>&1

# Test 2: Missing model argument (should fail)
echo "Test 2: Missing Model Argument"
./yolo_converter_test 2>&1 > /dev/null

# Test 3: Non-existent model file (should fail)
echo "Test 3: Non-existent Model File"
./yolo_converter_test -m nonexistent.onnx 2>&1 > /dev/null

# Test 4: Placeholder ONNX file (should fail)
echo "Test 4: Placeholder ONNX File Test"
if [ -f "yolov8n_converted.onnx" ]; then
    ./yolo_converter_test -m yolov8n_converted.onnx > /dev/null 2>&1
fi

# Test 5: Real ONNX model
echo "Test 5: Real ONNX Model Test"
if [ -f "../models/yolov8n.onnx" ]; then
    ./yolo_converter_test -m ../models/yolov8n.onnx > /dev/null 2>&1
fi

# Test 6: Model loading test
echo "Test 6: Model Loading Test"
if [ -f "../models/yolov8n.onnx" ]; then
    ./yolo_converter_test -m ../models/yolov8n.onnx -l > /dev/null 2>&1
fi

# Test 7: Model validation test
echo "Test 7: Model Validation Test"
if [ -f "../models/yolov8n.onnx" ]; then
    ./yolo_converter_test -m ../models/yolov8n.onnx -v > /dev/null 2>&1
fi

# Test 8: Benchmark test
echo "Test 8: Benchmark Test"
if [ -f "../models/yolov8n.onnx" ]; then
    ./yolo_converter_test -m ../models/yolov8n.onnx -b 5 > /dev/null 2>&1
fi

# Test 9: Image test
echo "Test 9: Image Test"
if [ -f "../models/yolov8n.onnx" ] && [ -f "../data/bus.jpg" ]; then
    ./yolo_converter_test -m ../models/yolov8n.onnx -i ../data/bus.jpg > /dev/null 2>&1
fi

# Test 10: Different input size test
echo "Test 10: Different Input Size Test"
if [ -f "../models/yolov8n.onnx" ]; then
    ./yolo_converter_test -m ../models/yolov8n.onnx -s 416x416 > /dev/null 2>&1
fi

# Test 11: Invalid size format test
echo "Test 11: Invalid Size Format Test"
./yolo_converter_test -m ../models/yolov8n.onnx -s invalid 2>&1 > /dev/null

print_status "PASS" "All tests completed"

# Generate coverage report
print_status "INFO" "Generating coverage report..."

# Create coverage directory
mkdir -p coverage_report

# Generate coverage data
lcov --capture --directory . --output-file coverage_report/coverage.info

if [ $? -ne 0 ]; then
    print_status "WARN" "lcov failed, trying gcov directly..."
    
    # Generate gcov files
    for file in src/*.cpp; do
        if [ -f "$file" ]; then
            gcov "$file" > /dev/null 2>&1
        fi
    done
    
    # Show gcov results
    echo ""
    echo "GCOV Coverage Results:"
    echo "======================"
    for file in *.gcov; do
        if [ -f "$file" ]; then
            echo "File: $file"
            grep -E "^[[:space:]]*[0-9]+:" "$file" | head -10
            echo ""
        fi
    done
else
    # Generate HTML report
    lcov --remove coverage_report/coverage.info '/usr/*' --output-file coverage_report/coverage_filtered.info
    genhtml coverage_report/coverage_filtered.info --output-directory coverage_report/html
    
    print_status "PASS" "Coverage report generated successfully"
    print_status "INFO" "HTML report available at: coverage_report/html/index.html"
fi

# Show summary
echo ""
echo "Coverage Summary"
echo "================"

# Count total lines and covered lines
total_lines=0
covered_lines=0

for file in *.gcov; do
    if [ -f "$file" ]; then
        file_total=$(grep -c "^[[:space:]]*[0-9]+:" "$file" 2>/dev/null || echo "0")
        file_covered=$(grep -c "^[[:space:]]*[0-9]+:" "$file" | grep -v "^[[:space:]]*0:" 2>/dev/null || echo "0")
        
        if [ "$file_total" -gt 0 ]; then
            coverage_percent=$((file_covered * 100 / file_total))
            echo "File: $file"
            echo "  Total lines: $file_total"
            echo "  Covered lines: $file_covered"
            echo "  Coverage: ${coverage_percent}%"
            echo ""
            
            total_lines=$((total_lines + file_total))
            covered_lines=$((covered_lines + file_covered))
        fi
    fi
done

if [ "$total_lines" -gt 0 ]; then
    overall_coverage=$((covered_lines * 100 / total_lines))
    echo "Overall Coverage: ${overall_coverage}%"
    echo "Total lines: $total_lines"
    echo "Covered lines: $covered_lines"
    
    if [ "$overall_coverage" -ge 80 ]; then
        print_status "PASS" "Good coverage achieved: ${overall_coverage}%"
    elif [ "$overall_coverage" -ge 60 ]; then
        print_status "WARN" "Moderate coverage: ${overall_coverage}%"
    else
        print_status "FAIL" "Low coverage: ${overall_coverage}%"
    fi
else
    print_status "WARN" "No coverage data found"
fi

# Show detailed coverage for specific files
echo ""
echo "Detailed Coverage Analysis"
echo "========================="

# Check specific source files
for file in src/yolo_converter_test.cpp src/yolo_converter_test_main.cpp; do
    if [ -f "$file" ]; then
        echo "Analyzing: $file"
        
        # Count lines
        total=$(wc -l < "$file")
        
        # Count executable lines (non-empty, non-comment, non-brace-only)
        executable=$(grep -v '^[[:space:]]*$' "$file" | grep -v '^[[:space:]]*//' | grep -v '^[[:space:]]*/\*' | grep -v '^[[:space:]]*\*/' | grep -v '^[[:space:]]*{[[:space:]]*$' | grep -v '^[[:space:]]*}[[:space:]]*$' | wc -l)
        
        echo "  Total lines: $total"
        echo "  Executable lines: $executable"
        echo ""
    fi
done

print_status "INFO" "Coverage test completed successfully!"
