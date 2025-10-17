# MAC Address Hexadecimal Input with LVGL

A hexadecimal input application for entering and validating physical Ethernet (MAC) addresses, built with LVGL (Light and Versatile Graphics Library) and SDL2.

## Features

- **Hexadecimal Keypad**: Clean 4×4 grid layout with buttons 0-9 and A-F
- **Automatic MAC Address Formatting**: Real-time formatting as `XX:XX:XX:XX:XX:XX`
- **Input Validation**: Validates complete 12 hex digits (6 bytes) with visual feedback
- **Icon-Based Feedback**: 
  - ✓ Green checkmark for valid MAC addresses
  - ❌ Red X for invalid/incomplete addresses
- **Control Buttons**: Backspace, Clear, and Enter with color coding
- **Compact Portrait Display**: Optimized for 320×640 resolution
- **Modular Architecture**: Separated GUI and MAC address processing logic
- **Fast Build System**: Pre-compiled LVGL library for quick development cycles

## Requirements

- **GCC compiler**
- **Make**
- **SDL2 development libraries** (libsdl2-dev)
- **LVGL v9.x** (automatically cloned by setup script)

## Quick Start

### 1. Run Setup Script (One Time)

The setup script will install dependencies and build LVGL library:

```bash
./setup.sh
```

This will:
- Check for and install required build tools (gcc, make, pkg-config)
- Install SDL2 development libraries if missing
- Clone LVGL v9.2 into the project directory
- **Compile LVGL into a static library** (takes 2-3 minutes, only once)
- Verify lv_conf.h configuration

### 2. Build the Application (Fast!)

```bash
make
```

Subsequent builds are **very fast** (< 1 second) because LVGL is already compiled!

### 3. Run the Application

```bash
./hexinput
```

Or:
```bash
make run
```

## Build System

The project uses a two-stage build system for optimal development speed:

### Stage 1: Setup (Run Once)
```bash
./setup.sh
```
- Compiles all LVGL sources into `lvgl/build/liblvgl.a` (~1.8 MB)
- Takes 2-3 minutes
- Only needs to be run once or when LVGL needs updating

### Stage 2: Application Build (Run Every Time)
```bash
make
```
- Only compiles application files (`main.c`, `mac_input.c`)
- Links against pre-built LVGL library
- **Takes < 1 second!** ⚡

### Build Commands

- `make` - Build application only (fast!)
- `make clean` - Clean application objects only
- `make clean-all` - Clean application + LVGL library
- `make run` - Build and run

## Usage

### Keypad Layout

The hexadecimal keypad is arranged in a 4×4 grid:

```
    1  2  3  4
    5  6  7  8
    9  A  B  C
    0  D  E  F
```

### Control Buttons

Bottom row (color-coded):
- **← (Backspace)**: Delete last hex digit - **Orange**
- **CLR (Clear)**: Clear all input - **Red**
- **ENTER**: Validate and submit - **Green** (wider button)

### Display

- **Title**: "MAC Address Input"
- **Status**: Shows digit count (e.g., "Digits: 8/12")
- **Text Area**: Displays formatted MAC address with auto-inserted colons

### MAC Address Format

As you type hex digits, the display automatically formats them:
```
Type: A → Display: A
Type: B → Display: AB
Type: C → Display: AB:C
Type: D → Display: AB:CD
...
Type: 45 → Display: AB:CD:EF:01:23:45
```

- Colons are automatically inserted after every 2 digits
- Maximum 12 hex digits (6 bytes)
- Real-time digit counter

### Validation

Press **ENTER** to validate:

#### ✓ Valid MAC Address (12 digits)
```
┌─────────────────────────┐
│ ✓  MAC Address         │  GREEN
├─────────────────────────┤
│                        │
│  AB:CD:EF:01:23:45    │
│                        │
│        [Close]         │
└─────────────────────────┘
```
- Shows green checkmark icon
- Displays the MAC address
- Clears input for next entry

#### ❌ Invalid/Incomplete MAC Address
```
┌─────────────────────────┐
│ ❌  Invalid MAC Address │  RED
├─────────────────────────┤
│ Incomplete MAC address: │
│ AB:CD:EF:01            │
│                        │
│ Entered: 8/12 digits   │
│ Please complete...     │
│                        │
│        [Close]         │
└─────────────────────────┘
```
- Shows red X icon
- Explains the error
- **Does NOT clear input** - allows you to continue

### Example Workflow

1. **Enter digits**: Click `A`, `B`, `C`, `D`, `E`, `F`
2. **Display shows**: `AB:CD:EF`
3. **Continue**: Click `0`, `1`, `2`, `3`, `4`, `5`
4. **Display shows**: `AB:CD:EF:01:23:45`
5. **Press ENTER**: Shows success with green checkmark
6. **Input clears**: Ready for next MAC address

## Project Structure

```
HexaInput/
├── main.c              # GUI implementation (LVGL interface)
├── mac_input.c         # MAC address processing logic
├── mac_input.h         # MAC address API header
├── lv_conf.h           # LVGL configuration (v9.2.0)
├── Makefile            # Application build configuration
├── setup.sh            # LVGL library build script
├── assets/             # Font files (NanumGothicCoding)
│   ├── NanumGothicCoding.ttf
│   └── NanumGothicCoding-Bold.ttf
├── lvgl/               # LVGL library (cloned by setup.sh)
│   └── build/
│       └── liblvgl.a   # Pre-compiled LVGL library
├── LICENSE
└── README.md           # This file
```

## Architecture

### Modular Design

The project separates concerns into distinct modules:

#### `mac_input.c/.h` - MAC Address Processing
- Pure C logic, no GUI dependencies
- MAC address data structure and operations
- Formatting (raw → `XX:XX:XX:XX:XX:XX`)
- Validation with error message generation
- Reusable in other projects

#### `main.c` - GUI Layer
- LVGL initialization and display setup
- Hex keypad creation and layout
- Event handling and callbacks
- Display updates
- Uses MAC input API

### API Functions

```c
// Initialize MAC address structure
void mac_init(MacAddress *mac);

// Add hex digit (0-9, A-F)
void mac_add_digit(MacAddress *mac, char digit);

// Delete last digit
void mac_delete_last(MacAddress *mac);

// Clear all input
void mac_clear(MacAddress *mac);

// Get formatted string (XX:XX:XX:XX:XX:XX)
void mac_get_formatted(const MacAddress *mac, char *output, size_t size);

// Validate and get error message
int mac_validate(const MacAddress *mac, char *error_msg, size_t size);

// Check if complete (12 digits)
int mac_is_valid(const MacAddress *mac);
```

## Configuration

### Display Resolution

Current resolution: **320×640 pixels** (portrait)

To change, edit in `main.c`:

```c
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 640
```

### Button Size

To change button dimensions, edit in `create_gui()`:

```c
const int btn_size = 55;  // Width and height in pixels
const int gap = 5;        // Gap between buttons
```

### Font Sizes

Current font configuration:
- **Title**: Montserrat 14pt
- **Status**: Montserrat 10pt
- **Text Area**: Montserrat 16pt
- **Buttons**: Montserrat 16pt

To change fonts, edit the `lv_obj_set_style_text_font()` calls in `create_gui()`.

### Button Colors

Control button colors in `create_gui()`:

```c
// Backspace - Orange
lv_obj_set_style_bg_color(backspace_btn, lv_palette_main(LV_PALETTE_ORANGE), 0);

// Clear - Red  
lv_obj_set_style_bg_color(clear_btn, lv_palette_main(LV_PALETTE_RED), 0);

// Enter - Green
lv_obj_set_style_bg_color(enter_btn, lv_palette_main(LV_PALETTE_GREEN), 0);
```

## LVGL Configuration

The project includes a pre-configured `lv_conf.h` file with:
- **LVGL version**: v9.2.0
- **Color depth**: 32-bit (XRGB8888)
- **SDL driver**: Enabled (LV_USE_SDL = 1)
- **Memory pool**: 256KB
- **Fonts enabled**: Montserrat 10, 14, 16, 20, 24, 28

## Troubleshooting

### "LVGL library not found" Error

Run the setup script:
```bash
./setup.sh
```

### "SDL not found" Error

Install SDL2 development libraries:
```bash
sudo apt-get install libsdl2-dev
```

### Compilation Errors

1. Verify `lv_conf.h` exists in the project directory
2. Check that `lvgl/build/liblvgl.a` exists (run `./setup.sh`)
3. Run `make clean` and then `make`

### Slow First Build

This is normal! The first time you run `./setup.sh`, it compiles all LVGL sources (~2-3 minutes). After that, `make` is very fast (< 1 second).

### Rebuild LVGL Library

If you need to rebuild the LVGL library:
```bash
make clean-all    # Remove LVGL library
./setup.sh        # Rebuild LVGL library
make              # Build application
```

## Development

### Making Changes

```bash
# Edit source files
nano main.c
nano mac_input.c

# Quick rebuild (< 1 second)
make

# Run
./hexinput
```

### Adding New Features

1. **GUI changes**: Edit `main.c`
2. **MAC logic changes**: Edit `mac_input.c` and `mac_input.h`
3. **LVGL config**: Edit `lv_conf.h`, then run `./setup.sh` and `make clean-all`

### Testing

Test validation behavior:
- Try entering 0-11 digits → Press ENTER → Should show red X
- Enter exactly 12 digits → Press ENTER → Should show green checkmark

## Technical Details

### Technologies Used

- **LVGL**: v9.2 (Light and Versatile Graphics Library)
- **SDL2**: Display and input handling  
- **C11**: Programming language standard

### Build System

- **Static Library**: LVGL compiled once into `liblvgl.a`
- **Incremental Builds**: Only changed files recompile
- **Fast Iteration**: < 1 second rebuild time

### Memory Usage

- **LVGL pool**: 256KB (configured in lv_conf.h)
- **LVGL library**: 1.8MB (static)
- **Application**: ~843KB (executable)
- **Typical runtime**: ~8-10MB RAM

### Performance

- **Frame rate**: 30 FPS (SDL VSync)
- **Input latency**: < 5ms
- **Rendering**: Hardware accelerated (SDL2)
- **Build time**: < 1 second (after initial setup)

## License

See [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Please feel free to submit issues or pull requests.

## Credits

- **LVGL**: https://lvgl.io/
- **SDL2**: https://www.libsdl.org/

## References

- [LVGL Documentation](https://docs.lvgl.io/)
- [LVGL v9 Migration Guide](https://docs.lvgl.io/master/CHANGELOG.html)
- [SDL2 Documentation](https://wiki.libsdl.org/)
- [MAC Address Format](https://en.wikipedia.org/wiki/MAC_address)

## Version History

### Current Version
- Hexadecimal input system for MAC addresses
- 320×640 portrait display
- Icon-based validation feedback
- Modular architecture (GUI + processing logic)
- Fast build system with pre-compiled LVGL library
- Color-coded control buttons
- Real-time formatting with auto-inserted colons
