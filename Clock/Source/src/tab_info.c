#include "tab_info.h"
#include "image_config.h"
#include <stdio.h>

// Create Info tab
void create_info_tab(lv_obj_t * parent) {
    // Tab 3: Info
    lv_obj_t * label3 = lv_label_create(parent);
    lv_label_set_text(label3, "Application Info");
    lv_obj_align(label3, LV_ALIGN_TOP_MID, 0, 10);
    
    // Create image widget for Intellivix logo
    lv_obj_t * logo_img = lv_image_create(parent);

    // Set the image source to the PNG file
    const char* image_path = IMAGE_PATH(INTELLIVIX_LOGO);
    printf("Loading image from: %s\n", image_path);
    lv_image_set_src(logo_img, image_path);
    printf("Image source set\n");
    
    // Check if the image loaded successfully
    const void* src = lv_image_get_src(logo_img);
    if (src != NULL) {
        printf("Image source is not NULL - image should be loaded\n");
    } else {
        printf("Image source is NULL - image failed to load\n");
    }
    
    // Position the image in the center, below the title
    lv_obj_align(logo_img, LV_ALIGN_TOP_MID, 0, 40);
    
    // Set image size to 100x40 pixels as requested
    lv_obj_set_size(logo_img, 250, 45);
    
    lv_obj_t * info_label = lv_label_create(parent);
    lv_label_set_text(info_label, 
        "Face Recognition Demo\n\n"
        "Version: 1.0.0\n"
        "Features:\n"
        "- Number keypad interface\n"
        "- Database basic operations\n"
        "- Settings panel\n"
        "- Korean true type font\n"
        "- Simple Korean input\n"
        "- ChunJiIn Korean input\n"
        "- Image(PNG) loading\n"
        "- Popup diaglog box\n"
    );
    lv_obj_align(info_label, LV_ALIGN_TOP_MID, 50, 100);
    lv_obj_set_width(info_label, 300);
    lv_label_set_long_mode(info_label, LV_LABEL_LONG_WRAP);
} 