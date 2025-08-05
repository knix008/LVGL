#include "svg_example.h"
#include "lvgl.h"
#include "lv_svg.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Example SVG data for a simple circle
static const char* simple_svg_data = 
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
    "<svg width=\"100\" height=\"100\" viewBox=\"0 0 100 100\" xmlns=\"http://www.w3.org/2000/svg\">"
    "  <circle cx=\"50\" cy=\"50\" r=\"40\" fill=\"blue\" stroke=\"red\" stroke-width=\"3\"/>"
    "</svg>";

void create_svg_example(lv_obj_t * parent) {
    // Create a container for the SVG
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_size(cont, 200, 200);
    lv_obj_center(cont);
    lv_obj_set_style_bg_color(cont, lv_color_white(), 0);
    lv_obj_set_style_border_width(cont, 2, 0);
    lv_obj_set_style_border_color(cont, lv_color_black(), 0);
    
    // Load SVG data
    lv_svg_node_t * svg_node = lv_svg_load_data(simple_svg_data, strlen(simple_svg_data));
    if (svg_node == NULL) {
        printf("Failed to load SVG data\n");
        
        // Create error label
        lv_obj_t * error_label = lv_label_create(cont);
        lv_label_set_text(error_label, "Failed to load SVG data");
        lv_obj_center(error_label);
        return;
    }
    
    // Create a simple colored rectangle to represent the SVG
    lv_obj_t * rect = lv_obj_create(cont);
    lv_obj_set_size(rect, 150, 150);
    lv_obj_center(rect);
    lv_obj_set_style_bg_color(rect, lv_color_hex(0x0000FF), 0); // Blue
    lv_obj_set_style_border_width(rect, 3, 0);
    lv_obj_set_style_border_color(rect, lv_color_hex(0xFF0000), 0); // Red border
    
    // Clean up
    lv_svg_node_delete(svg_node);
    
    // Add a label to indicate this is an SVG example
    lv_obj_t * label = lv_label_create(cont);
    lv_label_set_text(label, "SVG Example");
    lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, -10);
    
    printf("SVG example created successfully\n");
}

void create_svg_from_file_example(lv_obj_t * parent, const char * file_path) {
    // Create a container for the SVG
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_size(cont, 200, 200);
    lv_obj_center(cont);
    lv_obj_set_style_bg_color(cont, lv_color_white(), 0);
    lv_obj_set_style_border_width(cont, 2, 0);
    lv_obj_set_style_border_color(cont, lv_color_black(), 0);
    
    // Try to open and read the SVG file
    FILE * file = fopen(file_path, "rb");
    if (file == NULL) {
        printf("Failed to open SVG file: %s\n", file_path);
        
        // Create error label
        lv_obj_t * error_label = lv_label_create(cont);
        lv_label_set_text(error_label, "Failed to load SVG file");
        lv_obj_center(error_label);
        return;
    }
    
    // Get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // Allocate memory for file content
    char * svg_data = malloc(file_size + 1);
    if (svg_data == NULL) {
        printf("Failed to allocate memory for SVG file\n");
        fclose(file);
        return;
    }
    
    // Read file content
    size_t bytes_read = fread(svg_data, 1, file_size, file);
    svg_data[bytes_read] = '\0';
    fclose(file);
    
    // Load SVG data
    lv_svg_node_t * svg_node = lv_svg_load_data(svg_data, bytes_read);
    if (svg_node == NULL) {
        printf("Failed to parse SVG file: %s\n", file_path);
        free(svg_data);
        
        // Create error label
        lv_obj_t * error_label = lv_label_create(cont);
        lv_label_set_text(error_label, "Failed to parse SVG file");
        lv_obj_center(error_label);
        return;
    }
    
    // Create a simple colored rectangle to represent the SVG
    lv_obj_t * rect = lv_obj_create(cont);
    lv_obj_set_size(rect, 150, 150);
    lv_obj_center(rect);
    lv_obj_set_style_bg_color(rect, lv_color_hex(0x00FF00), 0); // Green
    lv_obj_set_style_border_width(rect, 2, 0);
    lv_obj_set_style_border_color(rect, lv_color_hex(0x0000FF), 0); // Blue border
    
    // Add a label to show the file path
    lv_obj_t * label = lv_label_create(cont);
    lv_label_set_text_fmt(label, "SVG: %s", file_path);
    lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, -10);
    
    // Clean up
    lv_svg_node_delete(svg_node);
    free(svg_data);
    
    printf("SVG file loaded successfully: %s\n", file_path);
}

// Helper function to create an SVG widget from a file path
lv_obj_t* create_svg_widget(lv_obj_t * parent, const char * file_path) {
    // Create a container for the SVG widget
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_size(cont, 100, 100);
    lv_obj_set_style_bg_color(cont, lv_color_white(), 0);
    lv_obj_set_style_border_width(cont, 1, 0);
    lv_obj_set_style_border_color(cont, lv_color_hex(0x808080), 0); // Gray border
    
    // Try to open and read the SVG file
    FILE * file = fopen(file_path, "rb");
    if (file == NULL) {
        printf("Failed to open SVG file: %s\n", file_path);
        
        // Create error label
        lv_obj_t * error_label = lv_label_create(cont);
        lv_label_set_text(error_label, "Error");
        lv_obj_center(error_label);
        return cont;
    }
    
    // Get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // Allocate memory for file content
    char * svg_data = malloc(file_size + 1);
    if (svg_data == NULL) {
        printf("Failed to allocate memory for SVG file\n");
        fclose(file);
        return cont;
    }
    
    // Read file content
    size_t bytes_read = fread(svg_data, 1, file_size, file);
    svg_data[bytes_read] = '\0';
    fclose(file);
    
    // Load SVG data
    lv_svg_node_t * svg_node = lv_svg_load_data(svg_data, bytes_read);
    if (svg_node == NULL) {
        printf("Failed to parse SVG file: %s\n", file_path);
        free(svg_data);
        
        // Create error label
        lv_obj_t * error_label = lv_label_create(cont);
        lv_label_set_text(error_label, "Parse Error");
        lv_obj_center(error_label);
        return cont;
    }
    
    // Create a simple colored rectangle to represent the SVG
    lv_obj_t * rect = lv_obj_create(cont);
    lv_obj_set_size(rect, 80, 80);
    lv_obj_center(rect);
    lv_obj_set_style_bg_color(rect, lv_color_hex(0xFF8000), 0); // Orange
    lv_obj_set_style_border_width(rect, 1, 0);
    lv_obj_set_style_border_color(rect, lv_color_hex(0x000000), 0); // Black border
    
    // Add a small label to show success
    lv_obj_t * label = lv_label_create(cont);
    lv_label_set_text(label, "SVG");
    lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, -5);
    
    // Clean up
    lv_svg_node_delete(svg_node);
    free(svg_data);
    
    printf("SVG widget created successfully: %s\n", file_path);
    return cont;
} 