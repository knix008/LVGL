#ifndef SVG_EXAMPLE_H
#define SVG_EXAMPLE_H

#include "lvgl.h"

// Example function to create an SVG image widget
// Note: This requires LV_USE_SVG = 1 and LV_USE_VECTOR_GRAPHIC = 1 in lv_conf.h
void create_svg_example(lv_obj_t * parent);

// Example function to load SVG from file
void create_svg_from_file_example(lv_obj_t * parent, const char * file_path);

// Helper function to create an SVG widget from a file path
lv_obj_t* create_svg_widget(lv_obj_t * parent, const char * file_path);

#endif // SVG_EXAMPLE_H 