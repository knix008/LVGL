# Korean Input System Guide

## Overview

This enhanced Korean input system provides support for both traditional jamo-based input and modern two-set keyboard input (두벌식 자판). The system is designed to work with LVGL-based applications and provides real-time Hangul composition.

## Features

### 1. Two-Set Keyboard Input (두벌식 자판)
- **QWERTY to Hangul mapping**: Maps standard QWERTY keys to Korean jamo
- **Real-time composition**: Automatically composes Hangul syllables as you type
- **Incomplete syllable handling**: Intelligently handles partial inputs

### 2. Traditional Jamo Input
- **Direct jamo selection**: Choose individual jamo (ㄱ, ㄴ, ㄷ, etc.)
- **Manual composition**: Build syllables by selecting choseong, jungseong, and jongseong
- **Visual feedback**: See composition progress in real-time

## Two-Set Keyboard Mapping

### Consonants (자음)
| Key | Jamo | Key | Jamo | Key | Jamo |
|-----|------|-----|------|-----|------|
| r   | ㄱ   | R   | ㄲ   | s   | ㄴ   |
| e   | ㄷ   | E   | ㄸ   | f   | ㄹ   |
| a   | ㅁ   | q   | ㅂ   | Q   | ㅃ   |
| t   | ㅅ   | T   | ㅆ   | d   | ㅇ   |
| w   | ㅈ   | W   | ㅉ   | c   | ㅊ   |
| z   | ㅋ   | x   | ㅌ   | v   | ㅍ   |
| g   | ㅎ   |     |      |     |      |

### Vowels (모음)
| Key | Jamo | Key | Jamo | Key | Jamo |
|-----|------|-----|------|-----|------|
| k   | ㅏ   | o   | ㅐ   | i   | ㅑ   |
| O   | ㅒ   | j   | ㅓ   | p   | ㅔ   |
| u   | ㅕ   | P   | ㅖ   | h   | ㅗ   |
| y   | ㅛ   | n   | ㅜ   | b   | ㅠ   |
| m   | ㅡ   | l   | ㅣ   | L   | ㅢ   |

## Usage Examples

### Basic Syllables
```
Input: rk    Output: 가
Input: sk    Output: 나
Input: ek    Output: 다
Input: fk    Output: 라
Input: ak    Output: 마
Input: qk    Output: 바
Input: tk    Output: 사
Input: dk    Output: 아
Input: wk    Output: 자
Input: ck    Output: 차
Input: zk    Output: 카
Input: xk    Output: 타
Input: vk    Output: 파
Input: gk    Output: 하
```

### Complex Words
```
Input: rkskekfk    Output: 안녕하세요
Input: dkssud      Output: 안녕
Input: qkrtk       Output: 바보
Input: dkdld       Output: 아야
Input: skfk        Output: 나라
Input: ekfk        Output: 다라
```

## API Reference

### Core Functions

#### `void init_korean_keypad(void)`
Initializes the Korean keypad system. Must be called before using any other functions.

#### `void process_two_set_input(char key)`
Processes a single key press for two-set keyboard input.

**Parameters:**
- `key`: The pressed key character

#### `void complete_current_syllable(void)`
Completes the current syllable and adds it to the buffer.

#### `void reset_two_set_state(two_set_input_state_t * state)`
Resets the two-set input state to initial values.

#### `uint32_t compose_hangul_from_indices(int cho_idx, int jung_idx, int jong_idx)`
Composes a Hangul syllable from jamo indices.

**Parameters:**
- `cho_idx`: Choseong (initial consonant) index
- `jung_idx`: Jungseong (vowel) index  
- `jong_idx`: Jongseong (final consonant) index

**Returns:**
- Unicode code point of the composed syllable

### Buffer Management

#### `void add_char_to_buffer(const char * str)`
Adds a character or string to the Korean input buffer.

#### `void remove_char_from_buffer(void)`
Removes the last character from the buffer (handles UTF-8 properly).

#### `void update_korean_display(void)`
Updates the display with current buffer contents.

## Integration with LVGL

### Setting up Display
```c
// Create a label for Korean text display
lv_obj_t * korean_label = lv_label_create(parent);
korean_keypad_state.display_label = korean_label;

// Set Korean font
if (korean_font) {
    lv_obj_set_style_text_font(korean_label, korean_font, 0);
}
```

### Handling Key Events
```c
// In your key event handler
void key_event_handler(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_KEY) {
        uint32_t key = lv_event_get_key(e);
        
        // Convert LVGL key to character and process
        char key_char = (char)key;
        process_two_set_input(key_char);
    }
}
```

## Building and Testing

### Build the Test Application
```bash
cd Source
mkdir build
cd build
cmake ..
make korean_test
```

### Run the Test
```bash
./korean_test
```

The test application provides:
- Individual jamo composition tests
- Complex syllable tests
- Interactive two-set keyboard input testing

## Technical Details

### Unicode Composition
The system uses the standard Hangul Unicode composition formula:
```
Syllable = 0xAC00 + (cho_idx * 21 * 28) + (jung_idx * 28) + jong_idx
```

### UTF-8 Encoding
All output is properly encoded in UTF-8 for display compatibility.

### State Management
The system maintains input state to handle:
- Incomplete syllables
- Multi-step composition
- Error recovery

## Troubleshooting

### Common Issues

1. **Font not loading**: Ensure the NanumGothic font files are in the correct location
2. **Incorrect composition**: Check that the input follows the two-set keyboard mapping
3. **Display issues**: Verify UTF-8 encoding and font support

### Debug Information
Enable debug output by checking the console for:
- Font loading status
- Composition state changes
- Input processing logs

## Future Enhancements

- Support for three-set keyboard (세벌식 자판)
- Advanced composition rules
- Predictive text input
- Custom keyboard layouts
- Voice input support 