# How to See Korean Characters in MiniGUI Windows

## What I Fixed

1. **Updated MiniGUI.cfg** - Added TrueType font configuration to load NanumGothic fonts
2. **Updated font loading code** - Improved the `chunjiin_load_korean_font()` function to properly load Korean fonts
3. **Rebuilt the application** - Recompiled with the new font loading code

## Korean Characters in Your Application

Your buttons and textbox already have Korean characters in the source code:

### Button Labels:
- **Row 1** (Fundamental Elements):
  - Button 0: `ㅣ` (vertical line - human/인)
  - Button 1: `ㆍ` (dot - heaven/천)
  - Button 2: `ㅡ` (horizontal line - earth/지)

- **Row 2** (Basic Consonants):
  - Button 3: `ㄱ,ㅋ` (giyeok, kieok)
  - Button 4: `ㄴ,ㄹ` (nieun, rieul)
  - Button 5: `ㄷ,ㅌ` (digeut, tieut)

- **Row 3** (More Consonants):
  - Button 6: `ㅂ,ㅍ` (bieup, pieup)
  - Button 7: `ㅅ,ㅎ` (siot, hieut)
  - Button 8: `ㅈ,ㅊ` (jieut, chieut)

- **Row 4**:
  - Button 9: `Space`
  - Button 10: `ㅇ,ㅁ` (ieung, mieum)
  - Button 11: `Back`

- **Row 5**:
  - Button 12: `Enter`
  - Button 13: `Clear`

## How to Run the Application

Run the application with:

```bash
./run.sh
```

## What to Look For

When the application window opens, you should see:

1. **Window Title**: "ChunJiIn Korean Input System"
2. **Text Box** (top): Shows Korean text you type
3. **Buttons**: Should display Korean characters (ㅣ, ㆍ, ㅡ, ㄱ,ㅋ, etc.)

## If Korean Characters Still Don't Display

Check the terminal output for font loading messages:
- ✓ Loaded NanumGothic-Regular TrueType font for Korean characters
- ⚠ Warning: Could not load Korean font...

If you see the warning, the issue might be:
1. Font file paths in MiniGUI.cfg
2. MiniGUI FreeType support
3. Permission issues accessing font files

## Verify Fonts Are Present

```bash
ls -lh ./install/share/fonts/
```

You should see:
- NanumGothic-Regular.ttf
- NanumGothic-Bold.ttf
- NanumGothic-ExtraBold.ttf

## Test the Input System

Once the window opens:
1. Click the `ㅣ`, `ㆍ`, `ㅡ` buttons to test fundamental elements
2. Click consonant buttons like `ㄱ,ㅋ` to cycle through consonants
3. The text box should display Korean characters as you type

## Example: Type "한글" (Hangul)

To type 한 (han):
1. Click `ㅅ,ㅎ` twice to get ㅎ
2. Click `ㅏ` (made from ㅣ + ㆍ)
3. Click `ㄴ,ㄹ` to get ㄴ

The ChunJiIn input method will combine these into the Korean syllable 한!
