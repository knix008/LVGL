#include "ui_korean_keypad.h"
#include <string.h>
#include <stdio.h>

// Forward declaration
static void create_hangul_buttons(lv_obj_t * parent);

// Korean keypad display label (extern for callbacks)
lv_obj_t * korean_keypad_display_label = NULL;

// Create Korean keypad UI components
void create_korean_keypad_ui(lv_obj_t * parent) {
    // Safety check for parent object
    if (parent == NULL) {
        printf("Error: Korean keypad parent is NULL\n");
        return;
    }
    
    printf("Creating Korean keypad UI with parent: %p\n", (void*)parent);
    
    // Set dark background for the entire keypad
    lv_obj_set_style_bg_color(parent, lv_color_hex(0x1E1E1E), 0);
    lv_obj_set_style_bg_opa(parent, LV_OPA_COVER, 0);
    
    // Keypad title
    lv_obj_t * keypad_label = lv_label_create(parent);
    if (keypad_label == NULL) {
        printf("Error: Failed to create keypad label\n");
        return;
    }
    lv_label_set_text(keypad_label, "한글 키패드");
    lv_obj_set_style_text_color(keypad_label, lv_color_hex(0xFFFFFF), 0);
    if (korean_font != NULL) {
        lv_obj_set_style_text_font(keypad_label, korean_font, 0);
    }
    lv_obj_align(keypad_label, LV_ALIGN_TOP_MID, 0, 10);

    // Display area for entered text
    lv_obj_t * keypad_display = lv_label_create(parent);
    if (keypad_display == NULL) {
        printf("Error: Failed to create keypad display\n");
        return;
    }
    lv_label_set_text(keypad_display, "");
    lv_obj_set_style_bg_color(keypad_display, lv_color_hex(0x2C2C2C), 0);
    lv_obj_set_style_bg_opa(keypad_display, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(keypad_display, lv_color_hex(0x555555), 0);
    lv_obj_set_style_border_width(keypad_display, 1, 0);
    lv_obj_set_style_pad_all(keypad_display, 10, 0);
    lv_obj_set_style_text_color(keypad_display, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_size(keypad_display, 550, 55);
    lv_obj_align(keypad_display, LV_ALIGN_TOP_MID, 0, 40);
    if (korean_font != NULL) {
        lv_obj_set_style_text_font(keypad_display, korean_font, 0);
    }
    korean_keypad_display_label = keypad_display;
    korean_keypad_state.display_label = keypad_display;

    // Mode switch button
    lv_obj_t * mode_btn = lv_btn_create(parent);
    if (mode_btn == NULL) {
        printf("Error: Failed to create mode button\n");
        return;
    }
    lv_obj_set_size(mode_btn, 60, 25);
    lv_obj_align(mode_btn, LV_ALIGN_TOP_LEFT, 15, 100);
    lv_obj_set_style_bg_color(mode_btn, lv_color_hex(0x388E3C), 0);
    lv_obj_set_style_text_color(mode_btn, lv_color_hex(0xFFFFFF), 0);
    
    lv_obj_t * mode_label = lv_label_create(mode_btn);
    if (mode_label == NULL) {
        printf("Error: Failed to create mode label\n");
        return;
    }
    lv_label_set_text(mode_label, "한글");
    if (korean_font != NULL) {
        lv_obj_set_style_text_font(mode_label, korean_font, 0);
    }
    lv_obj_center(mode_label);
    lv_obj_add_event_cb(mode_btn, korean_keypad_mode_cb, LV_EVENT_CLICKED, NULL);
    korean_keypad_state.mode_btn = mode_btn;

    printf("Creating Hangul buttons...\n");
    // Create character buttons based on current mode
    create_hangul_buttons(parent);

    // Function buttons row
    int btn_width = 65;
    int btn_height = 38;
    int start_x = 15;
    int start_y = 340;

    // Clear button
    lv_obj_t * clear_btn = lv_btn_create(parent);
    if (clear_btn == NULL) {
        printf("Error: Failed to create clear button\n");
        return;
    }
    lv_obj_set_size(clear_btn, btn_width, btn_height);
    lv_obj_align(clear_btn, LV_ALIGN_TOP_LEFT, start_x, start_y);
    lv_obj_set_style_bg_color(clear_btn, lv_color_hex(0xD32F2F), 0);
    lv_obj_set_style_text_color(clear_btn, lv_color_hex(0xFFFFFF), 0);
    
    lv_obj_t * clear_label = lv_label_create(clear_btn);
    if (clear_label == NULL) {
        printf("Error: Failed to create clear label\n");
        return;
    }
    lv_label_set_text(clear_label, "Clear");
    lv_obj_center(clear_label);
    lv_obj_add_event_cb(clear_btn, korean_keypad_clear_cb, LV_EVENT_CLICKED, NULL);

    // Backspace button
    lv_obj_t * backspace_btn = lv_btn_create(parent);
    if (backspace_btn == NULL) {
        printf("Error: Failed to create backspace button\n");
        return;
    }
    lv_obj_set_size(backspace_btn, btn_width, btn_height);
    lv_obj_align(backspace_btn, LV_ALIGN_TOP_LEFT, start_x + btn_width + 5, start_y);
    lv_obj_set_style_bg_color(backspace_btn, lv_color_hex(0xF57C00), 0);
    lv_obj_set_style_text_color(backspace_btn, lv_color_hex(0xFFFFFF), 0);
    
    lv_obj_t * backspace_label = lv_label_create(backspace_btn);
    if (backspace_label == NULL) {
        printf("Error: Failed to create backspace label\n");
        return;
    }
    lv_label_set_text(backspace_label, "⌫");
    lv_obj_center(backspace_label);
    lv_obj_add_event_cb(backspace_btn, korean_keypad_backspace_cb, LV_EVENT_CLICKED, NULL);

    // Space button
    lv_obj_t * space_btn = lv_btn_create(parent);
    if (space_btn == NULL) {
        printf("Error: Failed to create space button\n");
        return;
    }
    lv_obj_set_size(space_btn, btn_width * 2 + 5, btn_height);
    lv_obj_align(space_btn, LV_ALIGN_TOP_LEFT, start_x + 2 * (btn_width + 5), start_y);
    lv_obj_set_style_bg_color(space_btn, lv_color_hex(0x1976D2), 0);
    lv_obj_set_style_text_color(space_btn, lv_color_hex(0xFFFFFF), 0);
    
    lv_obj_t * space_label = lv_label_create(space_btn);
    if (space_label == NULL) {
        printf("Error: Failed to create space label\n");
        return;
    }
    lv_label_set_text(space_label, "Space");
    lv_obj_center(space_label);
    lv_obj_add_event_cb(space_btn, korean_keypad_space_cb, LV_EVENT_CLICKED, NULL);

    // Enter button
    lv_obj_t * enter_btn = lv_btn_create(parent);
    if (enter_btn == NULL) {
        printf("Error: Failed to create enter button\n");
        return;
    }
    lv_obj_set_size(enter_btn, btn_width, btn_height);
    lv_obj_align(enter_btn, LV_ALIGN_TOP_LEFT, start_x + 4 * (btn_width + 5), start_y);
    lv_obj_set_style_bg_color(enter_btn, lv_color_hex(0x388E3C), 0);
    lv_obj_set_style_text_color(enter_btn, lv_color_hex(0xFFFFFF), 0);
    
    lv_obj_t * enter_label = lv_label_create(enter_btn);
    if (enter_label == NULL) {
        printf("Error: Failed to create enter label\n");
        return;
    }
    lv_label_set_text(enter_label, "Enter");
    lv_obj_center(enter_label);
    lv_obj_add_event_cb(enter_btn, korean_keypad_enter_cb, LV_EVENT_CLICKED, NULL);
    
    printf("Korean keypad UI created successfully\n");
}

// Create Hangul character buttons
static void create_hangul_buttons(lv_obj_t * parent) {
    // Safety check for parent object
    if (parent == NULL) {
        printf("Error: Hangul buttons parent is NULL\n");
        return;
    }
    
    int btn_width = 38;
    int btn_height = 35;
    int start_x = 15;
    int start_y = 130;

    // First row: ㄱㄲㄴㄷㄸㄹㅁㅂㅃ
    for (int i = 0; i < 9; i++) {
        lv_obj_t * btn = lv_btn_create(parent);
        if (btn == NULL) {
            printf("Error: Failed to create button %d in first row\n", i);
            continue;
        }
        lv_obj_set_size(btn, btn_width, btn_height);
        lv_obj_align(btn, LV_ALIGN_TOP_LEFT, start_x + i * (btn_width + 3), start_y);
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x424242), 0);
        lv_obj_set_style_text_color(btn, lv_color_hex(0xFFFFFF), 0);
        
        lv_obj_t * btn_label = lv_label_create(btn);
        if (btn_label == NULL) {
            printf("Error: Failed to create label for button %d in first row\n", i);
            continue;
        }
        lv_label_set_text(btn_label, hangul_choseong[i]);
        if (korean_font != NULL) {
            lv_obj_set_style_text_font(btn_label, korean_font, 0);
        }
        lv_obj_center(btn_label);
        lv_obj_add_event_cb(btn, korean_keypad_char_cb, LV_EVENT_CLICKED, (void*)(uintptr_t)i);
    }

    // Second row: ㅅㅆㅇㅈㅉㅊㅋㅌㅍ
    for (int i = 9; i < 18; i++) {
        lv_obj_t * btn = lv_btn_create(parent);
        if (btn == NULL) {
            printf("Error: Failed to create button %d in second row\n", i);
            continue;
        }
        lv_obj_set_size(btn, btn_width, btn_height);
        lv_obj_align(btn, LV_ALIGN_TOP_LEFT, start_x + (i - 9) * (btn_width + 3), start_y + btn_height + 5);
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x424242), 0);
        lv_obj_set_style_text_color(btn, lv_color_hex(0xFFFFFF), 0);
        
        lv_obj_t * btn_label = lv_label_create(btn);
        if (btn_label == NULL) {
            printf("Error: Failed to create label for button %d in second row\n", i);
            continue;
        }
        lv_label_set_text(btn_label, hangul_choseong[i]);
        if (korean_font != NULL) {
            lv_obj_set_style_text_font(btn_label, korean_font, 0);
        }
        lv_obj_center(btn_label);
        lv_obj_add_event_cb(btn, korean_keypad_char_cb, LV_EVENT_CLICKED, (void*)(uintptr_t)i);
    }

    // Third row: ㅎㅏㅐㅑㅒㅓㅔㅕㅖ
    lv_obj_t * btn = lv_btn_create(parent);
    if (btn == NULL) {
        printf("Error: Failed to create first button in third row\n");
    } else {
        lv_obj_set_size(btn, btn_width, btn_height);
        lv_obj_align(btn, LV_ALIGN_TOP_LEFT, start_x, start_y + 2 * (btn_height + 5));
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x424242), 0);
        lv_obj_set_style_text_color(btn, lv_color_hex(0xFFFFFF), 0);
        
        lv_obj_t * btn_label = lv_label_create(btn);
        if (btn_label == NULL) {
            printf("Error: Failed to create label for first button in third row\n");
        } else {
            lv_label_set_text(btn_label, hangul_choseong[18]);
            if (korean_font != NULL) {
                lv_obj_set_style_text_font(btn_label, korean_font, 0);
            }
            lv_obj_center(btn_label);
            lv_obj_add_event_cb(btn, korean_keypad_char_cb, LV_EVENT_CLICKED, (void*)(uintptr_t)18);
        }
    }

    for (int i = 0; i < 8; i++) {
        lv_obj_t * btn = lv_btn_create(parent);
        if (btn == NULL) {
            printf("Error: Failed to create button %d in third row vowels\n", i);
            continue;
        }
        lv_obj_set_size(btn, btn_width, btn_height);
        lv_obj_align(btn, LV_ALIGN_TOP_LEFT, start_x + (i + 1) * (btn_width + 3), start_y + 2 * (btn_height + 5));
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x616161), 0);
        lv_obj_set_style_text_color(btn, lv_color_hex(0xFFFFFF), 0);
        
        lv_obj_t * btn_label = lv_label_create(btn);
        if (btn_label == NULL) {
            printf("Error: Failed to create label for button %d in third row vowels\n", i);
            continue;
        }
        lv_label_set_text(btn_label, hangul_jungseong[i]);
        if (korean_font != NULL) {
            lv_obj_set_style_text_font(btn_label, korean_font, 0);
        }
        lv_obj_center(btn_label);
        lv_obj_add_event_cb(btn, korean_keypad_char_cb, LV_EVENT_CLICKED, (void*)(uintptr_t)(i + 100));
    }

    // Fourth row: ㅗㅘㅙㅚㅛㅜㅝㅞㅟ
    for (int i = 8; i < 17; i++) {
        lv_obj_t * btn = lv_btn_create(parent);
        if (btn == NULL) {
            printf("Error: Failed to create button %d in fourth row\n", i);
            continue;
        }
        lv_obj_set_size(btn, btn_width, btn_height);
        lv_obj_align(btn, LV_ALIGN_TOP_LEFT, start_x + (i - 8) * (btn_width + 3), start_y + 3 * (btn_height + 5));
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x616161), 0);
        lv_obj_set_style_text_color(btn, lv_color_hex(0xFFFFFF), 0);
        
        lv_obj_t * btn_label = lv_label_create(btn);
        if (btn_label == NULL) {
            printf("Error: Failed to create label for button %d in fourth row\n", i);
            continue;
        }
        lv_label_set_text(btn_label, hangul_jungseong[i]);
        if (korean_font != NULL) {
            lv_obj_set_style_text_font(btn_label, korean_font, 0);
        }
        lv_obj_center(btn_label);
        lv_obj_add_event_cb(btn, korean_keypad_char_cb, LV_EVENT_CLICKED, (void*)(uintptr_t)(i + 100));
    }

    // Fifth row: ㅠㅡㅢㅣ (remaining vowels)
    for (int i = 17; i < 21; i++) {
        lv_obj_t * btn = lv_btn_create(parent);
        if (btn == NULL) {
            printf("Error: Failed to create button %d in fifth row\n", i);
            continue;
        }
        lv_obj_set_size(btn, btn_width, btn_height);
        lv_obj_align(btn, LV_ALIGN_TOP_LEFT, start_x + (i - 17) * (btn_width + 3), start_y + 4 * (btn_height + 5));
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x616161), 0);
        lv_obj_set_style_text_color(btn, lv_color_hex(0xFFFFFF), 0);
        
        lv_obj_t * btn_label = lv_label_create(btn);
        if (btn_label == NULL) {
            printf("Error: Failed to create label for button %d in fifth row\n", i);
            continue;
        }
        lv_label_set_text(btn_label, hangul_jungseong[i]);
        if (korean_font != NULL) {
            lv_obj_set_style_text_font(btn_label, korean_font, 0);
        }
        lv_obj_center(btn_label);
        lv_obj_add_event_cb(btn, korean_keypad_char_cb, LV_EVENT_CLICKED, (void*)(uintptr_t)(i + 100));
    }

    // Sixth row: Jongseong (final consonants) - only the most common 10
    int jongseong_indices[] = {1, 4, 7, 8, 16, 17, 19, 21, 22, 27}; // ㄱ, ㄴ, ㄷ, ㄹ, ㅁ, ㅂ, ㅅ, ㅇ, ㅈ, ㅎ
    int jongseong_count = sizeof(jongseong_indices) / sizeof(jongseong_indices[0]);
    for (int i = 0; i < jongseong_count; i++) {
        int jongseong_idx = jongseong_indices[i];
        lv_obj_t * btn = lv_btn_create(parent);
        if (btn == NULL) {
            printf("Error: Failed to create button %d in jongseong row\n", i);
            continue;
        }
        lv_obj_set_size(btn, btn_width, btn_height);
        lv_obj_align(btn, LV_ALIGN_TOP_LEFT, start_x + i * (btn_width + 3), start_y + 5 * (btn_height + 5));
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x795548), 0); // Brown color for jongseong
        lv_obj_set_style_text_color(btn, lv_color_hex(0xFFFFFF), 0);
        
        lv_obj_t * btn_label = lv_label_create(btn);
        if (btn_label == NULL) {
            printf("Error: Failed to create label for button %d in jongseong row\n", i);
            continue;
        }
        lv_label_set_text(btn_label, hangul_jongseong[jongseong_idx]);
        if (korean_font != NULL) {
            lv_obj_set_style_text_font(btn_label, korean_font, 0);
        }
        lv_obj_center(btn_label);
        lv_obj_add_event_cb(btn, korean_keypad_char_cb, LV_EVENT_CLICKED, (void*)(uintptr_t)(jongseong_idx + 200));
    }
    
    printf("Hangul buttons created successfully\n");
}

// Korean keypad character callback
void korean_keypad_char_cb(lv_event_t * e) {
    lv_obj_t * btn = lv_event_get_target(e);
    void * user_data = lv_event_get_user_data(e);
    int char_index = (int)(uintptr_t)user_data;
    
    process_hangul_character(char_index);
}

// Korean keypad mode callback
void korean_keypad_mode_cb(lv_event_t * e) {
    korean_keypad_state.mode = (korean_keypad_state.mode + 1) % 3;
    
    lv_obj_t * mode_btn = korean_keypad_state.mode_btn;
    lv_obj_t * mode_label = lv_obj_get_child(mode_btn, 0);
    
    switch (korean_keypad_state.mode) {
        case KOREAN_MODE_HANGUL:
            lv_label_set_text(mode_label, "한글");
            lv_obj_set_style_bg_color(mode_btn, lv_color_hex(0x4CAF50), 0);
            break;
        case KOREAN_MODE_ENGLISH:
            lv_label_set_text(mode_label, "ABC");
            lv_obj_set_style_bg_color(mode_btn, lv_color_hex(0x2196F3), 0);
            break;
        case KOREAN_MODE_NUMBER:
            lv_label_set_text(mode_label, "123");
            lv_obj_set_style_bg_color(mode_btn, lv_color_hex(0xFF9800), 0);
            break;
    }
}

// Korean keypad clear callback
void korean_keypad_clear_cb(lv_event_t * e) {
    korean_keypad_buffer[0] = '\0';
    reset_hangul_composition(&korean_keypad_state.composition);
    update_korean_display();
}

// Korean keypad backspace callback
void korean_keypad_backspace_cb(lv_event_t * e) {
    remove_char_from_buffer();
}

// Korean keypad enter callback
void korean_keypad_enter_cb(lv_event_t * e) {
    if (strlen(korean_keypad_buffer) > 0) {
        printf("Entered text: %s\n", korean_keypad_buffer);
        // Here you can add logic to process the entered text
        korean_keypad_buffer[0] = '\0';
        reset_hangul_composition(&korean_keypad_state.composition);
        update_korean_display();
    }
}

// Korean keypad space callback
void korean_keypad_space_cb(lv_event_t * e) {
    add_char_to_buffer(" ");
} 