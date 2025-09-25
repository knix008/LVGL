# ChunJiIn Korean Input System

A complete Korean input system based on the ChunJiIn (천지인) principle, implemented with MiniGUI for embedded systems.

## Overview

The ChunJiIn input system is based on the traditional Korean philosophical concept of "천지인" (Heaven, Earth, Human), using three fundamental elements to create all Korean characters:

- **ㆍ (천)** - Sky/Heaven (dot)
- **ㅡ (지)** - Earth (horizontal line)  
- **ㅣ (인)** - Human (vertical line)

## Features

### Core ChunJiIn System
- **Complete Korean syllable formation** (choseong, jungseong, jongseong)
- **Advanced vowel combinations** following ChunJiIn rules
- **Consonant cycling and toggling** for related sounds
- **Dot (ㆍ) display and cycling** functionality
- **Full Unicode support** with memory-safe operations
- **Comprehensive state machine** architecture

### Application
- **ChunJiIn Keypad Application** - Full MiniGUI interface with ChunJiIn keyboard

## File Structure

```
├── chunjiin_input.c          # Core ChunJiIn input system implementation
├── chunjiin_input.h          # ChunJiIn input system header
├── chunjiin_app.c            # Main ChunJiIn MiniGUI application (integrated UI)
├── run.sh                    # Run script for ChunJiIn application
├── Makefile                  # Build configuration
├── MiniGUI.cfg               # MiniGUI configuration
└── assets/                   # Korean fonts
    ├── NanumGothic-Regular.ttf
    ├── NanumGothic-Bold.ttf
    └── NanumGothic-ExtraBold.ttf
```

## Building

### Prerequisites
- MiniGUI development environment
- Korean fonts (included in assets/)
- GCC compiler with C99 support

### Build Commands

```bash
# Build ChunJiIn application
make all

# Clean build files
make clean

# Show help
make help
```

## Usage

### ChunJiIn Input System

The ChunJiIn system uses a simplified keyboard layout with three fundamental elements:

#### Fundamental Elements
- **a** - ㆍ (천) - Sky/Heaven
- **e** - ㅡ (지) - Earth  
- **i** - ㅣ (인) - Human

#### Consonant Groups
- **g** - ㄱ, ㅋ, ㄲ (cycle with repeated presses)
- **n** - ㄴ, ㄹ (cycle with repeated presses)
- **d** - ㄷ, ㅌ, ㄸ (cycle with repeated presses)
- **b** - ㅂ, ㅍ, ㅃ (cycle with repeated presses)
- **s** - ㅅ, ㅎ, ㅆ (cycle with repeated presses)
- **j** - ㅈ, ㅊ, ㅉ (cycle with repeated presses)
- **m** - ㅇ, ㅁ (cycle with repeated presses)

#### Special Keys
- **Space** - Finalize current syllable
- **<** - Backspace (cancel composition or delete character)
- **.** - Enter (complete input and clear)

### Running Applications

#### ChunJiIn Keypad Application
```bash
./chunjiin_app
```
- Full MiniGUI interface with ChunJiIn keyboard
- 5-row layout with ChunJiIn fundamental elements
- Visual feedback for input states
- Korean font support
- Press Escape or Q to quit

## ChunJiIn Input Examples

### Basic Character Formation

```
Input: g + a + i
Output: 가 (ㄱ + ㅏ = 가)

Input: n + e + i  
Output: 느 (ㄴ + ㅡ = 느)

Input: m + i
Output: 미 (ㅁ + ㅣ = 미)
```

### Consonant Cycling

```
Input: g (first press)
Output: ㄱ

Input: g (second press)  
Output: ㅋ

Input: g (third press)
Output: ㄲ

Input: g (fourth press)
Output: ㄱ (cycles back)
```

### Vowel Combinations

```
Input: a + e
Output: ㅗ (ㆍ + ㅡ = ㅗ)

Input: a + i
Output: ㅓ (ㆍ + ㅣ = ㅓ)

Input: a + a + e
Output: ㅛ (ㆍㆍ + ㅡ = ㅛ)
```

### Complex Syllables

```
Input: g + a + i + g
Output: 각 (ㄱ + ㅏ + ㄱ = 각)

Input: n + e + i + n
Output: 능 (ㄴ + ㅡ + ㄴ = 능)
```

## 5-Row Center-Aligned Keyboard Layout

The ChunJiIn keyboard follows a 5-row center-aligned layout optimized for Korean input:

```
┌─────────────────────────────────────────────────────────┐
│                    ChunJiIn Korean Input System        │
│                                                         │
│  [Text Display Area]                                    │
│                                                         │
│  Row 1: ChunJiIn Fundamental Elements (Center-Aligned)   │
│        ┌─────────┐ ┌─────────┐ ┌─────────┐               │
│        │   인    │ │   천    │ │   지    │               │
│        │  (ㅣ)   │ │  (ㆍ)   │ │  (ㅡ)   │               │
│        └─────────┘ └─────────┘ └─────────┘               │
│                                                         │
│  Row 2: Basic Consonants (Center-Aligned)              │
│        ┌─────────┐ ┌─────────┐ ┌─────────┐               │
│        │ ㄱ,ㅋ   │ │ ㄴ,ㄹ   │ │ ㄷ,ㅌ   │               │
│        └─────────┘ └─────────┘ └─────────┘               │
│                                                         │
│  Row 3: Additional Consonants (Center-Aligned)         │
│        ┌─────────┐ ┌─────────┐ ┌─────────┐               │
│        │ ㅂ,ㅍ   │ │ ㅅ,ㅎ   │ │ ㅈ,ㅊ   │               │
│        └─────────┘ └─────────┘ └─────────┘               │
│                                                         │
│  Row 4: Special Keys (Center-Aligned)                  │
│        ┌─────────┐ ┌─────────┐ ┌─────────┐               │
│        │  Space  │ │ ㅇ,ㅁ   │ │  Back  │               │
│        └─────────┘ └─────────┘ └─────────┘               │
│                                                         │
│  Row 5: Control Keys (Center-Aligned)                  │
│              ┌─────────┐ ┌─────────┐                   │
│              │  Enter  │ │  Clear  │                   │
│              └─────────┘ └─────────┘                   │
└─────────────────────────────────────────────────────────┘
```

### Layout Features
- **Center-aligned rows** for balanced appearance
- **Reduced button spacing** (10px) for compact layout
- **X-offset positioning** for fine-tuned alignment
- **Consistent button sizing** across all rows
- **Optimized spacing** between rows
- **Dynamic positioning** with automatic center calculation
- **Responsive layout** that adapts to window size

### Button Functions

#### Row 1: ChunJiIn Fundamental Elements
- **인 (ㅣ)** - Human element (vertical line)
- **천 (ㆍ)** - Sky/Heaven element (dot)
- **지 (ㅡ)** - Earth element (horizontal line)

#### Row 2: Basic Consonants
- **ㄱ,ㅋ** - G group (ㄱ, ㅋ, ㄲ - cycle with repeated presses)
- **ㄴ,ㄹ** - N group (ㄴ, ㄹ - cycle with repeated presses)
- **ㄷ,ㅌ** - D group (ㄷ, ㅌ, ㄸ - cycle with repeated presses)

#### Row 3: Additional Consonants
- **ㅂ,ㅍ** - B group (ㅂ, ㅍ, ㅃ - cycle with repeated presses)
- **ㅅ,ㅎ** - S group (ㅅ, ㅎ, ㅆ - cycle with repeated presses)
- **ㅈ,ㅊ** - J group (ㅈ, ㅊ, ㅉ - cycle with repeated presses)

#### Row 4: Special Keys
- **Space** - Finalize current syllable
- **ㅇ,ㅁ** - M group (ㅇ, ㅁ - cycle with repeated presses)
- **Back** - Backspace (cancel composition or delete character)

#### Row 5: Control Keys
- **Enter** - Complete input and clear
- **Clear** - Clear all input

## Technical Details

### State Machine
The ChunJiIn system uses a sophisticated state machine to track syllable formation:

- **STATE_START** - Initial state, no input
- **STATE_CHOSEONG** - Initial consonant entered
- **STATE_JUNGSEONG** - Vowel entered  
- **STATE_JONGSEONG** - Final consonant entered

### Memory Management
- Wide character buffers for Unicode support
- Bounds checking for all buffer operations
- Safe UTF-8 conversion functions
- Automatic cleanup on exit

### Font Support
- NanumGothic font family for Korean characters
- Automatic font loading and fallback
- Proper character encoding (UTF-8)

## Troubleshooting

### Common Issues

1. **Korean characters not displaying**
   - Ensure Korean fonts are copied to MiniGUI font directory
   - Check locale settings (ko_KR.UTF-8)
   - Verify font loading in application

2. **Input not working**
   - Check MiniGUI installation
   - Verify keyboard focus

3. **Build errors**
   - Ensure MiniGUI is properly installed
   - Check compiler and linker flags
   - Verify all source files are present

### Debug Information

Enable debug output by setting environment variables:
```bash
export CHUNJIIN_DEBUG=1
./chunjiin_app
```

## License

This project is part of the MiniGUI-Chunjiin Korean input system implementation.

## Contributing

When contributing to the ChunJiIn system:

1. Follow the existing code style
2. Add tests for new functionality
3. Update documentation
4. Ensure Korean character support
5. Test with various input combinations

## References

- ChunJiIn (천지인) Korean input method
- Korean Unicode standard
- MiniGUI documentation
- Korean language processing