# Korean Keypad Implementation Guide

## Overview

This project implements a Korean keypad with Hangul (한글) input support using LVGL GUI framework. The keypad provides a modern, touch-friendly interface for Korean text input with real-time character composition.

## Features

### 🎯 Core Features
- **Hangul Character Composition**: Real-time composition of Korean syllables from consonants and vowels
- **Multiple Input Modes**: Switch between Hangul, English, and Number input modes
- **Modern UI Design**: Clean, intuitive interface with color-coded buttons
- **UTF-8 Support**: Full Unicode support for Korean characters
- **Touch-Friendly**: Optimized button sizes and spacing for touch input

### 🔤 Input Modes
1. **한글 (Hangul)**: Korean character input with syllable composition
2. **ABC (English)**: English alphabet input
3. **123 (Numbers)**: Numeric input

### ⌨️ Character Layout

#### Hangul Mode
- **Consonants (자음)**: ㄱㄲㄴㄷㄸㄹㅁㅂㅃㅅㅆㅇㅈㅉㅊㅋㅌㅍㅎ
- **Vowels (모음)**: ㅏㅐㅑㅒㅓㅔㅕㅖㅗㅘㅙㅚㅛㅜㅝㅞㅟㅠㅡㅢㅣ

#### Function Buttons
- **Clear**: Clear all entered text
- **⌫ (Backspace)**: Delete last character
- **Space**: Add space character
- **Enter**: Submit entered text

## How to Use

### Building the Project

1. **Install Dependencies**:
   ```bash
   sudo apt install build-essential cmake libsdl2-dev
   ```

2. **Build the Project**:
   ```bash
   cd Source
   mkdir build
   cd build
   cmake ..
   make
   ```

3. **Run the Application**:
   ```bash
   ./lvgl_main
   ```

### Using the Korean Keypad

1. **Launch the Application**: Run `./lvgl_main` from the build directory
2. **Navigate to Korean Tab**: Click on the "한글" tab in the tab view
3. **Select Input Mode**: Use the mode button to switch between Hangul, English, and Number modes
4. **Enter Korean Text**:
   - Click consonants (자음) first
   - Click vowels (모음) to complete the syllable
   - The system automatically composes the syllable
   - Continue for additional syllables

### Example Usage

To type "안녕하세요" (Hello):
1. Click "ㅇ" (consonant)
2. Click "ㅏ" (vowel) → forms "아"
3. Click "ㄴ" (consonant)
4. Click "ㄴ" (consonant) → forms "안"
5. Click "ㄴ" (consonant)
6. Click "ㅕ" (vowel) → forms "녕"
7. Continue for "하세요"

## Technical Implementation

### File Structure
```
Source/
├── include/
│   ├── ui_korean_keypad.h      # Korean keypad header
│   └── ui_callbacks.h          # Callback function declarations
├── src/
│   ├── ui_korean_keypad.c      # Korean keypad implementation
│   ├── ui_main.c               # Main UI with tab integration
│   └── ui_callbacks.c          # Callback implementations
└── CMakeLists.txt              # Build configuration
```

### Key Components

#### 1. Hangul Composition System
- **Choseong (초성)**: Initial consonants
- **Jungseong (중성)**: Vowels
- **Jongseong (종성)**: Final consonants (optional)

#### 2. Unicode Support
- Uses Unicode Hangul ranges for proper character encoding
- UTF-8 conversion for display
- Support for all 11,172 possible Hangul syllables

#### 3. UI Components
- **Display Area**: Shows entered text in real-time
- **Character Buttons**: Color-coded by type (consonants/vowels)
- **Function Buttons**: Clear, backspace, space, enter
- **Mode Switch**: Toggle between input modes

### Configuration

#### Font Configuration
The project uses the SimSun CJK font for Korean character display:
```c
#define LV_FONT_SIMSUN_14_CJK 1
#define LV_FONT_DEFAULT &lv_font_simsun_14_cjk
```

#### Window Size
The application window is configured for optimal keypad usage:
```c
lv_display_t * disp = lv_sdl_window_create(320, 480);
```

## Customization

### Adding New Characters
To add additional Korean characters, modify the character arrays in `ui_korean_keypad.c`:
```c
static const char* hangul_choseong[] = {
    "ㄱ", "ㄲ", "ㄴ", "ㄷ", "ㄸ", "ㄹ", "ㅁ", "ㅂ", "ㅃ", "ㅅ", "ㅆ", "ㅇ", "ㅈ", "ㅉ", "ㅊ", "ㅋ", "ㅌ", "ㅍ", "ㅎ"
};
```

### Changing Colors
Modify button colors by updating the color values:
```c
lv_obj_set_style_bg_color(btn, lv_color_hex(0xE3F2FD), 0);  // Light blue for consonants
lv_obj_set_style_bg_color(btn, lv_color_hex(0xFFEBEE), 0);  // Light red for vowels
```

### Adjusting Layout
Modify button positions and sizes in the `create_hangul_buttons()` function:
```c
int btn_width = 35;
int btn_height = 35;
int start_x = 10;
int start_y = 140;
```

## Troubleshooting

### Common Issues

1. **Korean Characters Not Displaying**:
   - Ensure CJK font is enabled in `lv_conf.h`
   - Check that `LV_FONT_SIMSUN_14_CJK` is set to 1

2. **Build Errors**:
   - Verify all dependencies are installed
   - Check that SDL2 development libraries are available

3. **Character Composition Issues**:
   - Ensure proper UTF-8 encoding
   - Check Unicode conversion functions

### Debug Information
The application prints debug information to the console:
- Entered text is logged when Enter is pressed
- Tab changes are logged
- Character composition steps can be traced

## Future Enhancements

### Potential Improvements
1. **Advanced Composition**: Support for complex syllable combinations
2. **Predictive Text**: Word suggestions and auto-completion
3. **Custom Dictionaries**: User-defined word lists
4. **Gesture Support**: Swipe gestures for faster input
5. **Haptic Feedback**: Touch feedback for better UX
6. **Accessibility**: Voice input and screen reader support

### Integration Possibilities
- **Database Storage**: Save entered text to SQLite database
- **Cloud Sync**: Synchronize with cloud services
- **Multi-language**: Support for other CJK languages
- **Mobile Port**: Adapt for mobile platforms

## License

This implementation is part of the KoreanKeyPad project and follows the same licensing terms as the parent project.

## Contributing

To contribute to the Korean keypad implementation:
1. Fork the repository
2. Create a feature branch
3. Implement your changes
4. Test thoroughly with Korean text input
5. Submit a pull request

## Support

For issues and questions:
1. Check the troubleshooting section
2. Review the code comments
3. Test with different Korean text inputs
4. Report bugs with detailed reproduction steps 