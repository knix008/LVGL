# Korean Keypad Test Program

This directory contains a separate build system for the Korean Keypad Test program (`koreankeypadtest`).

## Overview

The `koreankeypadtest` program is a standalone test application for Korean Hangul input using a two-set keyboard layout. It demonstrates:

- Korean character composition from jamo (consonants and vowels)
- Two-set keyboard input processing
- Unicode Hangul syllable generation
- UTF-8 encoding

## Source Files

- `Source/koreankeypadtest/korean_test.c` - Main test program with interactive input
- `Source/koreankeypadtest/korean_hangul_standalone.c` - Korean Hangul processing functions (standalone version)
- `Source/koreankeypadtest/korean_common.h` - Common header file for Korean Hangul definitions
- `Source/koreankeypadtest/korean_hangul.h` - Original header file (LVGL-dependent)

## Building the Program

### Option 1: Using Makefile

```bash
# Build the program
make

# Clean build artifacts
make clean

# Build and run
make run

# Show available targets
make help
```

### Option 2: Using Build Script

```bash
# Build the program
./build.sh
```

### Option 3: Manual Compilation

```bash
gcc -Wall -Wextra -std=c99 -O2 \
    Source/koreankeypadtest/korean_test.c \
    Source/koreankeypadtest/korean_hangul.c \
    -o koreankeypadtest
```

## Running the Program

After building, run the program:

```bash
./koreankeypadtest
```

## Program Features

The test program includes several test functions:

1. **Jamo Composition Test** - Tests basic Korean syllable composition
2. **Complex Syllable Test** - Tests multi-syllable Korean words
3. **Interactive Input Test** - Allows manual input of two-set keyboard sequences

### Example Input

The program accepts two-set keyboard input sequences. For example:
- `rk` → 가 (ga)
- `rkskekfk` → 안녕하세요 (annyeonghaseyo)
- `dkssud` → 안녕 (annyeong)

### Two-Set Keyboard Layout

The program uses a simplified two-set keyboard mapping:

**Consonants (Choseong/Jongseong):**
- r → ㄱ, R → ㄲ
- s → ㄴ
- e → ㄷ, E → ㄸ
- f → ㄹ
- a → ㅁ
- q → ㅂ, Q → ㅃ
- t → ㅅ, T → ㅆ
- d → ㅇ
- w → ㅈ, W → ㅉ
- c → ㅊ
- z → ㅋ
- x → ㅌ
- v → ㅍ
- g → ㅎ

**Vowels (Jungseong):**
- k → ㅏ
- o → ㅐ, O → ㅒ
- i → ㅑ
- j → ㅓ
- p → ㅔ, P → ㅖ
- u → ㅕ
- h → ㅗ
- y → ㅛ
- n → ㅜ
- b → ㅠ
- m → ㅡ
- l → ㅣ, L → ㅢ

## Requirements

- GCC compiler
- Standard C library
- UTF-8 terminal support (for proper Korean character display)

## Installation (Optional)

To install the program system-wide:

```bash
make install
```

To uninstall:

```bash
make uninstall
```

## Troubleshooting

1. **Korean characters not displaying properly**: Ensure your terminal supports UTF-8 encoding
2. **Compilation errors**: Make sure you have GCC installed
3. **Permission denied**: Make sure the build script is executable (`chmod +x build.sh`)

## Notes

- This is a standalone test program and does not require LVGL or other dependencies
- The program uses `korean_hangul_standalone.c` which is a simplified version without LVGL dependencies
- The original `korean_hangul.c` contains LVGL-specific code and is not used in this build
- The program is designed for testing Korean input algorithms
- All Korean character processing is done using Unicode standards 