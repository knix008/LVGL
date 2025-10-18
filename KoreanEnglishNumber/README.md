# Korean English Number Input System

A comprehensive multi-mode input system for Korean, English, and Number entry built with LVGL (Light and Versatile Graphics Library) and SDL2. Features modern UI design with consistent Korean font support across all input modes.

## Features

- **Three Input Modes:**
  - **Korean (KOR)**: ChunJiIn (천지인) input method for Korean characters
  - **English (ENG)**: T9-style phone keypad with uppercase/lowercase support
  - **Number (123)**: Standard number pad with numeric input

- **Enhanced UI Features:**
  - **Portrait Display Optimized**: 320 x 640 window size for mobile-like experience
  - **Mode Switching**: Easy toggle between input modes via top-right button
  - **Unified Korean Font Support**: Consistent NanumGothic font across all modes
  - **Shift Functionality**: Dynamic uppercase/lowercase in English mode
  - **Visual Feedback**: Button labels change based on input state
  - **Popup Results**: Display input results with styled dialogs

- **Technical Features:**
  - **FreeType Integration**: High-quality Korean font rendering
  - **Memory Optimized**: Efficient font caching and resource management
  - **Consistent Positioning**: Aligned button layouts across all modes
  - **Unicode Support**: Proper Korean character composition and display

## Screenshot

The application displays a compact keyboard interface optimized for narrow screens.

## Requirements

- **Operating System**: Linux (Ubuntu/Debian recommended)
- **Dependencies**:
  - CMake (>= 3.10)
  - SDL2
  - FreeType2
  - GCC/Clang compiler
  - pkg-config

## Installation

### 1. Install Dependencies

On Ubuntu/Debian:
```bash
sudo apt-get update
sudo apt-get install build-essential cmake pkg-config libsdl2-dev libfreetype-dev
```

### 2. Build the Project

```bash
cd Source
./build.sh
```

**Build Script Options:**
```bash
./build.sh          # Normal build
./build.sh clean    # Clean all build artifacts and executables  
./build.sh help     # Show available commands
```

The build script will:
- Install any missing dependencies (with sudo)
- Configure the project with CMake
- Build LVGL library
- Build the main application

## Usage

### Running the Application

```bash
cd Source
./run.sh
```

Or run directly:
```bash
cd Source/build
./main
```

### Input Modes

#### Korean ChunJiIn Mode (KOR)

The ChunJiIn (천지인) input method uses three basic vowel strokes combined with consonant groups:

**Basic Vowel Elements:**
- **ㅣ** (vertical bar) - "천" (Heaven)
- **ㆍ** (dot) - "지" (Earth)  
- **ㅡ** (horizontal bar) - "인" (Human)

**Consonant Groups:**
- **ㄱㅋ** (G/K) - ㄱ → ㅋ → ㄲ (repeated presses)
- **ㄴㄹ** (N/L) - ㄴ → ㄹ
- **ㄷㅌ** (D/T) - ㄷ → ㅌ → ㄸ
- **ㅂㅍ** (B/P) - ㅂ → ㅍ → ㅃ
- **ㅅㅎ** (S/H) - ㅅ → ㅎ → ㅆ
- **ㅈㅊ** (J/C) - ㅈ → ㅊ → ㅉ
- **ㅇㅁ** (O/M) - ㅇ → ㅁ

**Layout:**
```
[ㅣ]  [ㆍ]  [ㅡ]
[ㄱㅋ][ㄴㄹ][ㄷㅌ]
[ㅂㅍ][ㅅㅎ][ㅈㅊ]
[Space][ㅇㅁ][←]
    [Enter]
```

**Features:**
- **Real-time Composition**: Characters form as you type
- **Syllable Building**: Automatically combines consonants and vowels
- **Backspace Support**: Removes last input element
- **Unicode Display**: Proper Korean character rendering with Korean fonts

#### English T9 Mode (ENG)

Phone-style T9 input with enhanced uppercase/lowercase support:

**Layout:**
```
[abc][def][ghi]
[jkl][mno][pqr]
[stu][vwx][yz]
[Next][Space][←]
[shift][Clear][Enter]
```

**Features:**
- **Dynamic Button Labels**: Button labels change based on shift state
  - Normal mode: `abc`, `def`, `ghi`, etc. (lowercase)
  - Shift mode: `ABC`, `DEF`, `GHI`, etc. (uppercase)
- **Shift Toggle**: Press "shift" button to toggle between upper/lowercase
- **Visual Feedback**: Shift button changes color when active (gray → green)
- **T9 Cycling**: Click a button repeatedly to cycle through its letters
- **Character Finalization**: Press "Next" to finalize the current character
- **Text Operations**:
  - Press "Space" to add a space
  - Press "←" to delete the last character
  - Press "Clear" to reset input and shift state
  - Press "Enter" to confirm input

**Usage Example:**
1. Type "Hello": Press `ghi` twice (h), press `def` twice (e), press `jkl` three times (l), press `jkl` three times (l), press `mno` three times (o)
2. For uppercase: Press "shift" first, then the letter buttons show uppercase labels

#### Number Mode (123)

Standard number pad layout:

```
[1][2][3]
[4][5][6]
[7][8][9]
[Clear][0][←]
   [Enter]
```

### Mode Switching

Click the button in the top-right corner to cycle through modes:
- **KOR** → **ENG** → **123** → **KOR**

Each mode maintains its own state and provides consistent visual feedback.

## Enhanced Features

### Shift Functionality (English Mode)

The English mode includes advanced shift support:

**Visual Indicators:**
- **Button Labels**: Change dynamically based on shift state
  - Normal: `abc`, `def`, `ghi` (lowercase)
  - Shift: `ABC`, `DEF`, `GHI` (uppercase)
- **Shift Button**: Color changes to indicate state
  - Inactive: Gray background, "shift" text
  - Active: Green background, "SHIFT" text

**State Management:**
- Automatic reset when clearing text
- Persistent during character input
- Reset when switching modes or closing dialogs

### Font Consistency

All input modes use the same Korean font system:

**Features:**
- **Unified Appearance**: Consistent text rendering across modes
- **Symbol Support**: Proper display of special characters (←, Korean characters)
- **Mixed Text Support**: English mode can display Korean if needed
- **Memory Efficient**: Single font loading shared across modes

### Button Layout Alignment

All modes use consistent button positioning:

**Standards:**
- **Display Area**: 280x50px at top with 5px margin
- **Button Dimensions**: 85x70px with 8px spacing
- **Start Position**: 70px from top (consistent across modes)
- **Container Padding**: 10px all around

This ensures visual consistency when switching between input modes.

## Project Structure

```
Source/
├── assets/               # Font files (NanumGothic TrueType fonts)
│   ├── NanumGothic-Regular.ttf
│   ├── NanumGothic-Bold.ttf
│   └── NanumGothic-ExtraBold.ttf
├── build/               # Build output directory
├── docs/                # Documentation
│   └── korean-font-integration.md
├── include/             # Header files
│   ├── chunjiin_input.h
│   ├── font_config.h
│   ├── qwerty_korean.h
│   ├── tab_chunjiin.h
│   ├── tab_english.h
│   ├── tab_number.h
│   └── ui_components.h
├── lvgl/                # LVGL library (submodule)
├── src/                 # Source files
│   ├── chunjiin_input.c  # ChunJiIn input logic
│   ├── main.c            # Application entry point
│   ├── qwerty_korean.c   # Korean QWERTY logic
│   ├── tab_chunjiin.c    # Korean input UI
│   ├── tab_english.c     # English T9 input UI
│   ├── tab_number.c      # Number pad UI
│   └── ui_components.c   # Main UI, mode switching, and font management
├── build.sh             # Build script
├── cmake_help.sh        # CMake helper script
├── install_freetype_deps.sh  # Dependency installer
├── run.sh               # Run script
└── CMakeLists.txt       # CMake configuration
```

## Configuration

### Font Settings

Korean font configuration is centralized in `src/ui_components.c` with settings in `include/font_config.h`:

```c
#define FONT_PATH(filename) "../assets/" filename
#define KOREAN_FONT_REGULAR "NanumGothic-Regular.ttf"
#define DEFAULT_FONT_SIZE 24
#define KOREAN_FONT_RENDER_MODE LV_FREETYPE_FONT_RENDER_MODE_BITMAP
```

**Font Features:**
- **Dual Size Support**: 24px (main) and 16px (small) fonts
- **Memory Optimized**: 256 glyph cache, configurable memory limits
- **Fallback Support**: Automatic fallback to built-in CJK fonts
- **Consistent Rendering**: Same font used across all input modes

### Font Architecture

The application uses a centralized font loading system:
- **Loading**: `init_freetype_and_fonts()` in `ui_components.c`
- **Access**: `get_korean_font()` and `get_korean_font_small()` functions
- **Usage**: All modes use Korean fonts for consistent appearance
- **Symbols**: Backspace arrows (←) rendered with Korean font for consistency

### Window Size

Window size is set in `src/main.c`:

```c
lv_display_t * disp = lv_sdl_window_create(320, 640);
```

## Building from Source

### Manual Build Steps

1. Create build directory:
```bash
mkdir -p build
cd build
```

2. Configure with CMake:
```bash
cmake ..
```

3. Build:
```bash
make -j$(nproc)
```

4. Run:
```bash
./main
```

### Clean Build

The build script supports cleaning all build artifacts:

```bash
# Clean all build files and executables
./build.sh clean

# Then rebuild
./build.sh
```

**Clean Command Features:**
- Removes entire `build/` directory
- Removes any executables in source directory
- Cleans all CMake cache and build artifacts
- Safe to run multiple times

**Build Script Usage:**
```bash
./build.sh          # Normal build
./build.sh clean    # Clean all build artifacts
./build.sh help     # Show help message
```

## Troubleshooting

### Font Not Loading

If Korean characters don't display:
- Check that `assets/NanumGothic-Regular.ttf` exists
- The application falls back to built-in CJK fonts automatically
- Verify `LV_FONT_SOURCE_HAN_SANS_SC_16_CJK` is enabled in `lvgl/lv_conf.h`

### SDL2 Errors

If SDL2 window doesn't appear:
```bash
# Reinstall SDL2
sudo apt-get install --reinstall libsdl2-dev
```

### Build Errors

If you encounter build errors:
```bash
# Clean build using build script
./build.sh clean

# Reinstall dependencies  
sudo apt-get install build-essential cmake pkg-config libsdl2-dev libfreetype-dev

# Rebuild
./build.sh
```

**Alternative manual clean:**
```bash
# Manual clean (if build script fails)
rm -rf build
```

## Development

### Korean Font Integration

The application uses a centralized font management system in `ui_components.c`:

```c
// Font access functions
lv_font_t * get_korean_font(void);      // 24px font
lv_font_t * get_korean_font_small(void); // 16px font

// Usage in UI components
lv_obj_set_style_text_font(label, get_korean_font_small(), 0);
```

**Benefits:**
- **Consistency**: All modes use the same font rendering
- **Performance**: Single font loading, shared across components
- **Maintainability**: Centralized font configuration
- **Unicode Support**: Proper Korean character composition

### Button Layout Customization

Button layouts are defined in each tab file with consistent dimensions:

```c
// Standard dimensions (320x640 optimized)
int btn_width = 85;
int btn_height = 70; 
int btn_spacing = 8;
int start_y = 70;  // Consistent across all modes
```

**Layout Files:**
- `src/tab_chunjiin.c` - Korean ChunJiIn layout
- `src/tab_english.c` - English T9 with shift support
- `src/tab_number.c` - Number pad layout

### Adding New Input Modes

1. **Create Header**: `include/tab_yourmode.h`
2. **Implement UI**: `src/tab_yourmode.c`
   ```c
   lv_obj_t* create_yourmode_tab(lv_obj_t* parent) {
       // Create tab container
       lv_obj_t* tab = lv_obj_create(parent);
       lv_obj_set_size(tab, LV_PCT(100), LV_PCT(100));
       lv_obj_set_style_pad_all(tab, 10, 0);
       
       // Use Korean font for consistency
       lv_obj_set_style_text_font(label, get_korean_font_small(), 0);
       
       return tab;
   }
   ```
3. **Update CMake**: Add to `CMakeLists.txt` MAIN_SOURCES
4. **Update Mode Logic**: 
   - Add enum to `ui_components.c`
   - Add case to `switch_input_mode()` function
   - Update mode cycling logic

## License

This project uses LVGL which is licensed under the MIT License.

## Credits

- **LVGL**: https://lvgl.io/
- **Font**: NanumGothic (Naver Corporation)
- **ChunJiIn Input Method**: Traditional Korean input system

## Version History

- **v2.0** - Major Feature Update
  - **Enhanced English Mode**: Added shift functionality with dynamic button labels
  - **Font Integration**: Unified Korean font support across all modes
  - **Visual Improvements**: Consistent button positioning and styling
  - **Symbol Support**: Proper backspace arrow (←) display in all modes
  - **UI Polish**: Green shift button when active, improved popup dialogs
  - **Documentation**: Comprehensive font integration documentation

- **v1.0** - Initial Release
  - Korean ChunJiIn input method
  - English T9 input (basic)
  - Number pad functionality
  - 320x640 portrait mode optimization
  - Basic mode switching system
