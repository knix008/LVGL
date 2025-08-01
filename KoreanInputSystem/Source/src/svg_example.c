#include "lvgl.h"

// Example SVG data - a simple blue circle
static const char * example_svg_data = 
    "<svg width='100' height='100' xmlns='http://www.w3.org/2000/svg'>"
    "<circle cx='50' cy='50' r='40' fill='#2196F3'/>"
    "<text x='50' y='60' text-anchor='middle' fill='white' font-size='24'>i</text>"
    "</svg>";

// Example function to create an SVG image widget
// Note: This requires LV_USE_SVG = 1 and LV_USE_VECTOR_GRAPHIC = 1 in lv_conf.h
void create_svg_example(lv_obj_t * parent) {
#if LV_USE_SVG && LV_USE_VECTOR_GRAPHIC
    // Create an image widget
    lv_obj_t * svg_img = lv_image_create(parent);
    
    // Set the SVG data as the image source
    lv_image_set_src(svg_img, example_svg_data);
    
    // Position the image
    lv_obj_align(svg_img, LV_ALIGN_CENTER, 0, 0);
    
    // Set size (SVG will scale to fit)
    lv_obj_set_size(svg_img, 100, 100);
    
    // Optional: Add some styling
    lv_obj_set_style_border_width(svg_img, 2, 0);
    lv_obj_set_style_border_color(svg_img, lv_color_hex(0xCCCCCC), 0);
    lv_obj_set_style_radius(svg_img, 8, 0);
#else
    // Fallback: Create a simple colored circle if SVG is not available
    lv_obj_t * fallback_circle = lv_obj_create(parent);
    lv_obj_set_size(fallback_circle, 100, 100);
    lv_obj_align(fallback_circle, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_color(fallback_circle, lv_color_hex(0x2196F3), 0);
    lv_obj_set_style_radius(fallback_circle, 50, 0);
    lv_obj_set_style_border_width(fallback_circle, 0, 0);
    
    // Add text label
    lv_obj_t * label = lv_label_create(fallback_circle);
    lv_label_set_text(label, "i");
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
#endif
}

// Example function to load SVG from file
void create_svg_from_file_example(lv_obj_t * parent, const char * file_path) {
#if LV_USE_SVG && LV_USE_VECTOR_GRAPHIC
    // Create an image widget
    lv_obj_t * svg_img = lv_image_create(parent);
    
    // Set the SVG file as the image source
    lv_image_set_src(svg_img, file_path);
    
    // Position and size the image
    lv_obj_align(svg_img, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_size(svg_img, 150, 150);
    
    // Optional: Add some styling
    lv_obj_set_style_border_width(svg_img, 2, 0);
    lv_obj_set_style_border_color(svg_img, lv_color_hex(0xCCCCCC), 0);
    lv_obj_set_style_radius(svg_img, 8, 0);
#else
    // Fallback message
    lv_obj_t * fallback_label = lv_label_create(parent);
    lv_label_set_text(fallback_label, "SVG support not enabled");
    lv_obj_align(fallback_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_color(fallback_label, lv_color_hex(0xFF0000), 0);
#endif
} 