# SVG Support in LVGL

This document explains how to enable and use SVG images in your LVGL project.

## Current Status

SVG support is currently **disabled** in this project. The following features need to be enabled to use SVG images:

## Required Configuration Changes

To enable SVG support, you need to modify `Source/lvgl/lv_conf.h`:

```c
// Enable vector graphics support (required for SVG)
#define LV_USE_VECTOR_GRAPHIC  1

// Enable ThorVG (vector graphics library) - choose one:
#define LV_USE_THORVG_INTERNAL 1  // Use built-in ThorVG
// OR
#define LV_USE_THORVG_EXTERNAL 1  // Use external ThorVG installation

// Enable SVG support
#define LV_USE_SVG 1
#define LV_USE_SVG_ANIMATION 0  // Optional: for animated SVGs
#define LV_USE_SVG_DEBUG 0      // Optional: for debugging
```

## Dependencies

SVG support requires:
1. **Vector Graphics Support**: `LV_USE_VECTOR_GRAPHIC = 1`
2. **ThorVG Library**: Either internal (`LV_USE_THORVG_INTERNAL = 1`) or external
3. **SVG Decoder**: `LV_USE_SVG = 1`

## How to Use SVG Images

### Method 1: SVG Data as String

```c
#include "svg_example.h"

// Create SVG image from string data
create_svg_example(parent_widget);
```

### Method 2: SVG File

```c
#include "svg_example.h"

// Create SVG image from file
create_svg_from_file_example(parent_widget, "A:assets/logo.svg");
```

### Method 3: Direct LVGL API

```c
// Create image widget
lv_obj_t * svg_img = lv_image_create(parent);

// Set SVG source (string data)
lv_image_set_src(svg_img, svg_data_string);

// OR set SVG source (file path)
lv_image_set_src(svg_img, "A:path/to/image.svg");

// Position and size
lv_obj_align(svg_img, LV_ALIGN_CENTER, 0, 0);
lv_obj_set_size(svg_img, 100, 100);
```

## Current Implementation

Since SVG support is disabled, the info tab currently uses a **programmatically created logo** made with basic LVGL shapes:

- Blue circular background
- White inner circle
- Information "i" symbol made with shapes
- Modern, clean design

## Files

- `Source/src/tab_info.c` - Current info tab with programmatic logo
- `Source/src/svg_example.c` - Example SVG usage functions
- `Source/include/svg_example.h` - SVG example header
- `Source/assets/logo.svg` - Sample SVG logo file
- `Source/SVG_README.md` - This documentation

## Benefits of SVG

1. **Scalable**: Images look crisp at any size
2. **Small file size**: Vector format is typically smaller than bitmap
3. **Editable**: Can be modified with text editors or design tools
4. **Animatable**: Can include animations (if enabled)
5. **Accessible**: Text-based format is human-readable

## Trade-offs

1. **Complexity**: Requires additional dependencies (ThorVG)
2. **Build time**: Increases compilation time
3. **Memory**: May use more runtime memory for complex SVGs
4. **Performance**: Rendering can be slower than bitmap images

## Recommendation

For this project, the current programmatic logo approach is recommended because:

1. **No additional dependencies** required
2. **Fast rendering** performance
3. **Small memory footprint**
4. **Easy to modify** programmatically
5. **Consistent with LVGL design patterns**

If you need more complex graphics or want to use existing SVG assets, consider enabling SVG support following the configuration steps above. 