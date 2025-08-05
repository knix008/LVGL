# SVG Extension Usage Guide

This project now includes LVGL's SVG extension for displaying SVG graphics. The SVG extension is enabled and ready to use.

## Features

- **SVG Parsing**: Load and parse SVG files and data
- **SVG Widget Creation**: Create reusable SVG widgets
- **File Support**: Load SVG files from the filesystem
- **Error Handling**: Graceful handling of parsing errors

## Configuration

The SVG extension is already enabled in the LVGL configuration:

```c
#define LV_USE_SVG 1
#define LV_USE_SVG_ANIMATION 1
#define LV_USE_VECTOR_GRAPHIC 1
```

## Available Functions

### Basic SVG Example
```c
void create_svg_example(lv_obj_t * parent);
```
Creates a simple SVG example with a blue circle and red border.

### Load SVG from File
```c
void create_svg_from_file_example(lv_obj_t * parent, const char * file_path);
```
Loads and displays an SVG file from the specified path.

### Create SVG Widget
```c
lv_obj_t* create_svg_widget(lv_obj_t * parent, const char * file_path);
```
Creates a reusable SVG widget from a file path. Returns a container object.

## Usage Examples

### 1. Basic SVG Example
```c
#include "svg_example.h"

// Create a simple SVG example
create_svg_example(lv_scr_act());
```

### 2. Load SVG from File
```c
#include "svg_example.h"

// Load an SVG file
create_svg_from_file_example(lv_scr_act(), "assets/calendar_icon.svg");
```

### 3. Create SVG Widget
```c
#include "svg_example.h"

// Create an SVG widget
lv_obj_t * svg_widget = create_svg_widget(lv_scr_act(), "assets/clock_icon.svg");
lv_obj_align(svg_widget, LV_ALIGN_TOP_LEFT, 10, 10);
```

## Available SVG Files

The project includes several SVG files in the `assets/` directory:

- `calendar_icon.svg` - Calendar icon with grid and colored dots
- `clock_icon.svg` - Clock icon
- `settings_icon.svg` - Settings icon
- `minimal.svg` - Minimal SVG example
- `simple_test.svg` - Simple test SVG

## Building and Testing

### Build SVG Test
```bash
cd Source
make svg_test
```

### Run SVG Test
```bash
cd Source/build
./svg_test
```

### Build All Tests (including SVG)
```bash
cd Source
make tests
```

## Implementation Details

The current implementation provides:

1. **SVG Parsing**: Uses LVGL's built-in SVG parser
2. **Visual Representation**: Displays colored rectangles to represent SVG content
3. **Error Handling**: Shows error messages for failed loads or parsing
4. **File I/O**: Reads SVG files from the filesystem

## Future Enhancements

The current implementation shows the basic structure. For full SVG rendering, you would need to:

1. Parse SVG elements (circles, rectangles, paths, etc.)
2. Use LVGL's vector graphics API to render each element
3. Handle SVG transformations and styling
4. Support SVG gradients and patterns

## Dependencies

- LVGL with SVG extension enabled
- SDL2 for display and input
- Standard C library for file I/O

## Notes

- The SVG extension requires `LV_USE_VECTOR_GRAPHIC = 1`
- SVG animation support is enabled with `LV_USE_SVG_ANIMATION = 1`
- Current implementation shows placeholder graphics for SVG content
- Full SVG rendering would require additional vector graphics implementation 