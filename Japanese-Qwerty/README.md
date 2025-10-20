# Japanese QWERTY Input Method (IME)

A Japanese Input Method Editor for LVGL with an on-screen QWERTY button keyboard and real-time romaji to kana conversion.

## Features

- **Interactive QWERTY Button Keyboard**: Click buttons to type Japanese characters
- **640×480 Window**: Compact application window optimized for embedded displays
- **Multiple Input Modes**: Hiragana (ひらがな), Katakana (カタカナ), and English
- **Real-time Romaji Conversion**: Type in romaji and automatically convert to kana
- **Runtime Font Loading**: Loads Japanese fonts directly from TrueType files using FreeType
- **Japanese Font Support**: Uses NotoSansCJK font for proper Japanese character display
- **Comprehensive Romaji Mapping**: Supports all standard Japanese syllables including:
  - Basic vowels and consonants
  - Voiced sounds (がぎぐげご, etc.)
  - Semi-voiced sounds (ぱぴぷぺぽ)
  - Contracted sounds (きゃ, しゅ, ちょ, etc.)
  - Double consonants (っ/ッ)
  - Special character ん (n/nn)
- **LVGL GUI**: Beautiful, responsive interface powered by LVGL 9.2
- **SDL2 Backend**: Cross-platform display support

## How Japanese IME Works

### Romaji → Hiragana/Katakana
Click buttons to type romanized Japanese and the IME automatically converts it to kana:
- `konnichiha` → `こんにちは`
- `arigatou` → `ありがとう`
- `nihon` → `にほん`
- `to-kyo-` → `とーきょー`

### Input Modes

1. **Hiragana Mode (ひらがな)**: Default mode for Japanese text
2. **Katakana Mode (カタカナ)**: For foreign words and technical terms
3. **English Mode**: Direct English character input

## Installation

### Prerequisites

The following packages are required:
- GCC compiler (`build-essential`)
- SDL2 development libraries (`libsdl2-dev`)
- FreeType development libraries (`libfreetype6-dev`)
- Git (`git`)

**Note**: Font generation tools (npm, lv_font_conv) are NOT required. Fonts are loaded at runtime!

### Automated Setup

Run the setup script to automatically check dependencies, clone LVGL, and build:

```bash
chmod +x setup.sh
./setup.sh
```

The script will:
1. Check for required system packages
2. Offer to install missing packages (requires sudo)
3. Clone LVGL v9.2 if not present
4. Build the LVGL static library
5. Verify Japanese font file exists
6. Build the Japanese Input Method application
7. Offer to run the application

### Manual Build

If you prefer manual setup:

```bash
# 1. Install dependencies
sudo apt-get install build-essential libsdl2-dev libfreetype6-dev git

# 2. Clone LVGL
git clone --depth 1 --branch release/v9.2 https://github.com/lvgl/lvgl.git

# 3. Build LVGL library
mkdir -p lvgl/build lvgl/lib
find lvgl/src -name "*.c" | while read src; do
    gcc -Wall -Wextra -O2 -I. -Ilvgl $(pkg-config --cflags sdl2) -c "$src" -o "lvgl/build/$(basename ${src%.c}.o)"
done
ar rcs lvgl/lib/liblvgl.a lvgl/build/*.o

# 4. Build the application
make
```

## Usage

### Running the Application

```bash
./japanese_input
```

The application window will open at 640×480 resolution.

### On-Screen Keyboard Controls

#### Letter Keys (QWERTY Layout)
- **Row 1**: Q W E R T Y U I O P
- **Row 2**: A S D F G H J K L
- **Row 3**: Z X C V B N M

#### Special Keys
- **Switch Mode** - Toggle between Hiragana/Katakana/English modes
- **Space** - Commit current input and add space
- **Enter** - Commit current input and add newline
- **Bksp** - Backspace (delete last character)
- **-** - Insert prolonged sound mark (ー)
- **Clear** - Clear all text

### Input Display

The application shows:
1. **Mode Label**: Current input mode (ひらがな/カタカナ/English)
2. **Text Area**: Committed text with Japanese characters
3. **Input Buffer**: Shows romaji input → kana conversion in real-time

## Romaji Conversion Examples

### Basic Syllables
```
ka ki ku ke ko → か き く け こ
sa shi su se so → さ し す せ そ
ta chi tsu te to → た ち つ て と
na ni nu ne no → な に ぬ ね の
ha hi fu he ho → は ひ ふ へ ほ
```

### Contracted Sounds (拗音)
```
kya kyu kyo → きゃ きゅ きょ
sha shu sho → しゃ しゅ しょ
cha chu cho → ちゃ ちゅ ちょ
nya nyu nyo → にゃ にゅ にょ
```

### Double Consonants (促音)
```
kitte → きって (stamp)
gakkou → がっこう (school)
zasshi → ざっし (magazine)
```

### Special Cases
```
n / nn → ん
- → ー (prolonged sound mark)
```

## Example Typing Session

```
1. Click "Switch Mode" to ensure Hiragana mode is active
2. Click: k-o-n-n-i-c-h-i-h-a
   → Input buffer shows: konnichiha → こんにちは
3. Click "Space"
   → Text committed: こんにちは 
4. Click: a-r-i-g-a-t-o-u
   → Input buffer shows: arigatou → ありがとう
5. Click "Enter"
   → Text committed with newline
```

## Project Structure

```
Japanese-Qwerty/
├── main.c                  # Main entry point
├── japanese_qwerty.h       # IME interface
├── japanese_qwerty.c       # Romaji to kana conversion logic
├── japanese_gui.h          # GUI interface
├── japanese_gui.c          # GUI with QWERTY button keyboard & font loading
├── Makefile                # Build configuration
├── setup.sh                # Automated setup script
├── lv_conf.h               # LVGL configuration
├── .gitignore              # Git ignore rules
├── README.md               # This file
├── lvgl/                   # LVGL library (cloned by setup)
└── assets/
    └── NotoSansCJK.ttc     # Japanese TrueType font (loaded at runtime)
```

### Code Architecture

1. **IME Core Layer** (`japanese_qwerty.h/c`)
   - Romaji to kana conversion
   - Input mode handling
   - Pure C logic with no GUI dependencies

2. **GUI Layer** (`japanese_gui.h/c`)
   - LVGL button-based QWERTY keyboard
   - Runtime font loading using FreeType
   - Event handling for button clicks
   - Visual feedback and display updates

3. **Application Layer** (`main.c`)
   - LVGL initialization
   - SDL window creation (640×480)
   - Font loading from assets
   - Main event loop

## Font Loading

The application loads Japanese fonts at runtime using LVGL's FreeType integration:

- **Font File**: `assets/NotoSansCJK.ttc`
- **Loading Method**: FreeType at runtime (no pre-generation needed)
- **Font Size**: 20pt for main display
- **Fallback**: Uses default LVGL font if loading fails

### Why Runtime Loading?

✅ **No build-time font generation required**  
✅ **No npm or lv_font_conv dependencies**  
✅ **Smaller repository size**  
✅ **Faster build times**  
✅ **Easy to change font sizes at runtime**

## Build Targets

```bash
make          # Build the application
make run      # Build and run the application
make clean    # Remove object files and executable
make clean-all # Remove all build files including LVGL
make help     # Show help message
```

## Extending the IME

### Adding Custom Romaji Mappings

Edit `japanese_qwerty.c` and add entries to the `romaji_table`:

```c
static const RomajiMap romaji_table[] = {
    // ... existing entries ...
    {"custom", "カスタム", "カスタム"},
    {NULL, NULL, NULL}
};
```

### Customizing the Keyboard Layout

Edit `japanese_gui.c` to modify the button layout:

```c
static const char *qwerty_rows[] = {
    "qwertyuiop",
    "asdfghjkl",
    "zxcvbnm"
};
```

### Changing Font Size

Edit `japanese_gui.c` in the `gui_load_fonts()` function:

```c
font_info_20.weight = 24;  // Change from 20 to 24 for larger font
```

## Troubleshooting

### Build Errors

**Error: SDL2 not found**
```bash
sudo apt-get install libsdl2-dev
```

**Error: FreeType not found**
```bash
sudo apt-get install libfreetype6-dev
```

**Error: LVGL library not found**
```bash
rm -rf lvgl/lib lvgl/build
./setup.sh  # Rebuild
```

### Runtime Issues

**Japanese characters not displaying**
- Ensure `assets/NotoSansCJK.ttc` exists
- Check terminal output for font loading errors
- Verify FreeType is installed correctly

**Buttons not responding**
- Ensure mouse input is working
- Check terminal for error messages

**Font loading failed message**
- Verify the font file path: `assets/NotoSansCJK.ttc`
- Check file permissions (should be readable)
- Application will use default font as fallback

## Technical Details

### Window Specifications
- **Resolution**: 640×480 pixels
- **Display Area**: 620×150 pixels text area
- **Keyboard Area**: 620×210 pixels
- **Button Layout**: Standard QWERTY (3 rows + special keys)

### Font Loading Process
1. Initialize LVGL FreeType support
2. Load `assets/NotoSansCJK.ttc` at 20pt size
3. Fall back to default font if loading fails
4. Apply font to text display components

### Romaji Conversion Algorithm

1. **Longest Match First**: Tries to match the longest possible romaji sequence
2. **Small Tsu Handling**: Detects doubled consonants (e.g., `tt` → `っ` + `t`)
3. **Real-time Feedback**: Shows current romaji buffer and composed kana

## Performance

- Real-time conversion with no noticeable lag
- Efficient button event handling
- Lightweight rendering optimized for 640×480 display
- Font caching by FreeType for fast rendering

## License

This project uses LVGL which is licensed under the MIT license.

## References

- [LVGL Documentation](https://docs.lvgl.io/)
- [LVGL FreeType Integration](https://docs.lvgl.io/master/libs/freetype.html)
- [Japanese Input Method (Wikipedia)](https://en.wikipedia.org/wiki/Japanese_input_method)
- [Romaji (Wikipedia)](https://en.wikipedia.org/wiki/Romanization_of_Japanese)

## Contributing

Contributions are welcome! Areas for improvement:
- Additional romaji variants
- More sophisticated rendering
- Customizable button themes
- Additional input modes

---

**Created with LVGL 9.2 + SDL2 + FreeType for 640×480 displays**  
**Fonts loaded at runtime - no generation required!**
