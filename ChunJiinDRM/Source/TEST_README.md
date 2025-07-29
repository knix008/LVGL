# ChunJiIn Input Test Suite

This test suite comprehensively tests the ChunJiIn input system, covering all choseong (initial consonants), jungseong (vowels), and jongseong (final consonants) combinations.

## Test Coverage

### 1. Choseong (Initial Consonants) Tests
Tests all 7 basic consonant groups:
- `g` → ㄱ
- `n` → ㄴ  
- `d` → ㄷ
- `b` → ㅂ
- `s` → ㅅ
- `j` → ㅈ
- `m` → ㅇ

### 2. Jungseong (Vowels) Tests
Tests all basic vowels:
- `h` → ㅏ
- `k` → ㅐ
- `l` → ㅑ
- `o` → ㅓ
- `p` → ㅔ
- `u` → ㅕ
- `i` → ㅣ

### 3. Compound Vowels Tests
Tests complex vowel combinations:
- `ho` → ㅘ (ㅗ + ㅏ)
- `hp` → ㅙ (ㅗ + ㅐ)
- `hi` → ㅚ (ㅗ + ㅣ)
- `uo` → ㅝ (ㅜ + ㅓ)
- `up` → ㅞ (ㅜ + ㅔ)
- `ui` → ㅟ (ㅜ + ㅣ)
- `li` → ㅢ (ㅡ + ㅣ)

### 4. Jongseong (Final Consonants) Tests
Tests final consonants in complete syllables:
- `ghg` → 각 (ㄱ + ㅏ + ㄱ)
- `nhg` → 낙 (ㄴ + ㅏ + ㄱ)
- `dhg` → 닥 (ㄷ + ㅏ + ㄱ)
- `bhg` → 박 (ㅂ + ㅏ + ㄱ)
- `shg` → 삭 (ㅅ + ㅏ + ㄱ)
- `jhg` → 작 (ㅈ + ㅏ + ㄱ)
- `mhg` → 악 (ㅇ + ㅏ + ㄱ)

### 5. Double Consonants Tests
Tests 쌍자음 (double consonants):
- `gg` → ㄲ
- `dd` → ㄸ
- `bb` → ㅃ
- `ss` → ㅆ
- `jj` → ㅉ

### 6. Consonant Group Toggling Tests
Tests the cycling behavior of consonant groups:
- `g` → `gg` → `ggg` (ㄱ → ㅋ → ㄲ)
- `d` → `dd` → `ddd` (ㄷ → ㅌ → ㄸ)
- `b` → `bb` → `bbb` (ㅂ → ㅍ → ㅃ)
- `s` → `ss` → `sss` (ㅅ → ㅎ → ㅆ)
- `j` → `jj` → `jjj` (ㅈ → ㅊ → ㅉ)

### 7. Complex Syllables Tests
Tests complete syllables with all components:
- `ghg` → 각
- `nhg` → 낙
- `dhg` → 닥
- `bhg` → 박
- `shg` → 삭
- `jhg` → 작
- `mhg` → 악

### 8. Vowel Combinations Tests
Tests various vowel combinations with consonants:
- `gh` → 가 (ㄱ + ㅏ)
- `gk` → 개 (ㄱ + ㅐ)
- `gl` → 갸 (ㄱ + ㅑ)
- `go` → 거 (ㄱ + ㅓ)
- `gp` → 게 (ㄱ + ㅔ)
- `gu` → 겨 (ㄱ + ㅕ)
- `gi` → 기 (ㄱ + ㅣ)

### 9. Special Functions Tests
Tests utility functions:
- Backspace functionality
- Space input
- Clear function
- Enter key handling

### 10. Edge Cases Tests
Tests boundary conditions:
- Empty input handling
- Invalid input handling
- Multiple consecutive consonants
- Buffer overflow protection

### 11. Complete Words Tests
Tests multi-syllable words:
- Simple syllables
- Repeated syllables
- Syllables with spaces
- Multiple syllables

## How to Run Tests

### Prerequisites
- GCC compiler
- UTF-8 locale support
- ChunJiIn input system files

### Build and Run
```bash
cd Source
./build_test.sh
```

### Manual Build
```bash
cd Source
gcc -Wall -Wextra -std=c99 -I./include -o test_chunjiin_input src/chunjiin_input.c src/test_chunjiin_input.c
./test_chunjiin_input
```

## Test Output Format

The test suite provides detailed output showing:
- ✓ PASS: for successful tests
- ✗ FAIL: for failed tests
- Test summary with statistics
- Success rate percentage

## Expected Results

All tests should pass if the ChunJiIn input system is working correctly. The test suite validates:

1. **Character Generation**: Correct Korean characters are produced from input sequences
2. **Combination Logic**: Vowels and consonants combine properly
3. **State Management**: Input state transitions work correctly
4. **Buffer Management**: Text buffer operations function properly
5. **Error Handling**: Invalid inputs are handled gracefully

## Troubleshooting

### Common Issues

1. **Locale Problems**: Ensure UTF-8 locale is set
   ```bash
   export LC_ALL=en_US.UTF-8
   ```

2. **Compilation Errors**: Check that all required files are present
   - `src/chunjiin_input.c`
   - `include/chunjiin_input.h`

3. **Test Failures**: Review the specific failing test to identify the issue

### Debug Mode

To run with additional debug information:
```bash
gcc -Wall -Wextra -std=c99 -DDEBUG -I./include -o test_chunjiin_input src/chunjiin_input.c src/test_chunjiin_input.c
```

## Test Maintenance

When adding new features to the ChunJiIn input system:

1. Add corresponding test cases to `test_chunjiin_input.c`
2. Update this documentation
3. Ensure all existing tests still pass
4. Run the full test suite before committing changes

## File Structure

```
Source/
├── src/
│   ├── chunjiin_input.c          # Main implementation (DO NOT MODIFY)
│   └── test_chunjiin_input.c     # Test suite
├── include/
│   └── chunjiin_input.h          # Header file (DO NOT MODIFY)
├── build_test.sh                 # Build script
└── TEST_README.md               # This documentation
``` 