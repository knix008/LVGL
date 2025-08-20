#include "tab_button.h"
#include "font_config.h"
#include "ui_components.h"
#include <stdio.h>



// Close button event callback function
static void close_button_event_cb(lv_event_t * e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        lv_obj_t * popup = lv_obj_get_parent(lv_event_get_target(e));
        lv_obj_del(popup);
    }
}

// Image button event callback function
static void image_button_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btn = lv_event_get_target(e);
    
    if (code == LV_EVENT_CLICKED) {
        // Get the button text from user data or create a descriptive text
        const char * btn_text = (const char*)lv_event_get_user_data(e);
        if (!btn_text) btn_text = "Image Button";
        printf("Image button clicked: %s\n", btn_text);
        
        // Create a popup message with better layout
        lv_obj_t * popup = lv_obj_create(lv_scr_act());
        lv_obj_set_size(popup, 250, 150);
        lv_obj_align(popup, LV_ALIGN_CENTER, 0, 0);
        lv_obj_set_style_bg_color(popup, lv_color_hex(0x4CAF50), 0);  // Green for image buttons
        lv_obj_set_style_border_width(popup, 2, 0);
        lv_obj_set_style_border_color(popup, lv_color_hex(0x388E3C), 0);
        lv_obj_set_style_radius(popup, 10, 0);
        lv_obj_set_style_pad_all(popup, 15, 0);
        
        // Add title label
        lv_obj_t * title_label = lv_label_create(popup);
        lv_label_set_text(title_label, "Image Button Clicked!");
        lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 0);
        lv_obj_set_style_text_color(title_label, lv_color_white(), 0);
        lv_obj_set_style_text_font(title_label, get_korean_font_small(), 0);
        
        // Main text label
        lv_obj_t * popup_label = lv_label_create(popup);
        lv_label_set_text(popup_label, btn_text);
        lv_obj_align(popup_label, LV_ALIGN_CENTER, 0, -10);
        lv_obj_set_style_text_color(popup_label, lv_color_white(), 0);
        lv_obj_set_style_text_font(popup_label, get_korean_font(), 0);
        lv_obj_set_style_text_align(popup_label, LV_TEXT_ALIGN_CENTER, 0);
        
        // Add close button
        lv_obj_t * close_btn = lv_button_create(popup);
        lv_obj_set_size(close_btn, 80, 35);
        lv_obj_align(close_btn, LV_ALIGN_BOTTOM_MID, 0, -5);
        lv_obj_set_style_bg_color(close_btn, lv_color_hex(0xF44336), 0);
        lv_obj_set_style_radius(close_btn, 5, 0);
        
        lv_obj_t * close_label = lv_label_create(close_btn);
        lv_label_set_text(close_label, "Close");
        lv_obj_center(close_label);
        lv_obj_set_style_text_color(close_label, lv_color_white(), 0);
        
        lv_obj_add_event_cb(close_btn, close_button_event_cb, LV_EVENT_CLICKED, NULL);
    }
}

// Button event callback function
static void button_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btn = lv_event_get_target(e);
    
    if (code == LV_EVENT_CLICKED) {
        const char * btn_text = lv_label_get_text(lv_obj_get_child(btn, 0));
        printf("Button clicked: %s\n", btn_text);
        
        // Create a popup message with better layout
        lv_obj_t * popup = lv_obj_create(lv_scr_act());
        lv_obj_set_size(popup, 250, 150);  // Made popup larger
        lv_obj_align(popup, LV_ALIGN_CENTER, 0, 0);
        lv_obj_set_style_bg_color(popup, lv_color_hex(0x2196F3), 0);
        lv_obj_set_style_border_width(popup, 2, 0);
        lv_obj_set_style_border_color(popup, lv_color_hex(0x1976D2), 0);
        lv_obj_set_style_radius(popup, 10, 0);
        lv_obj_set_style_pad_all(popup, 15, 0);  // Add padding
        
        // Add title label
        lv_obj_t * title_label = lv_label_create(popup);
        lv_label_set_text(title_label, "Button Clicked!");
        lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 0);
        lv_obj_set_style_text_color(title_label, lv_color_white(), 0);
        lv_obj_set_style_text_font(title_label, get_korean_font_small(), 0);
        
        // Main text label - positioned in the middle area
        lv_obj_t * popup_label = lv_label_create(popup);
        lv_label_set_text(popup_label, btn_text);
        lv_obj_align(popup_label, LV_ALIGN_CENTER, 0, -10);  // Slightly above center
        lv_obj_set_style_text_color(popup_label, lv_color_white(), 0);
        lv_obj_set_style_text_font(popup_label, get_korean_font(), 0);
        lv_obj_set_style_text_align(popup_label, LV_TEXT_ALIGN_CENTER, 0);
        
        // Add close button at the bottom
        lv_obj_t * close_btn = lv_button_create(popup);
        lv_obj_set_size(close_btn, 80, 35);  // Made button slightly larger
        lv_obj_align(close_btn, LV_ALIGN_BOTTOM_MID, 0, -5);  // Position at bottom with small margin
        lv_obj_set_style_bg_color(close_btn, lv_color_hex(0xF44336), 0);
        lv_obj_set_style_radius(close_btn, 5, 0);
        
        lv_obj_t * close_label = lv_label_create(close_btn);
        lv_label_set_text(close_label, "Close");
        lv_obj_center(close_label);
        lv_obj_set_style_text_color(close_label, lv_color_white(), 0);
        
        // Add event callback to close button
        lv_obj_add_event_cb(close_btn, close_button_event_cb, LV_EVENT_CLICKED, NULL);
    }
}

// Create Button tab
void create_button_tab(lv_obj_t * parent) {
    // Title
    lv_obj_t * title_label = lv_label_create(parent);
    lv_label_set_text(title_label, "Button Examples");
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_text_font(title_label, get_korean_font(), 0);
    
    // Create a container for buttons
    lv_obj_t * button_container = lv_obj_create(parent);
    lv_obj_set_size(button_container, 700, 480);
    lv_obj_align(button_container, LV_ALIGN_TOP_MID, 0, 50);
    lv_obj_set_style_pad_all(button_container, 20, 0);
    lv_obj_set_style_bg_color(button_container, lv_color_hex(0xF5F5F5), 0);
    lv_obj_set_style_border_width(button_container, 0, 0);
    
    // Section 1: Basic Buttons
    lv_obj_t * section1_label = lv_label_create(button_container);
    lv_label_set_text(section1_label, "Basic Buttons");
    lv_obj_align(section1_label, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_text_font(section1_label, get_korean_font_small(), 0);
    lv_obj_set_style_text_color(section1_label, lv_color_hex(0x333333), 0);
    
    // Basic button 1
    lv_obj_t * btn1 = lv_button_create(button_container);
    lv_obj_set_size(btn1, 120, 40);
    lv_obj_align(btn1, LV_ALIGN_TOP_LEFT, 0, 30);
    lv_obj_add_event_cb(btn1, button_event_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t * btn1_label = lv_label_create(btn1);
    lv_label_set_text(btn1_label, "Primary");
    lv_obj_center(btn1_label);
    lv_obj_set_style_bg_color(btn1, lv_color_hex(0x2196F3), 0);
    lv_obj_set_style_text_color(btn1_label, lv_color_white(), 0);
    
    // Basic button 2
    lv_obj_t * btn2 = lv_button_create(button_container);
    lv_obj_set_size(btn2, 120, 40);
    lv_obj_align(btn2, LV_ALIGN_TOP_LEFT, 140, 30);
    lv_obj_add_event_cb(btn2, button_event_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t * btn2_label = lv_label_create(btn2);
    lv_label_set_text(btn2_label, "Secondary");
    lv_obj_center(btn2_label);
    lv_obj_set_style_bg_color(btn2, lv_color_hex(0x757575), 0);
    lv_obj_set_style_text_color(btn2_label, lv_color_white(), 0);
    
    // Basic button 3
    lv_obj_t * btn3 = lv_button_create(button_container);
    lv_obj_set_size(btn3, 120, 40);
    lv_obj_align(btn3, LV_ALIGN_TOP_LEFT, 280, 30);
    lv_obj_add_event_cb(btn3, button_event_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t * btn3_label = lv_label_create(btn3);
    lv_label_set_text(btn3_label, "Success");
    lv_obj_center(btn3_label);
    lv_obj_set_style_bg_color(btn3, lv_color_hex(0x4CAF50), 0);
    lv_obj_set_style_text_color(btn3_label, lv_color_white(), 0);
    
    // Section 2: Styled Buttons
    lv_obj_t * section2_label = lv_label_create(button_container);
    lv_label_set_text(section2_label, "Styled Buttons");
    lv_obj_align(section2_label, LV_ALIGN_TOP_LEFT, 0, 100);
    lv_obj_set_style_text_font(section2_label, get_korean_font_small(), 0);
    lv_obj_set_style_text_color(section2_label, lv_color_hex(0x333333), 0);
    
    // Rounded button
    lv_obj_t * btn4 = lv_button_create(button_container);
    lv_obj_set_size(btn4, 120, 40);
    lv_obj_align(btn4, LV_ALIGN_TOP_LEFT, 0, 130);
    lv_obj_add_event_cb(btn4, button_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_radius(btn4, 20, 0);
    lv_obj_set_style_bg_color(btn4, lv_color_hex(0xFF9800), 0);
    
    lv_obj_t * btn4_label = lv_label_create(btn4);
    lv_label_set_text(btn4_label, "Rounded");
    lv_obj_center(btn4_label);
    lv_obj_set_style_text_color(btn4_label, lv_color_white(), 0);
    
    // Outlined button
    lv_obj_t * btn5 = lv_button_create(button_container);
    lv_obj_set_size(btn5, 120, 40);
    lv_obj_align(btn5, LV_ALIGN_TOP_LEFT, 140, 130);
    lv_obj_add_event_cb(btn5, button_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_bg_color(btn5, lv_color_white(), 0);
    lv_obj_set_style_border_width(btn5, 2, 0);
    lv_obj_set_style_border_color(btn5, lv_color_hex(0x2196F3), 0);
    
    lv_obj_t * btn5_label = lv_label_create(btn5);
    lv_label_set_text(btn5_label, "Outlined");
    lv_obj_center(btn5_label);
    lv_obj_set_style_text_color(btn5_label, lv_color_hex(0x2196F3), 0);
    
    // Icon button
    lv_obj_t * btn6 = lv_button_create(button_container);
    lv_obj_set_size(btn6, 120, 40);
    lv_obj_align(btn6, LV_ALIGN_TOP_LEFT, 280, 130);
    lv_obj_add_event_cb(btn6, button_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_bg_color(btn6, lv_color_hex(0xE91E63), 0);
    
    lv_obj_t * btn6_label = lv_label_create(btn6);
    lv_label_set_text(btn6_label, "Icon");
    lv_obj_center(btn6_label);
    lv_obj_set_style_text_color(btn6_label, lv_color_white(), 0);
    
    // Section 3: Korean Buttons
    lv_obj_t * section3_label = lv_label_create(button_container);
    lv_label_set_text(section3_label, "Korean Buttons");
    lv_obj_align(section3_label, LV_ALIGN_TOP_LEFT, 0, 200);
    lv_obj_set_style_text_font(section3_label, get_korean_font_small(), 0);
    lv_obj_set_style_text_color(section3_label, lv_color_hex(0x333333), 0);
    
    // Korean button 1
    lv_obj_t * btn7 = lv_button_create(button_container);
    lv_obj_set_size(btn7, 120, 40);
    lv_obj_align(btn7, LV_ALIGN_TOP_LEFT, 0, 230);
    lv_obj_add_event_cb(btn7, button_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_bg_color(btn7, lv_color_hex(0x9C27B0), 0);
    
    lv_obj_t * btn7_label = lv_label_create(btn7);
    lv_label_set_text(btn7_label, "확인");
    lv_obj_center(btn7_label);
    lv_obj_set_style_text_color(btn7_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(btn7_label, get_korean_font(), 0);
    
    // Korean button 2
    lv_obj_t * btn8 = lv_button_create(button_container);
    lv_obj_set_size(btn8, 120, 40);
    lv_obj_align(btn8, LV_ALIGN_TOP_LEFT, 140, 230);
    lv_obj_add_event_cb(btn8, button_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_bg_color(btn8, lv_color_hex(0xF44336), 0);
    
    lv_obj_t * btn8_label = lv_label_create(btn8);
    lv_label_set_text(btn8_label, "취소");
    lv_obj_center(btn8_label);
    lv_obj_set_style_text_color(btn8_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(btn8_label, get_korean_font(), 0);
    
    // Korean button 3
    lv_obj_t * btn9 = lv_button_create(button_container);
    lv_obj_set_size(btn9, 120, 40);
    lv_obj_align(btn9, LV_ALIGN_TOP_LEFT, 280, 230);
    lv_obj_add_event_cb(btn9, button_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_bg_color(btn9, lv_color_hex(0x607D8B), 0);
    
    lv_obj_t * btn9_label = lv_label_create(btn9);
    lv_label_set_text(btn9_label, "설정");
    lv_obj_center(btn9_label);
    lv_obj_set_style_text_color(btn9_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(btn9_label, get_korean_font(), 0);
    
    // Section 4: Large Buttons
    lv_obj_t * section4_label = lv_label_create(button_container);
    lv_label_set_text(section4_label, "Large Buttons");
    lv_obj_align(section4_label, LV_ALIGN_TOP_LEFT, 0, 300);
    lv_obj_set_style_text_font(section4_label, get_korean_font_small(), 0);
    lv_obj_set_style_text_color(section4_label, lv_color_hex(0x333333), 0);
    
    // Large button 1
    lv_obj_t * btn10 = lv_button_create(button_container);
    lv_obj_set_size(btn10, 200, 50);
    lv_obj_align(btn10, LV_ALIGN_TOP_LEFT, 0, 330);
    lv_obj_add_event_cb(btn10, button_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_bg_color(btn10, lv_color_hex(0x3F51B5), 0);
    lv_obj_set_style_radius(btn10, 25, 0);
    
    lv_obj_t * btn10_label = lv_label_create(btn10);
    lv_label_set_text(btn10_label, "Large Button");
    lv_obj_center(btn10_label);
    lv_obj_set_style_text_color(btn10_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(btn10_label, get_korean_font(), 0);
    
    // Large button 2
    lv_obj_t * btn11 = lv_button_create(button_container);
    lv_obj_set_size(btn11, 200, 50);
    lv_obj_align(btn11, LV_ALIGN_TOP_LEFT, 220, 330);
    lv_obj_add_event_cb(btn11, button_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_bg_color(btn11, lv_color_hex(0x009688), 0);
    lv_obj_set_style_radius(btn11, 25, 0);
    
    lv_obj_t * btn11_label = lv_label_create(btn11);
    lv_label_set_text(btn11_label, "Action Button");
    lv_obj_center(btn11_label);
    lv_obj_set_style_text_color(btn11_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(btn11_label, get_korean_font(), 0);
    
    // Section 5: Custom Image Buttons
    lv_obj_t * section5_label = lv_label_create(button_container);
    lv_label_set_text(section5_label, "Custom Image Buttons");
    lv_obj_align(section5_label, LV_ALIGN_TOP_LEFT, 450, 100);  // Position on the right side
    lv_obj_set_style_text_font(section5_label, get_korean_font_small(), 0);
    lv_obj_set_style_text_color(section5_label, lv_color_hex(0x333333), 0);
    
    // Image button 1 - Info button (using regular button with properly sized image)
    lv_obj_t * img_btn1 = lv_button_create(button_container);
    lv_obj_set_size(img_btn1, 80, 40);
    lv_obj_align(img_btn1, LV_ALIGN_TOP_LEFT, 450, 130);
    lv_obj_add_event_cb(img_btn1, image_button_event_cb, LV_EVENT_CLICKED, "Info Button");
    
    lv_obj_t * img1 = lv_image_create(img_btn1);
    lv_image_set_src(img1, "A:../assets/info_button.png");
    lv_obj_set_size(img1, 60, 25);  // Smaller size to fit inside button
    lv_obj_align(img1, LV_ALIGN_CENTER, 0, 0);
    
    // Image button 2 - Idea button (using regular button with properly sized image)
    lv_obj_t * img_btn2 = lv_button_create(button_container);
    lv_obj_set_size(img_btn2, 80, 40);
    lv_obj_align(img_btn2, LV_ALIGN_TOP_LEFT, 540, 130);
    lv_obj_add_event_cb(img_btn2, image_button_event_cb, LV_EVENT_CLICKED, "Idea Button");
    
    lv_obj_t * img2 = lv_image_create(img_btn2);
    lv_image_set_src(img2, "A:../assets/idea_button.png");
    lv_obj_set_size(img2, 60, 25);  // Smaller size to fit inside button
    lv_obj_align(img2, LV_ALIGN_CENTER, 0, 0);
    
    // Image button 3 - Config button (using regular button with properly sized image)
    lv_obj_t * img_btn3 = lv_button_create(button_container);
    lv_obj_set_size(img_btn3, 80, 40);
    lv_obj_align(img_btn3, LV_ALIGN_TOP_LEFT, 630, 130);
    lv_obj_add_event_cb(img_btn3, image_button_event_cb, LV_EVENT_CLICKED, "Config Button");
    
    lv_obj_t * img3 = lv_image_create(img_btn3);
    lv_image_set_src(img3, "A:../assets/config_button.png");
    lv_obj_set_size(img3, 60, 25);  // Smaller size to fit inside button
    lv_obj_align(img3, LV_ALIGN_CENTER, 0, 0);
    

    
    // Section 6: Buttons with Images
    lv_obj_t * section6_label = lv_label_create(button_container);
    lv_label_set_text(section6_label, "Buttons with Images");
    lv_obj_align(section6_label, LV_ALIGN_TOP_LEFT, 450, 220);
    lv_obj_set_style_text_font(section6_label, get_korean_font_small(), 0);
    lv_obj_set_style_text_color(section6_label, lv_color_hex(0x333333), 0);
    
    // Regular button with image and text 1
    lv_obj_t * btn_img1 = lv_button_create(button_container);
    lv_obj_set_size(btn_img1, 140, 50);
    lv_obj_align(btn_img1, LV_ALIGN_TOP_LEFT, 450, 250);
    lv_obj_add_event_cb(btn_img1, button_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_bg_color(btn_img1, lv_color_hex(0x673AB7), 0);
    lv_obj_set_style_radius(btn_img1, 10, 0);
    lv_obj_set_style_pad_all(btn_img1, 8, 0);
    
    // Add image to button
    lv_obj_t * btn_img1_image = lv_image_create(btn_img1);
    lv_image_set_src(btn_img1_image, "A:../assets/bulb.gif");
    lv_obj_set_size(btn_img1_image, 24, 24);
    lv_obj_align(btn_img1_image, LV_ALIGN_LEFT_MID, 10, 0);
    
    // Add text label to button
    lv_obj_t * btn_img1_label = lv_label_create(btn_img1);
    lv_label_set_text(btn_img1_label, "Ideas");
    lv_obj_align(btn_img1_label, LV_ALIGN_RIGHT_MID, -10, 0);
    lv_obj_set_style_text_color(btn_img1_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(btn_img1_label, get_korean_font(), 0);
    
    // Regular button with image and text 2
    lv_obj_t * btn_img2 = lv_button_create(button_container);
    lv_obj_set_size(btn_img2, 140, 50);
    lv_obj_align(btn_img2, LV_ALIGN_TOP_LEFT, 450, 310);
    lv_obj_add_event_cb(btn_img2, button_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_bg_color(btn_img2, lv_color_hex(0xFF5722), 0);
    lv_obj_set_style_radius(btn_img2, 10, 0);
    lv_obj_set_style_pad_all(btn_img2, 8, 0);
    
    // Add image to button
    lv_obj_t * btn_img2_image = lv_image_create(btn_img2);
    lv_image_set_src(btn_img2_image, "A:../assets/setting.jpg");
    lv_obj_set_size(btn_img2_image, 24, 24);
    lv_obj_align(btn_img2_image, LV_ALIGN_LEFT_MID, 10, 0);
    
    // Add text label to button
    lv_obj_t * btn_img2_label = lv_label_create(btn_img2);
    lv_label_set_text(btn_img2_label, "Config");
    lv_obj_align(btn_img2_label, LV_ALIGN_RIGHT_MID, -10, 0);
    lv_obj_set_style_text_color(btn_img2_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(btn_img2_label, get_korean_font(), 0);
    
    printf("Button tab created successfully\n");
}
