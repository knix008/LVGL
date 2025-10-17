# Chunjiin Input System Test Suite

This directory contains automated tests for the Chunjiin input system.

## How to Run All Tests

1. Make sure you have a C compiler (e.g., gcc) installed.
2. Run the provided script:

```bash
./run_test.sh
```

This will build and execute all test cases:
- `test_chunjiin` (core tests)
- `test_chunjiin_extra` (advanced and edge-case tests)

## Files
- `test_chunjiin.c`: Core Chunjiin input system tests
- `test_chunjiin_extra.c`: Advanced, fuzz, and boundary tests
- `Makefile`: Build rules for all test binaries
- `run_test.sh`: Script to build and run all tests

## Adding New Tests
Add new `.c` files and update the `Makefile` as needed. Ensure new tests are invoked in `run_test.sh` if required.

---

# Chunjiin Input System Test Cases

This file lists test cases for the Chunjiin input system. Each test case should be implemented in a corresponding C test file.

## Test Case Ideas

1. Initialization
   - State is zeroed after `chunjiin_init`
   - Mode is set to Hangul by default
2. Hangul Input
   - Single character input (e.g., ㄱ, ㅏ, ㄴ)
   - Compose a full syllable (e.g., ㄱ + ㅏ + ㄴ → 간)
   - Compose multiple syllables in sequence
   - Handle double consonants and vowels
   - Handle special characters (ㆍ, ㅡ, etc.)
3. English/Number/Special Mode
   - Switch to English mode and input letters
   - Switch to Number mode and input numbers
   - Switch to Special mode and input symbols
   - Mode cycling (Hangul → English → Number → Special → Hangul)
4. Buffer Handling
   - Input up to buffer limit (MAX_TEXT_LEN)
   - Input after buffer is full (should not overflow)
   - Clear buffer and check state
   - Enter key after buffer is cleared (should not crash)
   - Enter key with non-empty buffer
5. Cursor Handling
   - Cursor at start, middle, end
   - Delete character at various positions
   - Delete at buffer start (should not crash)
6. Edge Cases
   - Rapid mode switching
   - Rapid clear/enter presses
   - Invalid input values (negative, >11)
   - Null/empty input
7. Unicode/UTF-8 Conversion
   - Correct conversion for Hangul syllables
   - Correct conversion for English/numbers/specials
8. Integration
   - Simulate full user session: input, clear, enter, mode switch, etc.

---

Implement these as C unit tests, e.g. using Unity, CMocka, or a simple assert-based framework.