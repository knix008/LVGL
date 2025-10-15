# Korean QWERTY Input System

A comprehensive Korean virtual keyboard implementation using MiniGUI framework with full QWERTY layout and advanced Hangul composition support.

## Features

### 🔤 **Complete QWERTY Layout**
- Full 4-row keyboard layout with proper key spacing
- Dual Shift keys (Left and Right) in Row 3 for enhanced usability
- All standard punctuation and special characters with proper visibility
- Backspace and Enter functionality with visual feedback
- **Fixed Character Display**: Resolved font rendering issues for all ASCII characters

### 🇰🇷 **Advanced Korean Input**
- **Hangul Composition**: Real-time Korean character assembly with visual feedback
- **Compound Vowels**: Support for complex vowels (ㅜ+ㅓ=ㅝ, ㅗ+ㅏ=ㅘ, ㅗ+ㅣ=ㅚ, etc.)
- **Complete Jamo Set**: All Korean consonants and vowels including double consonants
- **Proper Syllable Formation**: Automatic cho-jung-jong composition (e.g., ㄱ+ㅜ+ㅓ+ㄴ = 권)
- **Smart Composition**: Automatic backspace and recomposition for complex characters

### 🎨 **User Interface**
- **Optimized Layout**: Mathematically centered button positioning with no overlaps
- **Visual Feedback**: Shift button state indication and mode indicators
- **Mode Switching**: Korean/English toggle with clear visual indicators (한글/Eng)
- **Real-time Display**: Live text composition preview with proper font rendering
- **Responsive Design**: Proper button spacing and centered rows for optimal UX

### 🛠 **Technical Features**
- **Enhanced Font Support**: NanumGothic Bold prioritized for optimal character coverage
- **Character Visibility**: Solved zero-width character issues (apostrophe substitution for backtick)
- **Unicode Support**: Full UTF-8 text handling with proper Korean locale support
- **Memory Management**: Proper resource cleanup and font management
- **Error Handling**: Graceful font fallback system with multiple font options
- **Performance**: Optimized font size (14pt) to reduce FreeType2 cache issues

## Project Structure

```
├── main.c              # Main application and UI logic
├── qwerty-korean.c     # Korean input processing and character composition
├── qwerty.h           # Header file with data structures and function declarations
├── Makefile           # Build configuration
├── MiniGUI.cfg        # MiniGUI runtime configuration
├── install_minigui.sh # MiniGUI installation script
└── assets/            # Font files
    ├── NanumGothic-Bold.ttf
    ├── NanumGothic-ExtraBold.ttf
    └── NanumGothic-Regular.ttf
```

## Building and Running

### Prerequisites
- MiniGUI 5.0+ with threading support
- GCC compiler
- Linux environment (tested on Ubuntu 24.04)
- Korean font support

### Installation
1. **Install MiniGUI** (if not already installed):
   ```bash
   ./install_minigui.sh
   ```

2. **Build the application**:
   ```bash
   make clean && make
   ```

3. **Run the Korean QWERTY input system**:
   ```bash
   ./qwerty
   ```

## Usage

### Basic Operation
- **Korean/English Toggle**: Click the "한글"/"Eng" button or use mode switching
- **Shift Keys**: Press either Left or Right Shift for capitalization/special characters
- **Text Input**: Click keys to compose text in the display area
- **Backspace**: Delete characters with the ← button
- **Enter**: Complete input and show result dialog

### Korean Input Examples
- **Simple Characters**: ㄱ + ㅏ = 가
- **Complex Syllables**: ㄱ + ㅜ + ㅓ + ㄴ = 권 (compound vowel ㅜ+ㅓ=ㅝ)
- **Final Consonants**: ㅎ + ㅏ + ㄴ = 한
- **Double Vowels**: ㅗ + ㅏ = ㅘ (와), ㅜ + ㅣ = ㅟ (위)

### Keyboard Layout
```
Row 0: ' 1 2 3 4 5 6 7 8 9 0 - = [Backspace]
Row 1:   Q W E R T Y U I O P [ ] \
Row 2:    A S D F G H J K L ; '
Row 3: [Shift] Z X C V B N M , . / [Shift]
Row 4:      [한글/Eng] [Space] [Enter]
```

**Note**: The first key shows an apostrophe (') instead of backtick (`) due to font rendering optimization, but functions identically for input purposes.

## Technical Implementation

### Korean Character Composition
The system implements proper Hangul composition following Unicode standards:
- **Initial Consonants (초성)**: 19 consonants including double consonants (ㄲ, ㄸ, ㅃ, ㅆ, ㅉ)
- **Vowels (중성)**: 21 vowels including compound forms (ㅝ, ㅘ, ㅚ, ㅟ, ㅢ, etc.)
- **Final Consonants (종성)**: 28 consonant combinations including complex forms (ㄳ, ㄺ, ㄻ, etc.)
- **Smart Composition**: Automatic handling of compound vowels like ㅜ+ㅓ=ㅝ

### Font Handling & Character Display
- **Primary Font**: NanumGothic-Bold (14pt) for optimal character coverage and reduced cache issues
- **Fallback Fonts**: NanumGothic-Regular → System fixed font
- **Character Visibility Solution**: 
  - Resolved zero-width/height backtick character issue in NanumGothic fonts
  - Implemented apostrophe (') substitution for reliable display
  - Maintains full keyboard functionality while ensuring visual clarity
- **Korean Locale**: Set to ko_KR.UTF-8 for proper Korean character support
- **Font Cache Optimization**: Reduced font size to minimize FreeType2 rendering issues

### Layout & UI Enhancements
- **Precise Positioning**: Mathematical centering for all keyboard rows
- **Overlap Prevention**: Fixed Row 3 spacing issues between Shift and Z keys
- **Dual Shift Support**: Left and Right Shift keys with proper visual feedback
- **Window Sizing**: Optimized 750px width for proper character display without cutoff

### Memory Management
- Proper MiniGUI resource cleanup with signal handling
- Font destruction on application exit
- Window and button resource management
- Graceful error handling for font loading failures

## Configuration

### MiniGUI Configuration (MiniGUI.cfg)
```ini
[system]
gal_engine=pc_xvfb
defaultmode=800x600-16bpp

[pc_xvfb]
window_caption=Korean QWERTY Input
bg_color=0xC0C0C0
```

### Compile Options
- **Threading**: `-lminigui_ths` for thread-safe operation
- **Font Support**: FreeType and HarfBuzz integration
- **Image Support**: PNG and JPEG support

## Troubleshooting

### Common Issues
1. **Font Errors**: `Invalid charset name` warnings are normal and don't affect functionality
2. **FreeType2 Cache Warnings**: `FONT>FT2: can't access image cache` messages are cosmetic only
3. **Display Issues**: Ensure proper Korean locale support (ko_KR.UTF-8 or en_US.UTF-8)
4. **Build Errors**: Verify MiniGUI development headers and threading support are installed
5. **Character Display**: If certain characters don't appear, check font installation in assets/

### Debug Output
The application provides verbose logging for:
- Font loading status and fallback sequence
- Korean character composition and compound vowel formation
- Shift key state changes and visual feedback
- Mode switching events (Korean ↔ English)
- Button press events and text output

### Performance Notes
- FreeType2 cache warnings are expected with Korean characters and don't impact functionality
- Font size optimized to 14pt to reduce cache pressure
- Window width set to 750px for optimal layout without character cutoff

## Known Problems

### 1. **Backtick Character Replacement**
**Issue**: The backtick character (`) has zero width/height in NanumGothic fonts, making it invisible in both button display and text output.

**Current Solution**: 
- **Button Display**: Shows apostrophe (') instead of backtick (`)
- **Text Output**: Produces apostrophe (') when the first key is pressed
- **Functionality**: Key mapping changed from `{"`", "~"}` to `{"'", "~"}` for consistency

**Impact**: 
- ✅ **Positive**: All characters are now visible and functional
- ⚠️ **Limitation**: Visual representation differs from standard QWERTY layout
- 🔄 **Alternative**: Users can modify `qwerty-korean.c` key mapping if backtick functionality is required

### 2. **Text Display Auto-Wrapping Issue**
**Issue**: Long text input in the display area may not wrap properly, potentially causing text to extend beyond the visible text box boundaries.

**Current Behavior**:
- Text displays in a single line within the text box
- No automatic line breaks or text wrapping
- Very long input may not be fully visible

**Potential Solutions**:
- Implement manual text wrapping at character limits
- Add horizontal scrolling to text display
- Increase text box height for multi-line display
- Add text length limiting with visual feedback

**Workaround**: 
- Use Enter key periodically to display current input
- Clear text box when it becomes too long
- Monitor input length during composition

## Known Limitations & Solutions
- **Font Cache Warnings**: FreeType2 glyph cache issues with Korean characters (cosmetic only)
- **Environment**: Requires X11 environment for pc_xvfb GAL engine
- **Character Rendering**: Zero-width backtick issue solved with apostrophe substitution
- **Layout Constraints**: Fixed button overlap issues with precise mathematical positioning

## Development Notes

### Key Features Implemented
- ✅ Dual Shift key support in Row 3 with proper spacing
- ✅ Compound vowel composition (권, 봐, 뷰, 의, etc.)
- ✅ Centered keyboard layout with mathematical precision
- ✅ Enhanced font fallback system for character coverage
- ✅ Real-time Korean text composition with visual feedback
- ✅ Character visibility solutions for problematic glyphs
- ✅ Optimized window sizing and button positioning

### Architecture
- **Modular Design**: Separate Korean processing (qwerty-korean.c) from UI logic (main.c)
- **State Management**: Comprehensive keyboard and composition state tracking
- **Font Management**: Multi-level fallback with automatic optimization
- **Event Handling**: Proper MiniGUI message processing with dual shift support
- **Resource Management**: Clean initialization and cleanup with signal handling
- **Event Handling**: Proper MiniGUI message processing
- **Resource Management**: Clean initialization and cleanup

## License
This project is part of the MiniGUI ecosystem and follows standard open-source practices.

## Recent Updates & Fixes

### Version 1.1 (October 2025)
- ✅ **Character Visibility**: Solved backtick character display issues with apostrophe substitution
- ✅ **Layout Optimization**: Fixed button overlapping issues in Row 3 (Shift + Z keys)
- ✅ **Font Enhancement**: Prioritized NanumGothic-Bold for better character coverage
- ✅ **Performance**: Optimized font size (14pt) to reduce FreeType2 cache warnings
- ✅ **UI Improvements**: Mathematical centering for all keyboard rows
- ✅ **Dual Shift**: Implemented Left and Right Shift keys with proper visual feedback
- ✅ **Window Sizing**: Optimized to 750px width for proper character display

### Known Working Features
- Korean character composition with compound vowels (권, 봐, 뷰, 의)
- Real-time text display with proper font rendering
- Mode switching between Korean and English
- All ASCII characters display correctly (with apostrophe substitution for backtick)
- Proper keyboard layout without overlaps
- Dual shift functionality with visual feedback

### Known Issues to be Aware Of
- **Backtick Key**: Displays and produces apostrophe (') instead of backtick (`)
- **Text Wrapping**: Long text input may extend beyond text box boundaries
- **Text Length**: No automatic limiting of input length

## Contributing
Contributions welcome for:
- **Text Display Improvements**: Auto-wrapping, scrolling, or multi-line support
- **Character Rendering**: Alternative solutions for backtick character display
- Additional language support (Japanese, Chinese)
- Advanced UI features (themes, customization)
- Performance optimizations and memory usage
- Cross-platform compatibility improvements
- Font rendering enhancements
- Accessibility features

## Testing
To verify all features work correctly:
1. **ASCII Character Display**: Test all first row characters (note: first key shows ' instead of `)
2. **Korean Composition**: Verify compound vowels (try: ㄱ+ㅜ+ㅓ+ㄴ = 권)
3. **Dual Shift**: Check both left and right shift buttons work
4. **Mode Switching**: Test Korean ↔ English toggle (한글/Eng button)
5. **Text Display**: Verify characters appear in both buttons and text box
6. **Text Length**: Test behavior with long input (observe wrapping issues)
7. **Character Mapping**: Confirm first key produces ' (apostrophe) not ` (backtick)

### Expected Behavior
- ✅ All buttons display visible characters
- ✅ Korean composition works correctly
- ✅ Mode switching changes button labels appropriately
- ⚠️ First key produces apostrophe, not backtick
- ⚠️ Very long text may extend beyond text box boundaries

---
**Author**: Suho Kwon(suho.kwon@gmail.com)
**Framework**: MiniGUI 5.0+ with FreeType2 and HarfBuzz  
**Platform**: Linux (Ubuntu 24.04+ recommended)  
**Version**: 1.1 - Enhanced Character Display  
**Last Updated**: October 15, 2025