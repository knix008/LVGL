# SVG Assets

This directory contains simple SVG files for testing and demonstration purposes.

## Files

### Basic Shapes
- **simple_circle.svg** - A blue circle with black border
- **simple_rectangle.svg** - A red rectangle with black border
- **simple_triangle.svg** - A green triangle with black border
- **simple_star.svg** - A yellow star with orange border

### Text and Complex Designs
- **simple_text.svg** - Purple text saying "Hello SVG!" with a gray border
- **complex_design.svg** - A comprehensive design with multiple shapes, gradients, and features

## SVG Features Demonstrated

### Basic Elements
- `<circle>` - Circular shapes
- `<rect>` - Rectangular shapes
- `<polygon>` - Polygonal shapes (triangles, stars)
- `<line>` - Straight lines
- `<text>` - Text elements

### Advanced Features
- **Gradients** - Linear and radial gradients
- **Colors** - Various fill and stroke colors
- **Styling** - Stroke width, opacity, etc.
- **Positioning** - Coordinate-based positioning
- **Text Styling** - Font family, size, alignment

## Usage

These SVG files can be used to test LVGL's SVG rendering capabilities. They are designed to be simple enough for basic testing while demonstrating various SVG features.

## File Specifications

All SVG files are:
- Valid XML format
- Use standard SVG namespace
- Include proper viewport dimensions
- Use common color names and hex values
- Compatible with most SVG renderers

## Testing with LVGL

To test these SVG files with LVGL, you can load them using LVGL's SVG decoder:

```c
lv_obj_t * img = lv_img_create(lv_scr_act());
lv_img_set_src(img, "assets/simple_circle.svg");
```
