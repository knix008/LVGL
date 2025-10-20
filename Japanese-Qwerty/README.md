# Japanese QWERTY Input Method (IME)

A Japanese Input Method Editor for LVGL with an on-screen QWERTY button keyboard featuring direct Japanese character input based on the JIS keyboard layout.

## Features

- **Interactive QWERTY Button Keyboard**: Click buttons to type Japanese characters directly
- **640×480 Window**: Compact application window optimized for embedded displays
- **JIS Keyboard Layout**: Standard Japanese computer keyboard layout
- **Direct Character Input**: Buttons show exact Japanese characters to input
- **Multiple Input Modes**: 
  - **Japanese (Hiragana/Katakana)**: Toggle with Shift key
  - **English**: Uppercase and lowercase Latin letters (Shift toggles case)
  - **Numbers/Symbols**: Digits and special characters (Shift for symbols)
- **Combining Marks**:
  - **Dakuten (゛)**: か→が, さ→ざ, た→だ, は→ば
  - **Handakuten (゜)**: は→ぱ, ひ→ぴ, ふ→ぷ
  - **Chōonpu (ー)**: Prolonged sound mark for katakana
- **Runtime Font Loading**: Loads Japanese fonts directly from TrueType files using FreeType
- **Visual Feedback**: 
  - Shift button: 🟢 Green (inactive) / 🟠 Orange (active)
  - Mode-aware keyboard labels
- **Smart Mode Toggle**: 123 button remembers previous input mode
- **LVGL GUI**: Beautiful, responsive interface powered by LVGL 9.2
- **SDL2 Backend**: Cross-platform display support

## Keyboard Layout

### Standard Mode (Hiragana/Katakana/English)
```
Row 1: [Q][W][E][R][T][Y][U][I][O][P][←]
Row 2:   [A][S][D][F][G][H][J][K][L]
Row 3: [Shift][Z][X][C][V][B][N][M][Enter]
Row 4:    [Space__________][123][Clear][゛][゜][ー]
```

**Button Sizes:**
- Letter keys: 45×32 pixels
- Backspace: 50×32 pixels
- Shift/Enter: 62×32 pixels
- Space: 200×32 pixels
- Other buttons: 32-62×32 pixels
- Gap: 4 pixels between all buttons

### Hiragana Mode
```
Row 0: [ぬ][ふ][あ][う][え][お][や][ゆ][よ][わ][ほ][へ]
Row 1: [た][て][い][す][か][ん][な][に][ら][せ][BS]
Row 2: [Shift][と][し][は][き][く][ま][の][り][Enter]
Row 3: [け][さ][そ][ひ][こ][も][ね][る][め][ろ][れ]
Row 4: [Space__________][カタカナ][123][Clear][゛][゜][ー]
```

With Shift (🟠) - Small characters:
```
Row 0: [ぬ][ふ][ぁ][ぅ][ぇ][ぉ][ゃ][ゅ][ょ][ゎ][ほ][へ]
Row 1: [た][て][ぃ][す][か][ん][な][に][ら][せ][BS]
Row 2: [Shift][と][し][は][き][く][ま][の][り][Enter]
Row 3: [け][さ][そ][ひ][こ][も][ね][る][ろ][れ][っ]
```

### Katakana Mode
```
Row 0: [ヌ][フ][ア][ウ][エ][オ][ヤ][ユ][ヨ][ワ][ホ][ヘ]
Row 1: [タ][テ][イ][ス][カ][ン][ナ][ニ][ラ][セ][BS]
Row 2: [Shift][ト][シ][ハ][キ][ク][マ][ノ][リ][Enter]
Row 3: [ケ][サ][ソ][ヒ][コ][モ][ネ][ル][メ][ロ][レ]
Row 4: [Space__________][English][123][Clear][゛][゜][ー]
```

With Shift (🟠) - Small characters:
```
Row 0: [ヌ][フ][ァ][ゥ][ェ][ォ][ャ][ュ][ョ][ヮ][ホ][ヘ]
Row 1: [タ][テ][ィ][ス][カ][ン][ナ][ニ][ラ][セ][BS]
Row 2: [Shift][ト][シ][ハ][キ][ク][マ][ノ][リ][Enter]
Row 3: [ケ][サ][ソ][ヒ][コ][モ][ネ][ル][ロ][レ][ッ]
```

### English Mode
```
Row 0: [1][2][3][4][5][6][7][8][9][0][-][=]
Row 1: [Q][W][E][R][T][Y][U][I][O][P][BS]
Row 2: [Shift][S][D][F][G][H][J][K][L][Enter]
Row 3: [;][.][/][;][.][/][;][.][/][;][.]
Row 4: [Space__________][ひらがな][123][Clear][゛][゜][ー]
```

With Shift (🟠) - Uppercase:
```
Row 0: [1][2][3][4][5][6][7][8][9][0][-][=]
Row 1: [Q][W][E][R][T][Y][U][I][O][P][BS]
Row 2: [Shift][S][D][F][G][H][J][K][L][Enter]
Row 3: [;][.][/][;][.][/][;][.][/][;][.]
```

### Number/Symbol Mode
```
Row 0: [1][2][3][4][5][6][7][8][9][0][-][=]
Row 1: [[][]][{][}][(][)][^][`][\ ][|][BS]
Row 2: [Shift][;][:][.][/][<][>][€][¥][Enter]
Row 3: [@][&][#][⌘][⌥][⌃][⌫][⌦][℉][µ][£]
Row 4:    [Space__________][あ/カ/ABC][Clear][゛][゜][ー]

With Shift (🟠):
Row 0: [!][~][°][$][%][•][*][_][+][±][≡][≈]
Row 1: [«][»][←][→][↑][↓][☆][★][▲][▼][BS]
Row 2: [Shift]["]['][¿][¡][√][×][÷][≠][Enter]
Row 3: [?][¢][§][¶][†][‡][✓][∑][π][Ω][∆]
```

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

### Button Controls

#### Main Input Buttons
Click the letter keys to type Japanese characters directly as shown on the buttons.

#### Special Keys

| Button | Function | Description |
|--------|----------|-------------|
| **あ/A** | Toggle mode | Japanese (Hiragana) ↔ English |
| **Shift** | Multi-function | Hiragana↔Katakana / Small chars / Uppercase / Symbols |
| **123** | Number mode | Toggles to numbers, shows previous mode (あ/ABC) |
| **Space** | Add space | Inserts space character |
| **Enter** | Submit input | Shows popup with result and clears text |
| **←** | Backspace | Deletes last character |
| **Clear** | Clear all | Removes all text |
| **゛** | Dakuten | Adds voicing mark (か→が) |
| **゜** | Handakuten | Adds semi-voicing mark (は→ぱ) |
| **ー** | Chōonpu | Prolonged sound mark |

### Shift Button Behavior

The Shift button (🟢 Green / 🟠 Orange) serves multiple purposes:

**In Japanese Mode:**
- 🟢 **Green (OFF)**: Hiragana characters (た, て, い, す...)
- 🟠 **Orange (ON)**: Katakana characters (タ, テ, イ, ス...)
- Also accesses small characters (っ, ゃ, ょ, ぁ, ぅ, etc.)

**In English Mode:**
- 🟢 **Green (OFF)**: Lowercase letters (a, b, c...)
- 🟠 **Orange (ON)**: Uppercase letters (A, B, C...)

**In Number Mode:**
- 🟢 **Green (OFF)**: Numbers (1, 2, 3...) and symbols
- 🟠 **Orange (ON)**: Special characters (!, @, #...)

### 123/ABC Toggle Button

**Smart Mode Memory:**
- In Letter mode → Button shows "**123**" → Click to enter Number mode
- In Number mode → Button shows previous mode (**あ** or **ABC**) → Click to return

**Example:**
1. Start in Japanese mode (Hiragana)
2. Click **[123]** → Enter number mode, button changes to **[あ]**
3. Type numbers...
4. Click **[あ]** → Return to Japanese mode, button changes to **[123]**

**Note:** The Shift key toggles between Hiragana and Katakana in Japanese mode, so you don't need a separate Katakana mode button.

### Combining Marks (Dakuten/Handakuten)

Type a base character, then click the combining mark button:

**Dakuten (゛) Examples:**
- か + ゛ → が (ka → ga)
- さ + ゛ → ざ (sa → za)
- た + ゛ → だ (ta → da)
- は + ゛ → ば (ha → ba)

**Handakuten (゜) Examples:**
- は + ゜ → ぱ (ha → pa)
- ひ + ゜ → ぴ (hi → pi)
- ふ + ゜ → ぷ (fu → pu)

## Example Typing

### Type "こんにちは" (Hello)
1. Click B (こ)
2. Click Y (ん)
3. Click I (に)
4. Click A (ち)
5. Click F (は)

### Type "ありがとう" (Thank you)
Need additional vowel characters (あ, り, がconversion, とう)

### Type "がっこう" (School)
1. Click T (か)
2. Click ゛ → becomes が
3. Click Shift (turns orange), then Z (っ)
4. Click B (こ)
5. Need う character

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
├── run_test.sh             # Test runner script
├── lv_conf.h               # LVGL configuration
├── .gitignore              # Git ignore rules
├── README.md               # This file
├── assets/
│   └── NotoSansCJK.ttc     # Japanese TrueType font (loaded at runtime)
├── tests/
│   ├── test_japanese_qwerty.c  # Core IME tests (26 tests)
│   ├── test_dakuten.c          # Combining marks tests (27 tests)
│   ├── Makefile                # Test build configuration
│   └── README.md               # Test documentation
└── lvgl/                   # LVGL library (cloned by setup)
```

### Code Architecture

1. **IME Core Layer** (`japanese_qwerty.h/c`)
   - Romaji to kana conversion (for fallback/compatibility)
   - Input mode handling
   - Buffer management
   - Pure C logic with no GUI dependencies

2. **GUI Layer** (`japanese_gui.h/c`)
   - LVGL button-based QWERTY keyboard with JIS layout
   - Runtime font loading using FreeType
   - Direct Japanese character input
   - Dakuten/handakuten combining logic
   - Event handling for button clicks
   - Visual feedback (shift colors, mode indicators)

3. **Application Layer** (`main.c`)
   - LVGL initialization
   - SDL window creation (640×480)
   - Font loading from assets
   - Main event loop

## Font Loading

The application loads Japanese fonts at runtime using LVGL's FreeType integration:

- **Font File**: `assets/NotoSansCJK.ttc`
- **Loading Method**: FreeType at runtime (no pre-generation needed)
- **Font Size**: 14px for all text
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

## Testing

The project includes a comprehensive test suite in the `tests/` directory.

### Quick Test Run

From the project root directory:

```bash
./run_test.sh     # Compile and run all tests with colored output
```

### Manual Test Running

Alternatively, run tests from the tests directory:

```bash
cd tests
make              # Build and run all tests
make run-qwerty   # Run core IME tests only
make run-dakuten  # Run dakuten combining tests only
make clean        # Clean test builds
```

### Test Coverage

**test_japanese_qwerty.c** (51 tests): Core IME functionality
- IME initialization and state management
- Mode switching (Hiragana/Katakana/English)
- Romaji to Kana conversion
- All vowels (a, i, u, e, o) in Hiragana and Katakana
- Y-row characters (ya, yu, yo)
- W-row character (wa)
- Additional consonants (nu, fu, ho, he)
- Complete word tests (arigatou, watashi)
- Backspace, Space, Enter operations
- Buffer management and overflow protection
- Mode preservation

**test_dakuten.c** (27 tests): Combining marks
- Dakuten (゛) conversion table: か→が, さ→ざ, た→だ, は→ば
- Handakuten (゜) conversion table: は→ぱ, ひ→ぴ, etc.
- Complete character set coverage (20 dakuten + 5 handakuten)

### Test Results

All tests pass with 100% success rate:
- ✅ 51/51 core IME tests passed
- ✅ 27/27 dakuten combining tests passed
- ✅ Total: 78/78 tests passed

## Customization

### Changing Font Size

Edit `japanese_gui.c` in the `gui_load_fonts()` function:

```c
lv_font_t *japanese_font = lv_freetype_font_create(
    "assets/NotoSansCJK.ttc",
    LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
    16,  // Change from 14 to your preferred size
    LV_FREETYPE_FONT_STYLE_NORMAL
);
```

### Modifying Button Layout

Edit button sizes in `japanese_gui.c` → `gui_create_qwerty_keyboard()`:

```c
int btn_width = 45;   // Change button width
int btn_height = 32;  // Change button height
int btn_gap = 4;      // Change gap between buttons
```

### Adding More Japanese Characters

Edit character arrays in `japanese_gui.c`:

```c
static const char *hiragana_hints[] = {
    // Add or modify character mappings
};
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

**Dakuten/Handakuten not working**
- Only works on compatible characters (か, さ, た, は rows)
- Must have at least one character typed before using combining marks

## Technical Details

### Window Specifications
- **Resolution**: 640×480 pixels
- **Text Display Area**: 620×170 pixels
- **Keyboard Area**: 620×250 pixels
- **Button Layout**: Standard QWERTY with JIS character mapping
- **All buttons centered** in their respective rows

### Font Loading Process
1. Initialize LVGL FreeType support
2. Load `assets/NotoSansCJK.ttc` at 14pt size
3. Fall back to default font if loading fails
4. Apply font to all text display components

### Direct Input Architecture

Unlike traditional IME systems that convert romaji to kana, this system provides **direct character input**:

```
Traditional IME:    Button "か" → romaji 'k'+'a' → conversion → output "か"
This System:        Button "か" → direct input → output "か"
```

**Benefits:**
- Faster input (no conversion delay)
- What You See Is What You Get (WYSIWYG)
- Familiar to users of Japanese keyboards
- Simpler codebase (no complex conversion logic needed)

### Combining Mark Algorithm

1. User types base character (e.g., "か")
2. User clicks dakuten button (゛)
3. System finds last UTF-8 character in display buffer
4. Looks up character in conversion table
5. Replaces base character with dakuten version

## Performance

- Real-time character input with no lag
- Efficient button event handling
- Lightweight rendering optimized for 640×480 display
- Font caching by FreeType for fast rendering
- All buttons update instantly on mode/shift changes

## License

This project uses LVGL which is licensed under the MIT license.

## References

- [LVGL Documentation](https://docs.lvgl.io/)
- [LVGL FreeType Integration](https://docs.lvgl.io/master/libs/freetype.html)
- [JIS Keyboard Layout (Wikipedia)](https://en.wikipedia.org/wiki/Keyboard_layout#Japanese)
- [Japanese Input Method (Wikipedia)](https://en.wikipedia.org/wiki/Japanese_input_method)
- [Dakuten and Handakuten (Wikipedia)](https://en.wikipedia.org/wiki/Dakuten_and_handakuten)

## Contributing

Contributions are welcome! Areas for improvement:
- Additional character mappings for complete JIS coverage
- Autocomplete/prediction features
- Customizable button themes and colors
- Additional input modes (half-width katakana, etc.)
- Kanji conversion support
- Export/save text functionality

---

**Created with LVGL 9.2 + SDL2 + FreeType for 640×480 displays**  
**Fonts loaded at runtime - no generation required!**  
**Direct Japanese character input based on JIS keyboard layout**
