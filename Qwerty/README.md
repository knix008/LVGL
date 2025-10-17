# Korean/English QWERTY Keypad with LVGL

A bilingual Korean/English virtual keyboard application built with LVGL (Light and Versatile Graphics Library), SDL2, and FreeType for TrueType font rendering.

## Features

- **Bilingual Support**: Seamless switching between English and Korean input modes
- **Hangul Composition**: Proper Korean character composition with jamo (자모) support
  - Complex vowel combinations (ㅗ+ㅏ→ㅘ, ㅜ+ㅓ→ㅝ, etc.)
  - Double consonants (ㄱ→ㄲ, ㅂ→ㅃ with Shift)
  - Real-time display of incomplete characters as you type
- **QWERTY Layout**: Standard 2-Set Korean (두벌식) keyboard mapping
- **FreeType Font Rendering**: Direct TrueType font rendering at runtime (no conversion needed)
- **Modern UI**: Built with LVGL v9 for smooth, hardware-accelerated graphics
- **Popup Dialog**: Enter key shows result in popup and clears input
- **Compact Design**: Optimized 640×480 layout

## Requirements

- **GCC compiler**
- **Make**
- **SDL2 development libraries** (libsdl2-dev)
- **FreeType development libraries** (libfreetype-dev)
- **LVGL v9.x** (automatically cloned by setup script)
- **Korean TrueType fonts** (NanumGothic fonts included in assets/)

## Quick Start

### 1. Run Setup Script

The setup script will install dependencies and download LVGL:

```bash
./setup.sh
```

This will:
- Check for and install required build tools (gcc, make, pkg-config)
- Install SDL2 development libraries if missing
- Clone LVGL v9.2 into the project directory
- Verify lv_conf.h configuration

### 2. Build the Application

```bash
make
```

For incremental builds (recommended):
```bash
make  # Only compiles changed files
```

For a complete clean build:
```bash
make clean  # Removes all object files including LVGL
make        # Full rebuild
```

The build system is optimized for development:
- **First build**: Compiles everything (takes 2-3 minutes)
- **Subsequent builds**: Only recompiles changed files (fast)
- **Clean build**: Removes all object files for fresh start

### 3. Run the Application

```bash
./qwerty
```

Or:
```bash
make run
```

## Usage

### Keyboard Layout

The virtual keyboard follows the standard QWERTY layout in a compact 640×480 window:

```
Row 0: ' 1 2 3 4 5 6 7 8 9 0 - = [←]        (← = Backspace)
Row 1: [Tab] q w e r t y u i o p [ ] \
Row 2: [Caps] a s d f g h j k l ; ' [Enter]
Row 3: [Shift] z x c v b n m , . / [Shift]
Row 4: [한/영] [Space] [Clear]
```

**Note**: The first button shows `'` (apostrophe) as a display substitute for the backtick, but types the actual `` ` `` character.

### Controls

- **한/영 (Han/Yeong)**: Toggle between Korean and English input modes
  - Shows "한글" when in English mode, "English" when in Korean mode
  - Orange background color for easy identification
- **Shift**: Toggle shift state (stays on until toggled off)
- **Caps Lock**: Toggle caps lock (stays on until toggled off)
- **← (Backspace)**: Delete last character (UTF-8 aware)
- **Enter**: Show input result in popup dialog and clear text area
  - Cyan background color for easy identification
- **Space**: Insert space character
- **Tab**: Insert 4 space characters (visible indentation)
- **Clear**: Clear all text
  - Orange background color for easy identification

### Button Color Scheme

The application uses color-coded buttons for easy identification:

- **🟠 Orange**: Function buttons (Language toggle, Clear)
- **🔵 Cyan**: Action buttons (Enter)
- **⚪ Default**: Regular keys (letters, numbers, symbols)

This color scheme helps users quickly identify special function buttons versus regular input keys.

### Korean Input

In Korean mode, the keyboard uses the **2-Set Korean (두벌식)** layout:

#### Basic Jamos:
- **Consonants (초성/종성)**: ㄱ ㄴ ㄷ ㄹ ㅁ ㅂ ㅅ ㅇ ㅈ ㅊ ㅋ ㅌ ㅍ ㅎ
- **Vowels (중성)**: ㅏ ㅑ ㅓ ㅕ ㅗ ㅛ ㅜ ㅠ ㅡ ㅣ

#### With Shift:
- **Double consonants**: ㄲ ㄸ ㅃ ㅆ ㅉ
- **Special vowels**: ㅐ ㅒ ㅔ ㅖ

#### Complex Vowel Combinations:
The application automatically combines vowels:
- ㅗ + ㅏ → ㅘ (h + k → 와)
- ㅗ + ㅐ → ㅙ (h + Shift+o → 왜)
- ㅗ + ㅣ → ㅚ (h + l → 외)
- **ㅜ + ㅓ → ㅝ** (n + j → 워)
- ㅜ + ㅔ → ㅞ (n + Shift+p → 웨)
- ㅜ + ㅣ → ㅟ (n + l → 위)
- ㅡ + ㅣ → ㅢ (m + l → 의)

#### Example:
- Type `r` (ㄱ) → shows "ㄱ"
- Type `n` (ㅜ) → shows "구"
- Type `j` (ㅓ) → shows "궈" (ㅜ+ㅓ combined to ㅝ)
- Type `s` (ㄴ) → shows "권"

## Project Structure

```
Qwerty/
├── main.c              # Main application with LVGL UI
├── qwerty.c            # Korean/English input logic and composition
├── qwerty.h            # Header file
├── lv_conf.h           # LVGL configuration (v9.4.0)
├── Makefile            # Build configuration
├── setup.sh            # Environment setup script (executable)
├── assets/             # TrueType font files
│   ├── NanumGothicCoding.ttf
│   ├── NanumGothicCoding-Bold.ttf
│   ├── NanumGothic-Regular.ttf
│   ├── NanumGothic-Bold.ttf
│   └── NanumGothic-ExtraBold.ttf
├── lvgl/               # LVGL library (cloned by setup.sh)
├── LICENSE
└── README.md           # This file
```

## Configuration

### LVGL Configuration (lv_conf.h)

The project includes a pre-configured `lv_conf.h` file with:
- **LVGL version**: v9.4.0-dev
- **Color depth**: 32-bit (XRGB8888)
- **SDL driver**: Enabled (LV_USE_SDL = 1)
- **FreeType support**: Enabled (LV_USE_FREETYPE = 1)
- **Memory pool**: 256KB (for font rendering)
- **CJK font support**: Enabled

### Display Resolution

Current resolution: **640×480** pixels

To change the display resolution, edit these constants in `main.c`:

```c
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
```

### Font Configuration

The application uses **NanumGothicCoding** as the default font with **FreeType** for direct TrueType rendering:

- **Status Label**: NanumGothicCoding 12px (normal style)
- **Text Area**: NanumGothicCoding 16px (normal style)
- **Keyboard Buttons**: NanumGothicCoding 16px (normal style)
- **Special Characters**: NanumGothicCoding 20px (normal style, for better visibility)

**Key Features:**
- **No font conversion needed!** FreeType renders `.ttf` files at runtime
- **Consistent appearance**: All text uses the same coding font family
- **Normal style**: All fonts use regular weight (not bold) for better readability
- **Korean + ASCII support**: NanumGothicCoding includes both Korean and English characters

## Recent Updates

### Version Improvements

**Latest Updates:**
- **Color-coded buttons**: Orange for function buttons, cyan for action buttons
- **Smart language button**: Shows target language (displays "한글" in English mode, "English" in Korean mode)
- **Improved tab behavior**: Inserts 4 visible spaces instead of invisible tab character
- **Optimized build system**: Faster incremental builds, proper clean functionality
- **Consistent font usage**: NanumGothicCoding font family throughout the application

**User Experience Enhancements:**
- **Visual feedback**: Color-coded buttons make functions easily identifiable
- **Intuitive language switching**: Button text shows what language you'll switch TO
- **Visible indentation**: Tab key provides clear visual spacing
- **Faster development**: Build system only recompiles changed files

## Building from Scratch

### Install Dependencies (Ubuntu/Debian)

```bash
sudo apt-get update
sudo apt-get install build-essential libsdl2-dev libfreetype-dev pkg-config git
```

### Clone LVGL

```bash
git clone https://github.com/lvgl/lvgl.git
cd lvgl
git checkout release/v9.2
cd ..
```

### Build

```bash
make -j$(nproc)
```

The first build compiles all 311 LVGL source files and may take 2-3 minutes. Subsequent builds are much faster.

## Troubleshooting

### "LVGL not found" Error

Run the setup script or manually clone LVGL:
```bash
./setup.sh
# or
git clone https://github.com/lvgl/lvgl.git
```

### "SDL not found" Error

Install SDL2 development libraries:
```bash
sudo apt-get install libsdl2-dev
```

### "FreeType not found" Error

Install FreeType development libraries:
```bash
sudo apt-get install libfreetype-dev
```

### Korean Characters Not Displaying

The application uses fonts from `assets/` directory. Verify:
1. `assets/NanumGothicCoding.ttf` exists
2. Font files are not corrupted

### Special Characters Not Visible

The application now uses NanumGothicCoding font which provides better visibility for special characters like backtick (`` ` ``) and tilde (`~`). If characters still appear invisible:
1. **Current**: Uses larger font size (20px) for special characters
2. **Alternative**: Check font file integrity

### Compilation Errors

1. Verify `lv_conf.h` exists in the project directory
2. Check that `lvgl/` directory contains LVGL v9.x
3. Run `make clean` and then `make`
4. Check compiler flags support C11 standard

### Build System Issues

**If builds are slow:**
- The first build compiles all LVGL files (normal)
- Subsequent builds should be fast (only changed files)
- Use `make clean` only when needed for fresh start

**If object files persist:**
- Run `make clean` to remove all object files including LVGL
- The improved clean target removes both application and library object files

## Korean Composition Details

### Composition Rules

The application implements proper Hangul composition:

1. **Consonant alone**: Shows as standalone jamo (초성)
2. **Consonant + Vowel**: Composes into syllable
3. **Consonant + Vowel + Consonant**: Complete syllable with 종성
4. **Complex vowels**: Automatically combines (ㅗ+ㅏ→ㅘ)
5. **Invalid combinations**: Preserves previous character, starts new

### UTF-8 Support

- Proper multi-byte character deletion
- Korean characters are 3 bytes in UTF-8
- Backspace deletes entire character, not individual bytes

## Development

### Button Size Customization

All buttons are **35×39px** (regular keys). To modify:

```c
// In create_key_button():
lv_obj_set_size(btn, width, 39);  // Change height here

// For individual buttons, change width parameter:
create_key_button(row, "a", callback, data, 38);  // Change width
```

### Button Color Customization

To change button colors, modify the styling in `create_gui()`:

```c
// Orange color for function buttons
lv_obj_set_style_bg_color(app_state.lang_button, lv_color_hex(0xFF8C00), 0);

// Cyan color for action buttons  
lv_obj_set_style_bg_color(app_state.enter_button, lv_color_hex(0x00FFFF), 0);

// Clear button (orange)
lv_obj_set_style_bg_color(app_state.clear_button, lv_color_hex(0xFF8C00), 0);
```

### Font Size Customization

Edit `init_fonts()` function in `main.c`:

```c
app_state.korean_font_20 = lv_freetype_font_create(
    "assets/NanumGothicCoding.ttf",
    LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
    16,  // <- Change size here
    LV_FREETYPE_FONT_STYLE_NORMAL
);
```

### Adding New Fonts

Place your `.ttf` font file in `assets/` directory and load it:

```c
lv_font_t *my_font = lv_freetype_font_create(
    "assets/MyFont.ttf",
    LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
    20,
    LV_FREETYPE_FONT_STYLE_NORMAL
);
```

### Modifying Keyboard Layout

Edit the `key_maps` array in `qwerty.c`:

```c
KeyMap key_maps[47] = {
    {"a", "A", "ㅁ", "ㅁ"},  // {normal, shift, korean, korean_shift}
    // ...
};
```

## Technical Details

### Technologies Used

- **LVGL**: v9.2 (Light and Versatile Graphics Library)
- **SDL2**: Display and input handling
- **FreeType**: TrueType font rendering
- **C11**: Programming language standard

### Architecture

```
User Input → LVGL Event → qwerty_process_korean_char() → 
  Composition Logic → UTF-8 Output → Text Area Display
```

### Memory Usage

- **LVGL pool**: 256KB (configured in lv_conf.h)
- **FreeType cache**: 256 glyphs (default)
- **Typical runtime**: ~10MB RAM

### Performance

- **Frame rate**: 30 FPS (SDL VSync)
- **Input latency**: < 5ms
- **Font rendering**: Hardware accelerated (SDL2)
- **Composition**: Real-time, no lag

## License

See [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Please feel free to submit issues or pull requests.

## Credits

- **LVGL**: https://lvgl.io/
- **SDL2**: https://www.libsdl.org/
- **FreeType**: https://www.freetype.org/
- **NanumGothic Font**: Naver Corporation

## References

- [LVGL Documentation](https://docs.lvgl.io/)
- [LVGL v9 Migration Guide](https://docs.lvgl.io/master/CHANGELOG.html)
- [FreeType Documentation](https://www.freetype.org/freetype2/docs/documentation.html)
- [SDL2 Documentation](https://wiki.libsdl.org/)
- [Hangul Composition](https://en.wikipedia.org/wiki/Hangul)
- [Korean Keyboard Layout (2-Set)](https://en.wikipedia.org/wiki/Keyboard_layout#Korean)
