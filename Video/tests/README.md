# Video Player Test Suite

This directory contains comprehensive test cases for the LVGL Video Player application.

## Overview

The test suite includes:
- **Unit Tests**: Individual component testing
- **Integration Tests**: End-to-end functionality testing
- **Performance Tests**: Stress testing and timing validation
- **Edge Case Tests**: Boundary condition testing
- **State Transition Tests**: Playback control validation

## Test Structure

```
tests/
├── test_video_player.c    # Main test suite
├── test_config.h          # Test configuration and macros
├── Makefile              # Build configuration
├── run_tests.sh          # Test runner script
└── README.md             # This file
```

## Test Categories

### 1. Initialization Tests
- Video player initialization
- State validation
- Default values verification

### 2. File Loading Tests
- Valid file loading
- Invalid file handling
- Path resolution
- File format support

### 3. Playback Control Tests
- Play functionality
- Pause functionality
- Stop functionality
- Resume functionality
- State transitions

### 4. Volume Control Tests
- Volume setting
- Volume clamping
- Volume persistence

### 5. Seek Tests
- Position seeking
- Boundary conditions
- Invalid positions

### 6. State Transition Tests
- Valid state changes
- Invalid state changes
- State persistence

### 7. Edge Case Tests
- Operations without loaded file
- Multiple consecutive operations
- Rapid state changes

### 8. Performance Tests
- Rapid state changes
- Memory usage
- Timing validation

## Running Tests

### Quick Start
```bash
# Run all tests
./run_tests.sh

# Check dependencies only
./run_tests.sh --check

# Build tests only
./run_tests.sh --build

# Run tests only
./run_tests.sh --run

# Generate HTML report
./run_tests.sh --report

# Clean up
./run_tests.sh --clean
```

### Manual Build and Run
```bash
# Build tests
make

# Run tests
make test

# Run with verbose output
make test-verbose

# Clean up
make clean
```

## Test Dependencies

### Required
- **GCC**: C compiler
- **Make**: Build system
- **SDL2**: Graphics and audio library
- **FFmpeg**: Video/audio processing library
- **LVGL**: Graphics library

### Optional
- **FFmpeg**: For test video generation (if not available, some tests will be skipped)

## Test Configuration

The test suite can be configured through `test_config.h`:

```c
// Test video settings
#define TEST_VIDEO_DURATION_SECONDS 5
#define TEST_VIDEO_WIDTH 320
#define TEST_VIDEO_HEIGHT 240

// Test timeouts
#define TEST_TIMEOUT_SHORT 100
#define TEST_TIMEOUT_MEDIUM 1000
#define TEST_TIMEOUT_LONG 5000

// Test volume levels
#define TEST_VOLUME_MIN 0
#define TEST_VOLUME_MAX 100
#define TEST_VOLUME_DEFAULT 50
```

## Test Output

### Console Output
```
========================================
    Video Player Test Suite
========================================

=== Testing Video Player Initialization ===
✓ PASS: Initial state should be STOPPED
✓ PASS: Initial position should be 0
✓ PASS: Initial duration should be 0
✓ PASS: Initial volume should be 50
✓ PASS: Initial file should be NULL

=== Testing Video Player File Loading ===
✓ PASS: State should remain STOPPED for non-existent file
✓ PASS: Current file should be set correctly
✓ PASS: State should be STOPPED after loading

========================================
    Test Results Summary
========================================
Total Tests: 25
Passed: 25
Failed: 0
Success Rate: 100.0%

🎉 All tests passed!
```

### HTML Report
The test runner generates an HTML report with:
- Test summary
- Detailed results
- Configuration information
- Timestamps

## Adding New Tests

### 1. Create Test Function
```c
static void test_new_functionality(void) {
    printf("\n=== Testing New Functionality ===\n");
    
    // Test setup
    video_player_init();
    
    // Test cases
    TEST_ASSERT_EQUAL(PLAYER_STOPPED, video_player_get_state(), "Initial state");
    
    // Test cleanup
    video_player_cleanup();
}
```

### 2. Add to Main Function
```c
int main(void) {
    // ... existing tests ...
    
    test_new_functionality();
    
    // ... rest of main function ...
}
```

### 3. Update Test Count
Update the `total_tests` counter if needed.

## Test Assertions

The test suite provides various assertion macros:

```c
// Basic assertions
TEST_ASSERT(condition, message)
TEST_ASSERT_EQUAL(expected, actual, message)
TEST_ASSERT_NOT_NULL(ptr, message)
TEST_ASSERT_NULL(ptr, message)
TEST_ASSERT_STR_EQUAL(expected, actual, message)

// State assertions
TEST_ASSERT_EQUAL(PLAYER_STOPPED, video_player_get_state(), "State check")
TEST_ASSERT_EQUAL(0, video_player_get_position(), "Position check")
TEST_ASSERT_EQUAL(50, video_player_get_volume(), "Volume check")
```

## Troubleshooting

### Common Issues

1. **Build Failures**
   - Check dependencies: `./run_tests.sh --check`
   - Install missing libraries
   - Verify LVGL build

2. **Test Failures**
   - Check test video file generation
   - Verify file permissions
   - Check system audio configuration

3. **Performance Issues**
   - Adjust test timeouts
   - Reduce test iterations
   - Check system resources

### Debug Mode
```bash
# Run with debug output
make test-verbose

# Check test logs
cat test_results.log
```

## Contributing

When adding new tests:
1. Follow the existing naming conventions
2. Add proper documentation
3. Include edge cases
4. Update this README if needed
5. Test on different systems

## License

This test suite is part of the LVGL Video Player project and follows the same license terms.
