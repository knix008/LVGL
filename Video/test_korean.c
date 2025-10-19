#include "lvgl.h"
#include "korean_fonts.h"
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    
    printf("Testing Korean font loading...\n");
    
    // Initialize LVGL
    lv_init();
    
    // Initialize display driver (SDL2)
    lv_display_t *disp = lv_sdl_window_create(640, 480);
    lv_display_set_default(disp);
    
    // Initialize input driver
    lv_indev_t *indev = lv_sdl_mouse_create();
    lv_indev_set_group(indev, lv_group_get_default());
    
    // Load Korean fonts
    load_korean_fonts();
    
    // Create a simple screen with Korean text
    lv_obj_t *screen = lv_obj_create(NULL);
    lv_scr_load(screen);
    
    // Set background color
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x2C2C2C), 0);
    
    // Create labels with Korean text
    lv_obj_t *label1 = lv_label_create(screen);
    lv_label_set_text(label1, "안녕하세요! Korean Font Test");
    lv_obj_set_style_text_font(label1, nanum_gothic_16, 0);
    lv_obj_set_style_text_color(label1, lv_color_white(), 0);
    lv_obj_align(label1, LV_ALIGN_TOP_MID, 0, 50);
    
    lv_obj_t *label2 = lv_label_create(screen);
    lv_label_set_text(label2, "비디오 플레이어 - Video Player");
    lv_obj_set_style_text_font(label2, nanum_gothic_bold_16, 0);
    lv_obj_set_style_text_color(label2, lv_color_hex(0x00FF00), 0);
    lv_obj_align(label2, LV_ALIGN_CENTER, 0, 0);
    
    lv_obj_t *label3 = lv_label_create(screen);
    lv_label_set_text(label3, "한글 폰트가 제대로 표시되면 성공!");
    lv_obj_set_style_text_font(label3, nanum_gothic_16, 0);
    lv_obj_set_style_text_color(label3, lv_color_hex(0x00BFFF), 0);
    lv_obj_align(label3, LV_ALIGN_BOTTOM_MID, 0, -50);
    
    printf("Korean font test application running.\n");
    printf("Check the window for Korean text display.\n");
    printf("Press Ctrl+C to exit.\n");
    
    // Main loop
    while (1) {
        lv_timer_handler();
        lv_tick_inc(5);
        usleep(5000);
    }
    
    return 0;
}
