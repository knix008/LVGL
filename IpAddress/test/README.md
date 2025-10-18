# IP Address Validation Test Suite

This directory contains comprehensive test cases for the IP address validation functionality.

## 📁 Test Structure

```
test/
├── test_ipv4.c          # IPv4 validation tests
├── test_ipv6.c          # IPv6 validation tests
├── test_all.c           # Comprehensive test suite
├── Makefile             # Build configuration
├── run_tests.sh         # Test runner script
└── README.md            # This file
```

## 🚀 Quick Start

### Run All Tests
```bash
cd test
./run_tests.sh
```

### Run Specific Tests
```bash
# IPv4 tests only
./run_tests.sh ipv4

# IPv6 tests only
./run_tests.sh ipv6

# Comprehensive tests only
./run_tests.sh comprehensive
```

### Manual Build and Run
```bash
# Build all tests
make all

# Run individual tests
make test-ipv4
make test-ipv6
make test-comprehensive

# Clean up
make clean
```

## 📋 Test Coverage

### IPv4 Tests (`test_ipv4.c`)
- ✅ Valid IPv4 addresses (192.168.1.1, 10.0.0.1, etc.)
- ❌ Invalid IPv4 addresses (empty, too many octets, invalid ranges)
- 🔍 Edge cases (0.0.0.0, 255.255.255.255, leading zeros)
- 🛠️ Octet parsing functions

### IPv6 Tests (`test_ipv6.c`)
- ✅ Valid IPv6 addresses (full notation, compressed notation)
- ❌ Invalid IPv6 addresses (too many groups, invalid hex, etc.)
- 🔍 Compressed notation (::, ::1, 2001:db8::1)
- 🛠️ Hex parsing functions

### Comprehensive Tests (`test_all.c`)
- 🔄 Combined IPv4 and IPv6 validation
- 🎯 Edge cases and boundary conditions
- 🛠️ Parse function testing
- 📝 Format function testing

## 🧪 Test Categories

### 1. **Valid Address Tests**
- Standard valid IP addresses
- Edge cases (0.0.0.0, 255.255.255.255)
- Compressed IPv6 notation
- Mixed case IPv6 addresses

### 2. **Invalid Address Tests**
- Empty strings
- Wrong number of octets/groups
- Out-of-range values
- Invalid characters
- Malformed syntax

### 3. **Edge Case Tests**
- Leading zeros
- Trailing/leading spaces
- Multiple separators
- Boundary values

### 4. **Function Tests**
- `ipv4_parse_octet()` - IPv4 octet parsing
- `ipv6_parse_hex()` - IPv6 hex parsing
- `ip_format_from_fields()` - Address formatting

## 📊 Test Results

Each test run provides:
- ✅ Pass/fail status for each test
- 📈 Success rate percentage
- 📋 Total test count
- 🎯 Detailed error messages

## 🔧 Build Requirements

- GCC compiler
- C99 standard support
- Access to `../ip_address.c` and `../ip_address.h`

## 🚨 Troubleshooting

### Build Errors
```bash
# Clean and rebuild
make clean
make all
```

### Permission Issues
```bash
# Make script executable
chmod +x run_tests.sh
```

### Missing Dependencies
Ensure the parent directory contains:
- `ip_address.c` - IP validation implementation
- `ip_address.h` - IP validation header

## 📈 Adding New Tests

To add new test cases:

1. **Edit existing test files** to add new test cases
2. **Create new test files** following the naming pattern `test_*.c`
3. **Update Makefile** to include new test executables
4. **Run tests** to verify new cases

### Example Test Case
```c
void test_new_feature(void) {
    printf("\n=== Testing New Feature ===\n");
    
    TEST_ASSERT(ipv4_validate("192.168.1.1"), "Basic IPv4 should be valid");
    TEST_ASSERT(!ipv4_validate("invalid"), "Invalid input should fail");
}
```

## 🎯 Test Philosophy

- **Comprehensive Coverage**: Test both valid and invalid cases
- **Edge Case Focus**: Test boundary conditions and edge cases
- **Clear Output**: Provide readable test results with pass/fail indicators
- **Modular Design**: Separate test files for different functionality
- **Easy Execution**: Simple commands to run all or specific tests

## 📝 Notes

- Tests use a custom `TEST_ASSERT` macro for consistent output
- All tests are self-contained and don't require external dependencies
- Test results include colored output for better readability
- The test suite is designed to be run in CI/CD pipelines
