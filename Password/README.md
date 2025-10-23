# Password Login System with LVGL

A user authentication interface built with LVGL (Light and Versatile Graphics Library) featuring password validation, mobile-style on-screen keypad, and popup notifications.

## Features

- **Window Size**: 320x640 pixels (mobile portrait orientation)
- **Fully Integrated 3×5 Keypad**: All functions in a single grid
  - 2 main input modes: English and Numbers/Special
  - SHIFT key with dual functionality (case toggle / number-special toggle)
  - T9-style multi-tap input (max 3 characters per button)
  - Integrated Backspace (←), Mode switching (123/ABC), and Space
  - Color-coded function buttons (Green SHIFT, Purple Mode, Red Backspace)
- **User ID Input**: Username or email with 32-character limit
- **Password Input**: Secure masked input with 32-character limit
- **Popup Notifications**: Modal dialogs for validation errors and login results
- **Clean Interface**: Minimal design without inline validation labels
- **Touch-Friendly**: Optimized for touchscreen or mouse input
- **Korean Font Support**: Uses NanumGothicCoding fonts loaded via FreeType
- **Comprehensive Tests**: Unit tests for validation logic

## User ID Requirements

The User ID must meet the following criteria:

- **Length**: Between 3 and 32 characters (enforced by input field)
- **First Character**: Must be alphanumeric (letter or number)
- **Allowed Characters**: Letters (a-z, A-Z), numbers (0-9), and special characters (. - _ @)
- **Email Format**: If the User ID contains @, it must be a valid email format
  - At least one character before the @
  - At least one character after the @
  - A domain with a dot (e.g., user@example.com)

### Valid User ID Examples

- `john123` (username)
- `user.name` (username with dot)
- `user@example.com` (email)
- `admin-user_2024` (username with special chars)

### Invalid User ID Examples

- `ab` (too short - minimum 3 characters)
- `@user` (cannot start with special character)
- `user!name` (! is not allowed)
- `user@com` (invalid email - missing domain dot)

## Password Requirements

The password must meet ALL of the following criteria:

- **Length**: Between 11 and 32 characters (enforced by input field)
- **Capital Letters**: At least one uppercase letter (A-Z)
- **Numbers**: At least one digit (0-9)
- **Special Characters**: At least one special character (!@#$%^&*, etc.)

## Project Structure

```
Password/
├── main.c                          # Main application entry point
├── password.h / password.c         # Password validation logic (LVGL-independent)
├── password_gui.h / password_gui.c # GUI implementation (LVGL-dependent)
├── keypad.h / keypad.c             # Mobile-style keypad (3×5 grid)
├── Makefile                        # Build configuration
├── setup.sh                        # LVGL setup script
├── lv_conf.h                       # LVGL configuration
├── .gitignore                      # Git ignore rules
├── README.md                       # This file
├── SUMMARY.md                      # Project summary
├── KEYPAD.md                       # Keypad usage guide
├── assets/                         # Font files
│   ├── NanumGothicCoding.ttf      # Regular Korean font
│   └── NanumGothicCoding-Bold.ttf # Bold Korean font
├── tests/                          # Unit tests
│   ├── test_password_validation.c # Password validation tests
│   ├── test_userid_validation.c   # User ID validation tests
│   ├── Makefile                   # Test build configuration
│   ├── run_test.sh                # Test runner script
│   └── README.md                  # Test documentation
└── lvgl/                          # LVGL library (cloned during setup)
```

## Requirements

### System Dependencies

- GCC compiler
- SDL2 development libraries
- FreeType development libraries
- Git

### Installation (Ubuntu/Debian)

```bash
sudo apt-get update
sudo apt-get install -y build-essential git libsdl2-dev libfreetype6-dev pkg-config
```

Or use the Makefile target:

```bash
make install-deps
```

## Building the Application

### 1. First-time Setup

Run the setup script to clone LVGL and build the library:

```bash
bash setup.sh
```

This script will:
- Check for required system packages
- Verify Korean font files exist
- Clone LVGL v9.2 from GitHub
- Build the LVGL static library
- Configure FreeType support

### 2. Build the Application

```bash
make
```

### 3. Run the Application

```bash
./password_login
```

Or build and run in one step:

```bash
make run
```

## Usage

### With On-Screen Keypad (Default)

1. **Launch the application**: Run `./password_login`
2. **Enter User ID**:
   - Tap/click on the User ID field
   - Use the on-screen keypad to enter your username or email
   - **SHIFT button** (green): Not used in number mode
   - **123 button** (purple): Switch to Numbers/Special mode
   - **← button** (red): Backspace to delete characters
   - Maximum 32 characters allowed
3. **Enter Password**:
   - Tap/click on the Password field
   - Use the keypad with mode switching:
     - **SHIFT** (green → orange): Toggle uppercase in English mode, or toggle special chars in number mode
     - **123** (purple): Switch to Numbers/Special mode
     - **ABC** (purple): Switch to English mode
   - T9-style input: Press buttons multiple times to cycle characters (max 3 per button)
   - Example: For "MyPass123!":
     - SHIFT → M, SHIFT → y, SHIFT → P, SHIFT → a, s, s, 123 → 1, 2, 3, SHIFT → !
4. **Login**: Click the Login button (always enabled)
5. **View Results**: Validation results and login status appear in popup dialogs

### With Physical Keyboard

You can also use your physical keyboard directly by clicking in the field and typing normally.

### Keypad Layout (3×5 Grid)

**English Mode:**
```
abc   def   ghi
jkl   mno   pqr
stu   vwx   yz
@     _     .
SHIFT 123   ←
```

**Numbers Mode:**
```
1     2     3
4     5     6
7     8     9
@     0     .
SHIFT ABC   ←
```

**Bottom Row Functions:**
- **SHIFT** (green/orange): Toggle case or number/special
- **123/ABC** (purple): Switch between English and Numbers/Special modes
- **←** (red): Backspace

For detailed keypad documentation, see [KEYPAD.md](KEYPAD.md).

## Example Valid Passwords

- `MyPassword123!`
- `SecurePass2024@`
- `Admin#2024Pass`
- `Test12345678$A`

## Example Invalid Passwords

- `password123!` (no capital letter)
- `PASSWORD123!` (no lowercase - wait, we don't require lowercase!)
- `MyPassword!` (too short, no number)
- `MyPassword123` (no special character)

## Makefile Targets

| Target | Description |
|--------|-------------|
| `make` or `make all` | Build the application and tests |
| `make clean` | Remove build artifacts (app + tests) |
| `make distclean` | Remove all build files including LVGL |
| `make run` | Build and run the application |
| `make test` | Run all unit tests |
| `make build-tests` | Build tests only |
| `make clean-tests` | Clean test artifacts only |
| `make install-deps` | Install system dependencies |
| `make help` | Show help message |

## Code Structure

### Module Overview

**password.c / password.h** (Logic - LVGL-independent)
- `password_validate()`: Validates password meets all requirements
- `password_get_validation_info()`: Returns detailed validation status
- `userid_validate()`: Validates User ID format
- `userid_is_email_format()`: Checks if User ID is email format

**password_gui.c / password_gui.h** (GUI - LVGL-dependent)
- `gui_init()`: Initializes LVGL, display, and input devices
- `gui_create_login_screen()`: Builds the login UI
- `gui_run()`: Main event loop
- `show_popup_message()`: Displays modal popup notifications
- `login_btn_event_handler()`: Processes login with validation
- `load_fonts()`: Loads Korean TrueType fonts via FreeType

**keypad.c / keypad.h** (Mobile Keypad)
- `keypad_create()`: Creates 3×5 integrated keypad
- `keypad_set_mode()`: Switches input modes (English/Numbers/Special)
- `keypad_set_target()`: Sets target textarea
- T9-style multi-tap character input
- Buffer length checking (max 32 chars)

**main.c** (Application Entry)
- Minimal main function
- Initializes GUI and starts event loop
- Login callback handler

## Testing

### Run All Tests

```bash
make test
```

Or from the tests directory:

```bash
cd tests
./run_test.sh
```

### Test Coverage

- **23 unit tests** total
- **10 password validation tests**
- **13 User ID validation tests**
- 100% pass rate

See [tests/README.md](tests/README.md) for detailed test documentation.

## Configuration

### Window Size

To change the window size, modify these constants in `password_gui.h`:

```c
#define WINDOW_WIDTH  320
#define WINDOW_HEIGHT 640
```

### Input Length Limits

Maximum character limits are set in `password_gui.c`:

```c
lv_textarea_set_max_length(userid_textarea, 32);
lv_textarea_set_max_length(password_textarea, 32);
```

### Password Requirements

To adjust password validation rules, modify in `password.h`:

```c
#define MIN_PASSWORD_LENGTH 11
```

And update the validation logic in `password.c`.

### Fonts

The application uses FreeType to load fonts at runtime. Font files should be placed in the `assets/` directory:

- `assets/NanumGothicCoding.ttf` - Regular font (16px)
- `assets/NanumGothicCoding-Bold.ttf` - Bold font (24px)

## Troubleshooting

### Build Errors

**Error: LVGL library not found**
```bash
bash setup.sh
```

**Error: SDL2 not found**
```bash
sudo apt-get install libsdl2-dev
```

**Error: FreeType not found**
```bash
sudo apt-get install libfreetype6-dev
```

### Runtime Errors

**Warning: Failed to load fonts**
- Ensure font files exist in `assets/` directory
- The application will fall back to default LVGL fonts

**SDL window creation failed**
- Check if display is available (required for GUI)
- Ensure SDL2 is properly installed

## Technical Details

- **LVGL Version**: v9.2
- **Display Driver**: SDL2
- **Font Rendering**: FreeType
- **Color Depth**: 32-bit (XRGB8888)
- **Input Devices**: Mouse and Keyboard via SDL

## References

- [LVGL Documentation](https://docs.lvgl.io/)
- [LVGL GitHub](https://github.com/lvgl/lvgl)
- [SDL2 Documentation](https://wiki.libsdl.org/)
- [FreeType Documentation](https://freetype.org/freetype2/docs/)
