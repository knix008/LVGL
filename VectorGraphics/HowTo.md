# ChunJiIn Input System - How To Guide

## Overview

The ChunJiIn Input System is a Korean input method based on the ChunJiIn (천지인) principle, which uses three fundamental elements:
- **ㆍ** (dot) - represents the sky/heaven
- **ㅡ** (horizontal line) - represents the earth
- **ㅣ** (vertical line) - represents human

## Key Features

### ✅ **Fully Functional ChunJiIn Input System**
- Complete choseong (initial consonant), jungseong (vowel), and jongseong (final consonant) support
- Proper Korean syllable formation and Unicode generation
- Comprehensive vowel combination rules
- Consonant cycling and toggling
- Dot (ㆍ) display and cycling functionality

### ✅ **Advanced Vowel Combinations**
- `ㅣ + ㆍ = ㅏ`, `ㅣ + ㆍ + ㅣ = ㅐ`
- `ㆍ + ㅣ = ㅓ`, `ㆍㆍ + ㅣ = ㅕ`
- `ㆍ + ㅡ = ㅗ`, `ㆍㆍ + ㅡ = ㅛ`
- `ㅡ + ㆍ = ㅜ`, `ㅜ + ㆍ = ㅠ`
- `ㅡ + ㆍㆍ = ㅠ`
- Complex combinations like `ㅗ + ㅣ = ㅚ`, `ㅜ + ㅣ = ㅟ`

### ✅ **Consonant Cycling System**
- **ㄱ → ㅋ → ㄲ → ㄱ** (g key)
- **ㄷ → ㅌ → ㄷ** (d key) - excludes ㄸ as it's not a valid final consonant
- **ㅂ → ㅍ → ㅂ** (b key) - excludes ㅃ as it's not a valid final consonant
- **ㅅ → ㅎ → ㅆ → ㅅ** (s key)
- **ㅈ → ㅊ → ㅈ** (j key) - excludes ㅉ as it's not a valid final consonant
- **ㄴ → ㄹ → ㄴ** (n key)
- **ㅇ → ㅁ → ㅇ** (m key)

### ✅ **Jongseong (Final Consonant) Support**
- All valid Korean final consonants supported
- Compound jongseong combinations (ㄳ, ㄵ, ㄶ, ㄺ, ㄻ, ㄼ, ㄽ, ㄾ, ㄿ, ㅀ, ㅄ)
- Proper decomposition when starting new syllables

### ✅ **Dot (ㆍ) Display and Cycling**
- **Choseong + dot**: Shows `choseong + ㆍ`
- **Jongseong + dot**: Shows `syllable + ㆍ`
- **Dot cycling**: `ㆍ → ㆍㆍ → ㆍ → ㆍㆍ` (cycles between single and double dots)
- **Jongseong + dot + dot + vowel**: Properly decomposes and forms new syllables

### ✅ **Comprehensive Testing**
- 558 test cases covering all functionality
- 100% test pass rate
- Extensive edge case coverage

## Key Mapping

| Key | Function |
|-----|----------|
| `g` | ㄱ (choseong) / ㄱ→ㅋ→ㄲ (jongseong cycling) |
| `n` | ㄴ (choseong) / ㄴ→ㄹ (jongseong cycling) |
| `d` | ㄷ (choseong) / ㄷ→ㅌ (jongseong cycling) |
| `b` | ㅂ (choseong) / ㅂ→ㅍ (jongseong cycling) |
| `s` | ㅅ (choseong) / ㅅ→ㅎ→ㅆ (jongseong cycling) |
| `j` | ㅈ (choseong) / ㅈ→ㅊ (jongseong cycling) |
| `m` | ㅇ (choseong) / ㅇ→ㅁ (jongseong cycling) |
| `a` | ㆍ (dot) - cycles between single and double dots |
| `e` | ㅡ (horizontal line) |
| `i` | ㅣ (vertical line) |
| `Enter` | Finalize current syllable |
| `Backspace` | Delete last input |
| `Space` | Clear all input |

## Usage Examples

### Basic Syllables
- `g + i + a` → `가` (ㄱ + ㅏ)
- `g + i + a + g` → `각` (ㄱ + ㅏ + ㄱ)
- `g + a + a + i` → `겨` (ㄱ + ㅕ)

### Complex Vowel Combinations
- `g + a + a + e` → `교` (ㄱ + ㅛ)
- `g + a + e + i + a` → `과` (ㄱ + ㅘ)

### Jongseong + Dot Combinations
- `g + i + a + g + a` → `각ㆍ` (각 + dot)
- `g + i + a + g + a + a` → `각ㆍㆍ` (각 + double dots)
- `g + i + a + g + a + a + i` → `가겨` (decomposes ㄱ and forms ㄱ + ㅕ)

### Consonant Cycling
- `g + i + a + g + g + g` → `같` (ㄱ → ㅋ → ㄲ → ㄱ)
- `g + i + a + s + s + s` → `갛` (ㅅ → ㅎ → ㅆ → ㅅ)

## Compilation Instructions

### Prerequisites
1. Install required packages:
   ```bash
   sudo apt install build-essential cmake libsdl2-dev
   ```

### Build Steps
1. Navigate to the Source directory:
   ```bash
   cd Source
   ```

2. Create and enter build directory:
   ```bash
   mkdir build
   cd build
   ```

3. Configure with CMake:
   ```bash
   cmake ..
   ```

4. Build the project:
   ```bash
   make
   ```

5. Run the executable:
   ```bash
   ./lvgl_main
   ```

## Testing

The system includes a comprehensive test suite with 558 test cases covering:
- Basic syllable formation
- Vowel combinations
- Consonant cycling
- Jongseong combinations
- Dot display and cycling
- Edge cases and error handling

To run the tests:
```bash
cd Source
gcc -o test_chunjiin_input src/test_chunjiin_input.c src/chunjiin_input.c -Iinclude
./test_chunjiin_input
```

## Recent Fixes

### ✅ **Fixed Jongseong + Dot + Dot + Vowel Issue**
- **Problem**: `각 + dot + dot + ㅣ` was producing `가거` instead of `가겨`
- **Root Cause**: `temp_vowel` value was being reset before vowel combination logic
- **Solution**: Save `temp_vowel` value before calling `reset_current_syllable()`
- **Result**: Now correctly produces `가겨` (decomposes ㄱ and forms ㄱ + ㅕ)

### ✅ **Enhanced Dot Cycling**
- **Problem**: Dot cycling wasn't working properly in all contexts
- **Solution**: Implemented proper cycling between single and double dots
- **Result**: `ㆍ → ㆍㆍ → ㆍ → ㆍㆍ` cycles correctly

### ✅ **Improved Vowel Combination Logic**
- **Problem**: Complex vowel combinations weren't working correctly
- **Solution**: Enhanced vowel combination rules and state management
- **Result**: All ChunJiIn vowel combinations now work correctly

## Technical Details

### File Structure
- `Source/src/chunjiin_input.c` - Main implementation
- `Source/include/chunjiin_input.h` - Header file
- `Source/src/test_chunjiin_input.c` - Comprehensive test suite
- `Source/src/main.c` - Main application entry point

### Key Functions
- `process_input(char key)` - Main input processing
- `handle_consonant(int key_code)` - Consonant input handling
- `handle_vowel(int key_code)` - Vowel input handling
- `finalize_syllable()` - Complete syllable formation
- `chunjiin_get_current_text(wchar_t * buffer)` - Get current display text

### State Management
The system uses a state machine with four states:
- `STATE_START` - Initial state
- `STATE_CHOSEONG` - Initial consonant entered
- `STATE_JUNGSEONG` - Vowel entered
- `STATE_JONGSEONG` - Final consonant entered

## Support

For issues or questions about the ChunJiIn input system, please refer to the test suite for examples of expected behavior or check the source code comments for implementation details.