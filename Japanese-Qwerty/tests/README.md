# Japanese QWERTY Input Method - Test Suite

This directory contains unit tests for the Japanese QWERTY Input Method Editor.

## Test Files

### 1. `test_japanese_qwerty.c`
Tests the core IME functionality:
- IME initialization
- Mode switching (Hiragana, Katakana, English)
- Romaji to Kana conversion
- Input buffer management
- Backspace, Space, Enter operations
- Display text generation
- Buffer overflow protection
- Special character handling (small tsu, n/nn conversion)

### 2. `test_dakuten.c`
Tests the dakuten and handakuten combining marks:
- Dakuten conversion table (゛) - か→が, さ→ざ, etc.
- Handakuten conversion table (゜) - は→ぱ, ひ→ぴ, etc.
- Lookup functionality
- Complete character set coverage

## Running Tests

### Run All Tests
```bash
cd tests
make
```

### Run Individual Test Suites
```bash
# Japanese QWERTY core tests
make run-qwerty

# Dakuten combining tests
make run-dakuten
```

### Clean Test Builds
```bash
make clean
```

## Test Output

Tests provide detailed output with:
- ✓ PASS markers for successful tests
- ✗ FAIL markers for failed tests
- Expected vs actual values for failures
- Summary statistics at the end

Example output:
```
================================================
  Japanese QWERTY Input Method - Test Suite
================================================

=== Testing: IME Initialization ===
✓ PASS: Buffer position initialized to 0
✓ PASS: Output position initialized to 0
✓ PASS: Display position initialized to 0
...

================================================
  Test Results
================================================
Total tests run: 95
✓ Passed: 95
✗ Failed: 0

🎉 All tests passed!
```

## Test Coverage

The test suite covers:

1. **Initialization** - Proper state setup
2. **Mode Management** - Hiragana/Katakana/English switching
3. **Conversion Logic** - Romaji to Kana mappings
4. **Input Processing** - Character, backspace, space, enter
5. **Buffer Safety** - Overflow protection
6. **Edge Cases** - Long input, special characters
7. **Combining Marks** - Dakuten and handakuten
8. **Display Output** - Text generation and formatting

## Adding New Tests

To add new tests:

1. Create a new test function:
```c
void test_new_feature(void) {
    TEST_START("New Feature");
    
    IMEState state;
    ime_init(&state);
    
    // Your test code here
    TEST_ASSERT(condition, "Test description");
}
```

2. Call it from `main()`:
```c
int main(void) {
    test_new_feature();
    // ... other tests
}
```

3. Rebuild and run:
```bash
make clean
make
```

## Continuous Integration

These tests can be integrated into CI/CD pipelines:

```bash
# Run tests and check exit code
cd tests
make
if [ $? -eq 0 ]; then
    echo "All tests passed!"
else
    echo "Tests failed!"
    exit 1
fi
```

## Test Dependencies

The tests have minimal dependencies:
- GCC compiler
- Standard C library
- Japanese QWERTY core module (`japanese_qwerty.c`)

No LVGL or GUI dependencies required for core logic tests.

