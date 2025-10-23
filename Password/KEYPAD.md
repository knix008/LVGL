# Mobile-Style Keypad Documentation

## Overview

The application now includes a mobile-style on-screen keypad (3 columns × 5 rows) with multiple input modes, making it ideal for touchscreen devices or systems without a physical keyboard.

## Keypad Layout

### Physical Layout
```
┌─────────────────────────────┐
│                             │
│   Keypad (3×5 grid)         │
│   ┌───┬───┬───┐             │
│   │ a │ b │ c │  Row 0      │
│   ├───┼───┼───┤             │
│   │ d │ e │ f │  Row 1      │
│   ├───┼───┼───┤             │
│   │ g │ h │ i │  Row 2      │
│   ├───┼───┼───┤             │
│   │ j │ k │ l │  Row 3      │
│   ├───┼───┼───┤             │
│   │ABC│123│ #+│  Row 4 (Mode Switch)
│   └───┴───┴───┘             │
├─────────────────────────────┤
│  Function Buttons           │
│  [  ⌫  ][  ␣  ]            │
└─────────────────────────────┘
```

**Note**: The bottom row of the keypad contains mode switching buttons integrated directly into the grid.

## Input Modes

The keypad supports **2 main input modes** with sub-modes controlled by the **SHIFT** key. All function keys are integrated into the 3×5 grid - no separate buttons needed!

### 1. English Mode (Switchable with `ABC` button)

#### Lowercase (default, SHIFT inactive)
```
abc   def   ghi
jkl   mno   pqr
stu   vwx   yz
@.-   _     [Space]
SHIFT 123   ←
```
- **T9-style input**: Press a key multiple times to cycle through letters (max 3 chars)
  - Example: Press "abc" 3 times within 1 second: a → b → c
  - Example: Press "yz" 2 times within 1 second: y → z
- **Special symbols**: @.- button cycles through @, ., -
- **SHIFT key**: Toggles to uppercase mode
- **123 button**: Switches to Numbers/Special mode
- **← (Backspace)**: Deletes last character

#### Uppercase (SHIFT active)
```
ABC   DEF   GHI
JKL   MNO   PQR
STU   VWX   YZ
@.-   _     [Space]
SHIFT 123   ←
```
- **T9-style input**: Press a key multiple times to cycle through letters (max 3 chars)
  - Example: Press "ABC" 3 times within 1 second: A → B → C
  - Example: Press "YZ" 2 times within 1 second: Y → Z
- **Special symbols**: @.- button cycles through @, ., -
- **SHIFT key** (orange): Toggles back to lowercase mode
- **123 button**: Switches to Numbers/Special mode
- **← (Backspace)**: Deletes last character

### 2. Numbers/Special Mode (Switchable with `123` button)

#### Numbers (default, SHIFT inactive)
```
1     2     3
4     5     6
7     8     9
.     0     [Space]
SHIFT ABC   ←
```
- **Direct input**: Each key enters a single character
- **Space key**: Bottom-right position for easy access
- **SHIFT key**: Toggles to special characters mode
- **ABC button**: Switches to English mode
- **← (Backspace)**: Deletes last character

#### Special Characters (SHIFT active)
```
!     @     #
$     %     ^
&     *     (
)     -     _
SHIFT ABC   ←
```
- **Direct input**: Each key enters a single character
- **SHIFT key** (orange): Toggles back to numbers mode
- **ABC button**: Switches to English mode
- **← (Backspace)**: Deletes last character

## Function Keys (Integrated in Keypad)

**All function keys are integrated into the 3×5 keypad grid** - there are no separate button panels!

| Button | Location | Function | Description |
|--------|----------|----------|-------------|
| `SHIFT` | Row 4, Col 0 | Toggle Sub-mode | In English mode: toggle lowercase ⟷ uppercase<br>In Numbers/Special mode: toggle numbers ⟷ special chars |
| `123` | Row 4, Col 1 | Mode Switch | Switches to Numbers/Special input mode (available in English mode) |
| `ABC` | Row 4, Col 1 | Mode Switch | Switches to English input mode (available in Numbers/Special mode) |
| `←` | Row 4, Col 2 | Backspace | Deletes the last character from input |
| `[Space]` | Row 3, Col 2 | Space | Inserts a space character (available in Numbers mode) |

**Visual Indicators**:
- **SHIFT button color**:
  - Blue: Shift inactive (lowercase or numbers)
  - Orange: Shift active (uppercase or special characters)

**Bottom Row Layouts by Mode**:
- In English mode (lowercase/uppercase): `SHIFT | 123 | ←`
- In Numbers/Special mode: `SHIFT | ABC | ←`

## Usage

### Entering User ID
1. Tap on the "User ID" input field
2. The keypad will activate for that field
3. The keypad starts in English lowercase mode by default
4. To enter letters:
   - Lowercase: Use keypad as-is, press keys multiple times for T9-style cycling
   - Uppercase: Press `SHIFT` button (turns orange), then press keys
5. To enter numbers/symbols: Press `123` button
6. Use ⌫ to delete mistakes
7. Use Space button for spaces

### Entering Password
1. Tap on the "Password" input field
2. The keypad will activate for that field
3. Remember password requirements:
   - At least 11 characters
   - One or more capital letters (use `SHIFT` in English mode)
   - One or more numbers (use `123` mode)
   - One or more special characters (use `SHIFT` in Numbers/Special mode)
4. Switch between modes and use SHIFT as needed

### Example Password Entry Flow
To enter `MyPass123!`:
1. Tap password field
2. Starting in lowercase mode, press `SHIFT` → keypad shows uppercase
3. Press "MNO" once → `M`
4. Press `SHIFT` again → keypad shows lowercase
5. Press "wxyz" twice → `y`
6. Press `SHIFT` → uppercase
7. Press "PQRS" once → `P`
8. Press `SHIFT` → lowercase
9. Press "abc" once → `a`
10. Press "tuv" three times → `s`
11. Press "tuv" three times → `s`
12. Press `123` button → switches to numbers mode
13. Press `1`, `2`, `3`
14. Press `SHIFT` → switches to special characters
15. Press `!` button

## Screen Layout

```
┌─────────────────────────────┐ 0
│  Login                       │
│                              │
│  User ID:                    │
│  [_________________]         │
│  ✓ Validation msg            │
│                              │
│  Password:                   │
│  [•••••••••••••••••]         │
│  Length: 0/11 | ✓✓✓          │
│                              │
│  [   Login Button   ]        │
│                              │
│  Status messages here        │
├─────────────────────────────┤ 270
│  ┌───────┬───────┬───────┐  │
│  │  abc  │  def  │  ghi  │  │
│  ├───────┼───────┼───────┤  │
│  │  jkl  │  mno  │  pqr  │  │
│  ├───────┼───────┼───────┤  │
│  │  stu  │  vwx  │  yz   │  │
│  ├───────┼───────┼───────┤  │
│  │  @.-  │   _   │ Space │  │
│  ├───────┼───────┼───────┤  │
│  │ SHIFT │  123  │   ←   │  │
│  └───────┴───────┴───────┘  │
│                              │
└─────────────────────────────┘ 640
```
**Note**: All function keys (SHIFT, Backspace ←, Mode switching) are integrated into the 3×5 keypad grid. No separate function panel needed!

## Technical Details

### Keypad Architecture

```
keypad.h / keypad.c
├── keypad_create()          - Creates 3x5 button grid
├── keypad_set_target()      - Sets target textarea
├── keypad_set_mode()        - Changes input mode
├── keypad_get_mode()        - Gets current mode
└── keypad_delete()          - Cleanup

password_gui.c
├── textarea_focus_event_handler()  - Switches keypad target
├── mode_button_event_handler()     - Switches input mode
└── special_key_event_handler()     - Handles backspace/space
```

### Key Features

1. **Auto-targeting**: When you tap a textarea (User ID or Password), the keypad automatically targets that field

2. **Mode persistence**: Selected mode stays active when switching between textareas

3. **Visual feedback**: Buttons provide visual feedback on press

4. **Password masking**: Password field masks characters while maintaining validation

5. **Real-time validation**: Password requirements update live as you type

## Module Integration

### Files Added
- `keypad.h` - Keypad interface
- `keypad.c` - Keypad implementation

### Files Modified
- `password_gui.c` - Integrated keypad into GUI
- `Makefile` - Added keypad.c to build

### Dependencies
```
keypad.c
  └── Depends on: lvgl/lvgl.h

password_gui.c
  ├── Depends on: keypad.h
  ├── Depends on: password.h
  └── Depends on: lvgl/lvgl.h
```

## Customization

### Changing Keypad Size

In `password_gui.c`, modify:
```c
keypad_config_t keypad_cfg = {
    .width = WINDOW_WIDTH - 10,
    .height = 235,  // Change this value
    .parent = screen,
    .target_textarea = userid_textarea
};
```

### Adding More Keys

To expand beyond 3×5, modify `keypad.c`:
1. Change `KEYPAD_COLS` and `KEYPAD_ROWS` constants
2. Update `keypad_layouts` arrays
3. Adjust button size calculations

### Adding More Modes

1. Add new mode to `keypad_mode_t` enum in `keypad.h`
2. Add layout to `keypad_layouts` array in `keypad.c`
3. Add mode button in `password_gui.c`

## Testing

### Test Cases

1. **Basic Input**:
   - Enter "test" in User ID using lowercase mode
   - Verify text appears correctly

2. **Mode Switching**:
   - Enter "Test123!" using multiple modes
   - Verify all characters entered correctly

3. **Field Switching**:
   - Enter text in User ID
   - Tap Password field
   - Verify keypad now types into Password field

4. **Special Functions**:
   - Enter "abc"
   - Press Backspace → verify shows "ab"
   - Press Space → verify shows "ab "

5. **Password Validation**:
   - Enter "MyPassword123!"
   - Verify all validation indicators turn green
   - Verify Login button becomes enabled

## Known Limitations

1. No multi-page navigation for lowercase/uppercase modes (p-z on same page)
2. No clipboard support (copy/paste)
3. No autocomplete or suggestions
4. No long-press for alternate characters

## Future Enhancements

- Add swipe gestures for mode switching
- Implement multi-page navigation with arrow buttons
- Add character preview on key press
- Support for international characters
- Haptic feedback (if supported by platform)
