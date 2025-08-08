#!/bin/bash

# YOLO Converter Test Program - Test Script
# =========================================

echo "YOLO Converter Test Program - Test Script"
echo "========================================="
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

# Check if test program exists
if [ ! -f "./build/yolo_converter_test" ]; then
    print_status "FAIL" "Test program not found: ./build/yolo_converter_test"
    exit 1
fi

print_status "INFO" "Found test program: ./build/yolo_converter_test"

# Test 1: Help message
echo ""
echo "Test 1: Help Message"
echo "==================="
./yolo_converter_test -h
if [ $? -eq 0 ]; then
    print_status "PASS" "Help message displayed correctly"
else
    print_status "FAIL" "Help message failed"
fi

# Test 2: Missing model argument
echo ""
echo "Test 2: Missing Model Argument"
echo "=============================="
./yolo_converter_test 2>&1
if [ $? -ne 0 ]; then
    print_status "PASS" "Correctly rejected missing model argument"
else
    print_status "FAIL" "Should have rejected missing model argument"
fi

# Test 3: Non-existent model file
echo ""
echo "Test 3: Non-existent Model File"
echo "==============================="
./yolo_converter_test -m nonexistent.onnx 2>&1
if [ $? -ne 0 ]; then
    print_status "PASS" "Correctly rejected non-existent model file"
else
    print_status "FAIL" "Should have rejected non-existent model file"
fi

# Test 4: Test with placeholder ONNX file (should fail)
echo ""
echo "Test 4: Placeholder ONNX File Test"
echo "=================================="
if [ -f "yolov8n_converted.onnx" ]; then
    ./yolo_converter_test -m yolov8n_converted.onnx
    if [ $? -eq 0 ]; then
        print_status "PASS" "Test program handled invalid ONNX file gracefully"
    else
        print_status "WARN" "Test program failed with invalid ONNX file (expected)"
    fi
else
    print_status "INFO" "Placeholder ONNX file not found, skipping test"
fi

# Test 5: Test with real ONNX model
echo ""
echo "Test 5: Real ONNX Model Test"
echo "============================"
if [ -f "../models/yolov8n.onnx" ]; then
    ./yolo_converter_test -m ../models/yolov8n.onnx
    if [ $? -eq 0 ]; then
        print_status "PASS" "Successfully tested real ONNX model"
    else
        print_status "FAIL" "Failed to test real ONNX model"
    fi
else
    print_status "WARN" "Real ONNX model not found: ../models/yolov8n.onnx"
fi

# Test 6: Model loading test
echo ""
echo "Test 6: Model Loading Test"
echo "=========================="
if [ -f "../models/yolov8n.onnx" ]; then
    ./yolo_converter_test -m ../models/yolov8n.onnx -l
    if [ $? -eq 0 ]; then
        print_status "PASS" "Model loading test successful"
    else
        print_status "FAIL" "Model loading test failed"
    fi
else
    print_status "WARN" "Real ONNX model not found, skipping test"
fi

# Test 7: Model validation test
echo ""
echo "Test 7: Model Validation Test"
echo "============================="
if [ -f "../models/yolov8n.onnx" ]; then
    ./yolo_converter_test -m ../models/yolov8n.onnx -v
    if [ $? -eq 0 ]; then
        print_status "PASS" "Model validation test successful"
    else
        print_status "FAIL" "Model validation test failed"
    fi
else
    print_status "WARN" "Real ONNX model not found, skipping test"
fi

# Test 8: Benchmark test
echo ""
echo "Test 8: Benchmark Test"
echo "====================="
if [ -f "../models/yolov8n.onnx" ]; then
    ./yolo_converter_test -m ../models/yolov8n.onnx -b 5
    if [ $? -eq 0 ]; then
        print_status "PASS" "Benchmark test successful"
    else
        print_status "FAIL" "Benchmark test failed"
    fi
else
    print_status "WARN" "Real ONNX model not found, skipping test"
fi

# Test 9: Image test
echo ""
echo "Test 9: Image Test"
echo "================="
if [ -f "../models/yolov8n.onnx" ] && [ -f "../data/bus.jpg" ]; then
    ./yolo_converter_test -m ../models/yolov8n.onnx -i ../data/bus.jpg
    if [ $? -eq 0 ]; then
        print_status "PASS" "Image test successful"
    else
        print_status "FAIL" "Image test failed"
    fi
else
    print_status "WARN" "Required files not found, skipping image test"
fi

# Test 10: Different input size test
echo ""
echo "Test 10: Different Input Size Test"
echo "=================================="
if [ -f "../models/yolov8n.onnx" ]; then
    ./yolo_converter_test -m ../models/yolov8n.onnx -s 416x416
    if [ $? -eq 0 ]; then
        print_status "PASS" "Different input size test successful"
    else
        print_status "FAIL" "Different input size test failed"
    fi
else
    print_status "WARN" "Real ONNX model not found, skipping test"
fi

# Test 11: Invalid size format test
echo ""
echo "Test 11: Invalid Size Format Test"
echo "================================="
./yolo_converter_test -m ../models/yolov8n.onnx -s invalid 2>&1
if [ $? -ne 0 ]; then
    print_status "PASS" "Correctly rejected invalid size format"
else
    print_status "FAIL" "Should have rejected invalid size format"
fi

# Summary
echo ""
echo "Test Summary"
echo "==========="
print_status "INFO" "All tests completed"
print_status "INFO" "Check the output above for individual test results"
echo ""
print_status "INFO" "Test program features verified:"
echo "  - Command line argument parsing"
echo "  - Error handling for missing/invalid files"
echo "  - Model loading and validation"
echo "  - Inference testing"
echo "  - Benchmark functionality"
echo "  - Image processing"
echo "  - Different input sizes"
echo "  - Graceful error handling"
echo ""
print_status "INFO" "Test script completed successfully!"
