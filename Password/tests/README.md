# Password Login Tests

This directory contains unit tests for the Password Login application.

## Test Files

### test_password_validation.c
Tests for password validation functionality:
- Valid password with all requirements
- Password too short (< 11 chars)
- Password without capital letter
- Password without number
- Password without special character
- Minimum valid password (11 chars)
- Empty and NULL passwords
- Maximum length password (32 chars)
- Password validation info structure

### test_userid_validation.c
Tests for User ID validation functionality:
- Valid username formats
- Valid email formats
- Email format detection
- User ID length validation (min 3, max 64 chars)
- Invalid characters detection
- Starting character validation
- Empty and NULL User IDs
- Dots and dashes support

## Running Tests

### Quick Run (Using Script - Recommended)
```bash
cd tests
./run_test.sh
```
The script will:
- Auto-build tests if needed
- Run all tests with colored output
- Show summary with pass/fail counts

### Build All Tests
```bash
cd tests
make
```

### Run All Tests (Using Make)
```bash
cd tests
make run
```

### Run Individual Tests
```bash
cd tests
./test_password_validation
./test_userid_validation
```

### Clean Test Artifacts
```bash
cd tests
make clean
```

### From Project Root
```bash
make test          # Run all tests
make build-tests   # Build tests only
make clean         # Clean main project AND tests
make clean-tests   # Clean tests only
```

## Test Output

Tests provide clear output with:
- ✅ PASSED - Test passed successfully
- ❌ FAILED - Test failed with details
- Summary showing passed/total tests

Example output:
```
=================================
PASSWORD VALIDATION TESTS
=================================

Running test: Valid password with all requirements
  ✓ Password should be valid
  ✅ PASSED

Running test: Password too short
  ✓ Password should be invalid (too short)
  ✅ PASSED

...

=================================
RESULTS: 10/10 tests passed
=================================
```

## Adding New Tests

1. Create a new test file in `tests/` directory
2. Include the test framework macros:
   ```c
   #define TEST(name)
   #define ASSERT(condition, message)
   #define RUN_TEST(test)
   ```
3. Write test functions returning `int` (1 = pass, 0 = fail)
4. Add test to `main()` using `RUN_TEST(test_name)`
5. Update `Makefile` to build the new test

## Requirements

- GCC compiler
- Make build system
- No external dependencies (tests use only standard C library)

## Test Coverage

Current test coverage:
- ✅ Password validation logic
- ✅ User ID validation logic
- ✅ Edge cases (NULL, empty, min/max lengths)
- ✅ Email format detection
- ✅ Special character validation

Future test additions:
- GUI interaction tests (would require LVGL test framework)
- Keypad input simulation tests
- Integration tests

