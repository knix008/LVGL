#!/bin/bash

# Test Webcam Viewer with No Webcam
# 웹캠이 없을 때 웹캠 뷰어 테스트

echo "🧪 Testing Webcam Viewer with No Webcam"
echo "======================================"
echo ""

# Check if webcam devices exist
echo "📹 Checking webcam devices..."
if ls /dev/video* 2>/dev/null; then
    echo "⚠️  Webcam devices found. This test is for no webcam scenario."
    echo "   To test properly, disconnect webcam or use different camera index."
    echo ""
else
    echo "✅ No webcam devices found. Perfect for testing error handling."
    echo ""
fi

# Check if webcam_viewer exists
if [ ! -f "./webcam_viewer" ]; then
    echo "❌ Error: webcam_viewer executable not found!"
    echo "Please build the project first:"
    echo "  cmake .. && make -j\$(nproc)"
    exit 1
fi

echo "✅ webcam_viewer found!"
echo ""

# Test 1: Try to use camera index 999 (non-existent)
echo "🧪 Test 1: Using non-existent camera index 999"
echo "Expected: Graceful error message, no core dump"
echo "Command: ./webcam_viewer -c 999"
echo ""
./webcam_viewer -c 999
echo ""
echo "Test 1 completed."
echo ""

# Test 2: Try to use camera index 5 (likely non-existent)
echo "🧪 Test 2: Using camera index 5"
echo "Expected: Graceful error message, no core dump"
echo "Command: ./webcam_viewer -c 5"
echo ""
./webcam_viewer -c 5
echo ""
echo "Test 2 completed."
echo ""

# Test 3: Try to use camera index 10 (likely non-existent)
echo "🧪 Test 3: Using camera index 10"
echo "Expected: Graceful error message, no core dump"
echo "Command: ./webcam_viewer -c 10"
echo ""
./webcam_viewer -c 10
echo ""
echo "Test 3 completed."
echo ""

# Test 4: Try simple webcam viewer with non-existent camera
echo "🧪 Test 4: Simple webcam viewer with non-existent camera"
echo "Expected: Graceful error message, no core dump"
echo "Command: ./webcam_viewer_simple -s 999"
echo ""
./webcam_viewer_simple -s 999
echo ""
echo "Test 4 completed."
echo ""

echo "🎉 All tests completed!"
echo "If no core dumps occurred, the error handling is working correctly."
echo ""
