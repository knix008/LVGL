#include "lvgl.h"
#include "korean_fonts.h"
#include "video_player.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <SDL2/SDL.h>

// Global variables for GUI
static lv_obj_t *main_screen;
static lv_obj_t *video_area;
static lv_obj_t *control_panel;
static lv_obj_t *play_btn;
static lv_obj_t *pause_btn;
static lv_obj_t *stop_btn;
static lv_obj_t *progress_bar;
static lv_obj_t *time_label;
static lv_obj_t *file_list;
static lv_obj_t *file_label;
static lv_obj_t *volume_slider;
static lv_obj_t *volume_label;

// Function prototypes for GUI
static void play_button_cb(lv_event_t *e);
static void pause_button_cb(lv_event_t *e);
static void stop_button_cb(lv_event_t *e);
static void file_selected_cb(lv_event_t *e);
static void volume_changed_cb(lv_event_t *e);
static void progress_changed_cb(lv_event_t *e);
static void update_timer_cb(lv_timer_t *timer);
static void load_video_files(void);
static void update_time_display(void);
static void update_control_buttons(void);
static void create_video_area(void);
static void create_control_panel(void);
static void create_file_browser(void);
static void create_volume_control(void);
static void refresh_playlist_cb(lv_event_t *e);

// Play button callback
static void play_button_cb(lv_event_t *e) {
    (void)e; // Suppress unused parameter warning
    if (video_player_get_current_file()[0] == '\0') {
        lv_label_set_text(file_label, "파일을 선택하세요");
        return;
    }
    
    video_player_play();
    update_control_buttons();
    lv_label_set_text(file_label, "재생 중...");
}

// Pause button callback
static void pause_button_cb(lv_event_t *e) {
    (void)e; // Suppress unused parameter warning
    if (video_player_get_state() == PLAYER_PLAYING) {
        video_player_pause();
        update_control_buttons();
        lv_label_set_text(file_label, "일시정지");
    }
}

// Stop button callback
static void stop_button_cb(lv_event_t *e) {
    (void)e; // Suppress unused parameter warning
    video_player_stop();
    lv_bar_set_value(progress_bar, 0, LV_ANIM_OFF);
    update_control_buttons();
    update_time_display();
    lv_label_set_text(file_label, "정지됨");
}

// File selection callback
static void file_selected_cb(lv_event_t *e) {
    lv_obj_t *btn = lv_event_get_target(e);
    const char *filename = lv_list_get_button_text(file_list, btn);
    
    if (filename) {
        video_player_load_file(filename);
        lv_label_set_text(file_label, filename);
        lv_bar_set_value(progress_bar, 0, LV_ANIM_OFF);
        update_time_display();
        update_control_buttons();
    }
}

// Volume change callback
static void volume_changed_cb(lv_event_t *e) {
    lv_obj_t *slider = lv_event_get_target(e);
    int volume = lv_slider_get_value(slider);

    // Set volume in video player (0-100)
    video_player_set_volume(volume);

    char vol_text[32];
    snprintf(vol_text, sizeof(vol_text), "볼륨: %d%%", volume);
    lv_label_set_text(volume_label, vol_text);
}

// Progress bar change callback
static void progress_changed_cb(lv_event_t *e) {
    lv_obj_t *bar = lv_event_get_target(e);
    int32_t value = lv_bar_get_value(bar);
    
    int total_duration = video_player_get_duration();
    if (total_duration > 0) {
        int position = (value * total_duration) / 100;
        video_player_seek(position);
        update_time_display();
    }
}

// Update timer callback
static void update_timer_cb(lv_timer_t *timer) {
    (void)timer; // Suppress unused parameter warning
    
    video_player_update();
    
    int state = video_player_get_state();
    if (state == PLAYER_PLAYING) {
        int position = video_player_get_position();
        int duration = video_player_get_duration();
        
        if (position >= duration && duration > 0) {
            video_player_stop();
            update_control_buttons();
            lv_label_set_text(file_label, "재생 완료");
        }
        
        int32_t progress = (position * 100) / duration;
        lv_bar_set_value(progress_bar, progress, LV_ANIM_OFF);
        update_time_display();
    }
}

// Load video files from video directory
static void load_video_files(void) {
    DIR *dir;
    struct dirent *entry;
    
    // Clear existing items
    lv_obj_clean(file_list);
    
    // Add video files from video directory
    dir = opendir("video");
    if (dir != NULL) {
        char path[512];
        struct stat st;
        while ((entry = readdir(dir)) != NULL) {
            snprintf(path, sizeof(path), "video/%s", entry->d_name);
            if (stat(path, &st) == 0 && S_ISREG(st.st_mode)) {
                const char *filename = entry->d_name;
                const char *ext = strrchr(filename, '.');
                
                // Check for video file extensions
                if (ext && (strcmp(ext, ".mp4") == 0 || strcmp(ext, ".avi") == 0 || 
                           strcmp(ext, ".mkv") == 0 || strcmp(ext, ".mov") == 0 ||
                           strcmp(ext, ".wmv") == 0 || strcmp(ext, ".flv") == 0)) {
                    
                    lv_obj_t *btn = lv_list_add_btn(file_list, LV_SYMBOL_FILE, filename);
                    lv_obj_add_event_cb(btn, file_selected_cb, LV_EVENT_CLICKED, NULL);
                }
            }
        }
        closedir(dir);
    }
    
    // Add some sample files if directory is empty
    if (lv_obj_get_child_cnt(file_list) == 0) {
        lv_list_add_btn(file_list, LV_SYMBOL_FILE, "sample_video1.mp4");
        lv_list_add_btn(file_list, LV_SYMBOL_FILE, "sample_video2.avi");
        lv_list_add_btn(file_list, LV_SYMBOL_FILE, "sample_video3.mkv");
    }
}

// Update time display
static void update_time_display(void) {
    int current_pos = video_player_get_position();
    int total_duration = video_player_get_duration();
    
    int current_min = current_pos / 60;
    int current_sec = current_pos % 60;
    int total_min = total_duration / 60;
    int total_sec = total_duration % 60;
    
    char time_text[32];
    snprintf(time_text, sizeof(time_text), "%02d:%02d / %02d:%02d", 
             current_min, current_sec, total_min, total_sec);
    lv_label_set_text(time_label, time_text);
}

// Update control buttons state
static void update_control_buttons(void) {
    int state = video_player_get_state();
    switch (state) {
        case PLAYER_STOPPED:
            lv_obj_clear_state(play_btn, LV_STATE_DISABLED);
            lv_obj_add_state(pause_btn, LV_STATE_DISABLED);
            lv_obj_add_state(stop_btn, LV_STATE_DISABLED);
            break;
        case PLAYER_PLAYING:
            lv_obj_add_state(play_btn, LV_STATE_DISABLED);
            lv_obj_clear_state(pause_btn, LV_STATE_DISABLED);
            lv_obj_clear_state(stop_btn, LV_STATE_DISABLED);
            break;
        case PLAYER_PAUSED:
            lv_obj_clear_state(play_btn, LV_STATE_DISABLED);
            lv_obj_add_state(pause_btn, LV_STATE_DISABLED);
            lv_obj_clear_state(stop_btn, LV_STATE_DISABLED);
            break;
    }
}

// Create video display area
static void create_video_area(void) {
    // Create a container for aspect ratio control - now wider and centered
    lv_obj_t *video_container = lv_obj_create(main_screen);
    lv_obj_set_size(video_container, 640, 360);
    lv_obj_align(video_container, LV_ALIGN_TOP_LEFT, 10, 10);
    lv_obj_set_style_bg_color(video_container, lv_color_hex(0x000000), 0);
    lv_obj_set_style_border_width(video_container, 2, 0);
    lv_obj_set_style_border_color(video_container, lv_color_hex(0x0078D4), 0);
    lv_obj_set_style_radius(video_container, 4, 0);
    lv_obj_set_style_pad_all(video_container, 0, 0);
    
    // Try to create LVGL FFmpeg player inside the container
    video_area = lv_ffmpeg_player_create(video_container);
    if (video_area != NULL) {
        // Set the video player to fill the container but maintain aspect ratio
        lv_obj_set_size(video_area, 640, 360);
        lv_obj_align(video_area, LV_ALIGN_CENTER, 0, 0);

        // Set the video player object in our video player logic
        video_player_set_object(video_area);

        // Enable auto restart
        lv_ffmpeg_player_set_auto_restart(video_area, true);

        printf("LVGL FFmpeg player created successfully with audio support\n");
    } else {
        printf("Error: Failed to create LVGL FFmpeg player, using fallback\n");

        // Fallback: create a regular object with placeholder text
        video_area = lv_obj_create(video_container);
        lv_obj_set_size(video_area, 640, 360);
        lv_obj_align(video_area, LV_ALIGN_CENTER, 0, 0);
        lv_obj_set_style_bg_color(video_area, lv_color_hex(0x000000), 0);

        // Set the video player object in our video player logic
        video_player_set_object(video_area);

        // Add placeholder text
        lv_obj_t *placeholder = lv_label_create(video_area);
        lv_label_set_text(placeholder, "비디오 영역\n\nFFmpeg 플레이어를 사용할 수 없습니다");
        lv_obj_set_style_text_color(placeholder, lv_color_white(), 0);
        lv_obj_set_style_text_font(placeholder, nanum_gothic_16, 0);
        lv_obj_center(placeholder);
    }
}

// Create control panel
static void create_control_panel(void) {
    control_panel = lv_obj_create(main_screen);
    lv_obj_set_size(control_panel, 640, 120);
    lv_obj_align(control_panel, LV_ALIGN_TOP_LEFT, 10, 380);
    lv_obj_set_style_bg_color(control_panel, lv_color_hex(0x1E1E1E), 0);
    lv_obj_set_style_border_width(control_panel, 2, 0);
    lv_obj_set_style_border_color(control_panel, lv_color_hex(0x0078D4), 0);
    lv_obj_set_style_radius(control_panel, 4, 0);
    lv_obj_set_style_pad_all(control_panel, 10, 0);

    // File info label at top
    file_label = lv_label_create(control_panel);
    lv_label_set_text(file_label, "파일을 선택하세요");
    lv_obj_set_style_text_font(file_label, nanum_gothic_16, 0);
    lv_obj_set_style_text_color(file_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(file_label, LV_ALIGN_TOP_LEFT, 5, 5);

    // Time label at top right
    time_label = lv_label_create(control_panel);
    lv_label_set_text(time_label, "00:00 / 00:00");
    lv_obj_set_style_text_font(time_label, nanum_gothic_16, 0);
    lv_obj_set_style_text_color(time_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(time_label, LV_ALIGN_TOP_RIGHT, -5, 5);

    // Progress bar
    progress_bar = lv_bar_create(control_panel);
    lv_obj_set_size(progress_bar, 620, 10);
    lv_obj_align(progress_bar, LV_ALIGN_TOP_MID, 0, 35);
    lv_obj_set_style_bg_color(progress_bar, lv_color_hex(0x404040), LV_PART_MAIN);
    lv_obj_set_style_bg_color(progress_bar, lv_color_hex(0x0078D4), LV_PART_INDICATOR);
    lv_obj_set_style_radius(progress_bar, 5, LV_PART_MAIN);
    lv_obj_set_style_radius(progress_bar, 5, LV_PART_INDICATOR);
    lv_obj_add_event_cb(progress_bar, progress_changed_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // Control buttons - centered
    play_btn = lv_btn_create(control_panel);
    lv_obj_set_size(play_btn, 60, 45);
    lv_obj_align(play_btn, LV_ALIGN_BOTTOM_MID, -70, -5);
    lv_obj_set_style_bg_color(play_btn, lv_color_hex(0x0078D4), 0);
    lv_obj_t *play_label = lv_label_create(play_btn);
    lv_label_set_text(play_label, LV_SYMBOL_PLAY);
    lv_obj_set_style_text_font(play_label, &lv_font_montserrat_20, 0);
    lv_obj_center(play_label);
    lv_obj_add_event_cb(play_btn, play_button_cb, LV_EVENT_CLICKED, NULL);

    pause_btn = lv_btn_create(control_panel);
    lv_obj_set_size(pause_btn, 60, 45);
    lv_obj_align(pause_btn, LV_ALIGN_BOTTOM_MID, 0, -5);
    lv_obj_set_style_bg_color(pause_btn, lv_color_hex(0x0078D4), 0);
    lv_obj_t *pause_label = lv_label_create(pause_btn);
    lv_label_set_text(pause_label, LV_SYMBOL_PAUSE);
    lv_obj_set_style_text_font(pause_label, &lv_font_montserrat_20, 0);
    lv_obj_center(pause_label);
    lv_obj_add_event_cb(pause_btn, pause_button_cb, LV_EVENT_CLICKED, NULL);

    stop_btn = lv_btn_create(control_panel);
    lv_obj_set_size(stop_btn, 60, 45);
    lv_obj_align(stop_btn, LV_ALIGN_BOTTOM_MID, 70, -5);
    lv_obj_set_style_bg_color(stop_btn, lv_color_hex(0x0078D4), 0);
    lv_obj_t *stop_label = lv_label_create(stop_btn);
    lv_label_set_text(stop_label, LV_SYMBOL_STOP);
    lv_obj_set_style_text_font(stop_label, &lv_font_montserrat_20, 0);
    lv_obj_center(stop_label);
    lv_obj_add_event_cb(stop_btn, stop_button_cb, LV_EVENT_CLICKED, NULL);

    update_control_buttons();
}

// Create file browser
static void create_file_browser(void) {
    lv_obj_t *file_panel = lv_obj_create(main_screen);
    lv_obj_set_size(file_panel, 130, 580);
    lv_obj_align(file_panel, LV_ALIGN_TOP_RIGHT, -10, 10);
    lv_obj_set_style_bg_color(file_panel, lv_color_hex(0x1E1E1E), 0);
    lv_obj_set_style_border_width(file_panel, 2, 0);
    lv_obj_set_style_border_color(file_panel, lv_color_hex(0x0078D4), 0);
    lv_obj_set_style_radius(file_panel, 4, 0);
    lv_obj_set_style_pad_all(file_panel, 8, 0);

    // File list title
    lv_obj_t *title = lv_label_create(file_panel);
    lv_label_set_text(title, "재생목록");
    lv_obj_set_style_text_font(title, nanum_gothic_bold_16, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 5);

    // Refresh button
    lv_obj_t *refresh_btn = lv_btn_create(file_panel);
    lv_obj_set_size(refresh_btn, 114, 35);
    lv_obj_align(refresh_btn, LV_ALIGN_TOP_MID, 0, 30);
    lv_obj_set_style_bg_color(refresh_btn, lv_color_hex(0x0078D4), 0);
    lv_obj_t *refresh_label = lv_label_create(refresh_btn);
    lv_label_set_text(refresh_label, "새로고침");
    lv_obj_set_style_text_font(refresh_label, nanum_gothic_16, 0);
    lv_obj_center(refresh_label);
    lv_obj_add_event_cb(refresh_btn, refresh_playlist_cb, LV_EVENT_CLICKED, NULL);

    // File list
    file_list = lv_list_create(file_panel);
    lv_obj_set_size(file_list, 114, 490);
    lv_obj_align(file_list, LV_ALIGN_TOP_MID, 0, 75);
    lv_obj_set_style_bg_color(file_list, lv_color_hex(0x2D2D2D), 0);
    lv_obj_set_style_border_width(file_list, 1, 0);
    lv_obj_set_style_border_color(file_list, lv_color_hex(0x404040), 0);
    lv_obj_set_style_radius(file_list, 4, 0);

    // Load video files
    load_video_files();
}

// Refresh playlist callback
static void refresh_playlist_cb(lv_event_t *e) {
    (void)e;
    load_video_files();
}

// Create volume control
static void create_volume_control(void) {
    lv_obj_t *vol_panel = lv_obj_create(main_screen);
    lv_obj_set_size(vol_panel, 640, 80);
    lv_obj_align(vol_panel, LV_ALIGN_TOP_LEFT, 10, 510);
    lv_obj_set_style_bg_color(vol_panel, lv_color_hex(0x1E1E1E), 0);
    lv_obj_set_style_border_width(vol_panel, 2, 0);
    lv_obj_set_style_border_color(vol_panel, lv_color_hex(0x0078D4), 0);
    lv_obj_set_style_radius(vol_panel, 4, 0);
    lv_obj_set_style_pad_all(vol_panel, 10, 0);

    // Volume label
    volume_label = lv_label_create(vol_panel);
    lv_label_set_text(volume_label, "볼륨: 50%");
    lv_obj_set_style_text_font(volume_label, nanum_gothic_16, 0);
    lv_obj_set_style_text_color(volume_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(volume_label, LV_ALIGN_TOP_LEFT, 5, 5);

    // Volume slider
    volume_slider = lv_slider_create(vol_panel);
    lv_obj_set_size(volume_slider, 620, 12);
    lv_obj_align(volume_slider, LV_ALIGN_BOTTOM_MID, 0, -5);
    lv_slider_set_range(volume_slider, 0, 100);
    lv_slider_set_value(volume_slider, 50, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(volume_slider, lv_color_hex(0x404040), LV_PART_MAIN);
    lv_obj_set_style_bg_color(volume_slider, lv_color_hex(0x0078D4), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(volume_slider, lv_color_hex(0xFFFFFF), LV_PART_KNOB);
    lv_obj_set_style_radius(volume_slider, 6, LV_PART_MAIN);
    lv_obj_set_style_radius(volume_slider, 6, LV_PART_INDICATOR);
    lv_obj_add_event_cb(volume_slider, volume_changed_cb, LV_EVENT_VALUE_CHANGED, NULL);
}

// Main application initialization
void gui_init(void) {
    // Load Korean fonts first
    load_korean_fonts();
    
    // Initialize LVGL file system for FFmpeg
    lv_fs_posix_init();
    
    // Audio will be initialized in main function
    
    // Initialize video player
    video_player_init();
    
    // Create main screen
    main_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(main_screen, lv_color_hex(0x22232A), 0);
    lv_obj_set_style_pad_all(main_screen, 0, 0);
    lv_scr_load(main_screen);
    
    // Create UI components
    create_video_area();
    create_control_panel();
    create_file_browser();
    create_volume_control();
    
    // Create update timer
    lv_timer_t *update_timer = lv_timer_create(update_timer_cb, 1000, NULL);
    lv_timer_set_repeat_count(update_timer, -1);
}

// Main function
int main(int argc, char *argv[]) {
    (void)argc; // Suppress unused parameter warning
    (void)argv; // Suppress unused parameter warning

    // Initialize SDL with audio support BEFORE LVGL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("Error: Could not initialize SDL: %s\n", SDL_GetError());
        return 1;
    }
    printf("SDL initialized with video and audio support\n");

    // Initialize LVGL
    lv_init();

    // Initialize display driver (SDL2)
    lv_display_t *disp = lv_sdl_window_create(800, 600);
    lv_display_set_default(disp);

    // Initialize input driver
    lv_indev_t *indev = lv_sdl_mouse_create();
    lv_indev_set_group(indev, lv_group_get_default());

    printf("Display and input initialized, FFmpeg player will handle audio playback\n");

    // Initialize the GUI
    gui_init();
    
    // Main loop
    while (1) {
        lv_timer_handler();
        lv_tick_inc(5);
        usleep(5000);
    }
    
    return 0;
}
