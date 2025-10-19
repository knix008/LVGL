/**
 * @file audio_gui.c
 * @brief Audio player GUI implementation (320x640 resolution)
 */

#include "audio_gui.h"
#include "audio_player.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

// GUI components
static lv_obj_t *screen;
static lv_obj_t *title_label;
static lv_obj_t *artist_label;
static lv_obj_t *album_label;
static lv_obj_t *time_current_label;
static lv_obj_t *time_total_label;
static lv_obj_t *progress_slider;
static lv_obj_t *play_pause_btn;
static lv_obj_t *play_pause_label;
static lv_obj_t *stop_btn;
static lv_obj_t *volume_slider;
static lv_obj_t *volume_label;
static lv_obj_t *file_list;

static bool is_seeking = false;

// Format time in MM:SS format
static void format_time(char *buf, uint32_t time_ms)
{
    uint32_t total_seconds = time_ms / 1000;
    uint32_t minutes = total_seconds / 60;
    uint32_t seconds = total_seconds % 60;
    sprintf(buf, "%02u:%02u", minutes, seconds);
}

// Event handlers
static void play_pause_btn_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    
    if (code == LV_EVENT_CLICKED) {
        audio_state_t state = audio_player_get_state();
        
        if (state == AUDIO_STATE_PLAYING) {
            audio_player_pause();
        } else {
            audio_player_play();
        }
    }
}

static void stop_btn_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    
    if (code == LV_EVENT_CLICKED) {
        audio_player_stop();
    }
}

static void progress_slider_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    
    if (code == LV_EVENT_PRESSING) {
        is_seeking = true;
    } else if (code == LV_EVENT_RELEASED) {
        int32_t value = lv_slider_get_value(progress_slider);
        audio_player_seek(value);
        is_seeking = false;
    }
}

static void volume_slider_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    
    if (code == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *slider = lv_event_get_target(e);
        if (!slider) return;
        
        int32_t value = lv_slider_get_value(slider);
        audio_player_set_volume((uint8_t)value);
        
        char buf[16];
        sprintf(buf, "Vol: %d%%", (int)value);
        if (volume_label) {
            lv_label_set_text(volume_label, buf);
        }
    }
}


static void file_list_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);
    
    if (code == LV_EVENT_CLICKED) {
        const char *txt = lv_list_get_button_text(file_list, btn);
        if (txt) {
            printf("Loading file: %s\n", txt);
            
            // Build the full path
            char filepath[512];
            snprintf(filepath, sizeof(filepath), "audio/%s", txt);
            
            if (audio_player_load(filepath)) {
                audio_metadata_t metadata;
                if (audio_player_get_metadata(&metadata)) {
                    audio_gui_update_track_info(metadata.title, metadata.artist, metadata.album);
                }
                audio_player_play();
            } else {
                printf("Failed to load: %s\n", filepath);
            }
        }
    }
}

// Check if file has MP3 or WAV extension
static bool is_audio_file(const char *filename)
{
    size_t len = strlen(filename);
    if (len < 4) return false;
    
    const char *ext = filename + len - 4;
    return (strcasecmp(ext, ".mp3") == 0 || strcasecmp(ext, ".wav") == 0);
}

// Scan directory and add audio files to playlist
static void scan_music_directory(void)
{
    const char *music_dir = "audio";
    DIR *dir = opendir(music_dir);
    
    if (!dir) {
        printf("Warning: Could not open directory '%s'\n", music_dir);
        printf("Creating example playlist entries...\n");
        
        // Add example entries if directory doesn't exist or is empty
        lv_obj_t *btn;
        btn = lv_list_add_button(file_list, LV_SYMBOL_AUDIO, "example1.mp3");
        lv_obj_add_event_cb(btn, file_list_event_cb, LV_EVENT_CLICKED, NULL);
        
        btn = lv_list_add_button(file_list, LV_SYMBOL_AUDIO, "example2.wav");
        lv_obj_add_event_cb(btn, file_list_event_cb, LV_EVENT_CLICKED, NULL);
        
        btn = lv_list_add_button(file_list, LV_SYMBOL_AUDIO, "example3.mp3");
        lv_obj_add_event_cb(btn, file_list_event_cb, LV_EVENT_CLICKED, NULL);
        
        return;
    }
    
    struct dirent *entry;
    int file_count = 0;
    
    printf("Scanning '%s' for audio files...\n", music_dir);
    
    // Read directory entries
    while ((entry = readdir(dir)) != NULL) {
        // Skip hidden files and directories
        if (entry->d_name[0] == '.') {
            continue;
        }
        
        // Check if it's a regular file and has audio extension
        if (entry->d_type == DT_REG || entry->d_type == DT_UNKNOWN) {
            if (is_audio_file(entry->d_name)) {
                // Add to playlist
                lv_obj_t *btn = lv_list_add_button(file_list, LV_SYMBOL_AUDIO, entry->d_name);
                lv_obj_add_event_cb(btn, file_list_event_cb, LV_EVENT_CLICKED, NULL);
                file_count++;
                printf("  Added: %s\n", entry->d_name);
            }
        }
    }
    
    closedir(dir);
    
    if (file_count == 0) {
        printf("No audio files found in '%s'\n", music_dir);
        printf("Please add MP3 or WAV files to this directory.\n");
        
        // Add a message to the list
        lv_obj_t *btn = lv_list_add_button(file_list, LV_SYMBOL_WARNING, "No audio files found");
        lv_obj_add_flag(btn, LV_OBJ_FLAG_CLICKABLE);  // Make it non-clickable
    } else {
        printf("Found %d audio file(s)\n", file_count);
    }
}

static void keyboard_event_cb(lv_event_t *e)
{
    (void)e;  // Event parameter not used
    uint32_t key = lv_indev_get_key(lv_indev_active());
    
    if (key == LV_KEY_ESC || key == 'q' || key == 'Q') {
        // Quit signal will be handled by main loop
        exit(0);
    } else if (key == ' ') {
        // Spacebar for play/pause
        audio_state_t state = audio_player_get_state();
        if (state == AUDIO_STATE_PLAYING) {
            audio_player_pause();
        } else {
            audio_player_play();
        }
    }
}

lv_obj_t *audio_gui_create(void)
{
    // Create main screen
    screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x1a1a1a), 0);
    lv_screen_load(screen);
    
    // Add keyboard event handler to screen
    lv_obj_add_event_cb(screen, keyboard_event_cb, LV_EVENT_KEY, NULL);
    lv_group_t *g = lv_group_create();
    lv_group_add_obj(g, screen);
    lv_indev_set_group(lv_indev_active(), g);
    
    // Title section (top area)
    lv_obj_t *info_container = lv_obj_create(screen);
    lv_obj_set_size(info_container, 300, 140);
    lv_obj_align(info_container, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_bg_color(info_container, lv_color_hex(0x2a2a2a), 0);
    lv_obj_set_style_border_color(info_container, lv_color_hex(0x404040), 0);
    
    title_label = lv_label_create(info_container);
    lv_label_set_text(title_label, "No Track Loaded");
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(title_label, lv_color_hex(0xffffff), 0);
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 10);
    lv_label_set_long_mode(title_label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_width(title_label, 280);
    
    artist_label = lv_label_create(info_container);
    lv_label_set_text(artist_label, "Unknown Artist");
    lv_obj_set_style_text_font(artist_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(artist_label, lv_color_hex(0xcccccc), 0);
    lv_obj_align(artist_label, LV_ALIGN_TOP_MID, 0, 50);
    lv_label_set_long_mode(artist_label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_width(artist_label, 280);
    
    album_label = lv_label_create(info_container);
    lv_label_set_text(album_label, "Unknown Album");
    lv_obj_set_style_text_font(album_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(album_label, lv_color_hex(0x999999), 0);
    lv_obj_align(album_label, LV_ALIGN_TOP_MID, 0, 80);
    lv_label_set_long_mode(album_label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_width(album_label, 280);
    
    // Progress slider
    progress_slider = lv_slider_create(screen);
    lv_obj_set_size(progress_slider, 280, 10);
    lv_obj_align(progress_slider, LV_ALIGN_TOP_MID, 0, 170);
    lv_slider_set_range(progress_slider, 0, 100);
    lv_slider_set_value(progress_slider, 0, LV_ANIM_OFF);
    lv_obj_add_event_cb(progress_slider, progress_slider_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_set_style_bg_color(progress_slider, lv_color_hex(0x404040), LV_PART_MAIN);
    lv_obj_set_style_bg_color(progress_slider, lv_color_hex(0x00a8ff), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(progress_slider, lv_color_hex(0x00d4ff), LV_PART_KNOB);
    
    // Time labels
    time_current_label = lv_label_create(screen);
    lv_label_set_text(time_current_label, "00:00");
    lv_obj_set_style_text_color(time_current_label, lv_color_hex(0xffffff), 0);
    lv_obj_align(time_current_label, LV_ALIGN_TOP_LEFT, 20, 190);
    
    time_total_label = lv_label_create(screen);
    lv_label_set_text(time_total_label, "00:00");
    lv_obj_set_style_text_color(time_total_label, lv_color_hex(0xffffff), 0);
    lv_obj_align(time_total_label, LV_ALIGN_TOP_RIGHT, -20, 190);
    
    // Control buttons
    int btn_y = 230;
    
    // Play/Pause button
    play_pause_btn = lv_button_create(screen);
    lv_obj_set_size(play_pause_btn, 100, 50);
    lv_obj_align(play_pause_btn, LV_ALIGN_TOP_MID, -55, btn_y);
    lv_obj_add_event_cb(play_pause_btn, play_pause_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_bg_color(play_pause_btn, lv_color_hex(0x00a8ff), 0);
    
    play_pause_label = lv_label_create(play_pause_btn);
    lv_label_set_text(play_pause_label, LV_SYMBOL_PLAY);
    lv_obj_set_style_text_font(play_pause_label, &lv_font_montserrat_20, 0);
    lv_obj_center(play_pause_label);
    
    // Stop button
    stop_btn = lv_button_create(screen);
    lv_obj_set_size(stop_btn, 100, 50);
    lv_obj_align(stop_btn, LV_ALIGN_TOP_MID, 55, btn_y);
    lv_obj_add_event_cb(stop_btn, stop_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_bg_color(stop_btn, lv_color_hex(0xff4444), 0);
    
    lv_obj_t *stop_label = lv_label_create(stop_btn);
    lv_label_set_text(stop_label, LV_SYMBOL_STOP);
    lv_obj_set_style_text_font(stop_label, &lv_font_montserrat_20, 0);
    lv_obj_center(stop_label);
    
    // Volume control
    lv_obj_t *volume_container = lv_obj_create(screen);
    lv_obj_set_size(volume_container, 280, 80);
    lv_obj_align(volume_container, LV_ALIGN_TOP_MID, 0, 300);
    lv_obj_set_style_bg_color(volume_container, lv_color_hex(0x2a2a2a), 0);
    
    lv_obj_t *vol_icon = lv_label_create(volume_container);
    lv_label_set_text(vol_icon, LV_SYMBOL_VOLUME_MAX);
    lv_obj_set_style_text_color(vol_icon, lv_color_hex(0xffffff), 0);
    lv_obj_align(vol_icon, LV_ALIGN_LEFT_MID, 10, 0);
    
    volume_slider = lv_slider_create(volume_container);
    lv_obj_set_size(volume_slider, 180, 10);
    lv_obj_align(volume_slider, LV_ALIGN_CENTER, 10, 0);
    lv_slider_set_range(volume_slider, 0, 100);
    lv_slider_set_value(volume_slider, 70, LV_ANIM_OFF);
    lv_obj_add_event_cb(volume_slider, volume_slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_set_style_bg_color(volume_slider, lv_color_hex(0x404040), LV_PART_MAIN);
    lv_obj_set_style_bg_color(volume_slider, lv_color_hex(0x00a8ff), LV_PART_INDICATOR);
    
    volume_label = lv_label_create(volume_container);
    lv_label_set_text(volume_label, "Vol: 70%");
    lv_obj_set_style_text_color(volume_label, lv_color_hex(0xffffff), 0);
    lv_obj_align(volume_label, LV_ALIGN_CENTER, 0, -20);
    
    // File list
    lv_obj_t *list_label = lv_label_create(screen);
    lv_label_set_text(list_label, "Playlist");
    lv_obj_set_style_text_color(list_label, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_text_font(list_label, &lv_font_montserrat_14, 0);
    lv_obj_align(list_label, LV_ALIGN_TOP_LEFT, 20, 400);
    
    file_list = lv_list_create(screen);
    lv_obj_set_size(file_list, 280, 200);
    lv_obj_align(file_list, LV_ALIGN_TOP_MID, 0, 430);
    lv_obj_set_style_bg_color(file_list, lv_color_hex(0x2a2a2a), 0);
    
    // Scan and load audio files from assets/music directory
    scan_music_directory();
    
    // Set initial volume
    audio_player_set_volume(70);
    
    return screen;
}

void audio_gui_update_position(uint32_t position_ms, uint32_t duration_ms)
{
    if (is_seeking) {
        return;  // Don't update while user is seeking
    }
    
    // Update time labels
    char buf[16];
    format_time(buf, position_ms);
    lv_label_set_text(time_current_label, buf);
    
    format_time(buf, duration_ms);
    lv_label_set_text(time_total_label, buf);
    
    // Update progress slider
    if (duration_ms > 0) {
        lv_slider_set_range(progress_slider, 0, duration_ms);
        lv_slider_set_value(progress_slider, position_ms, LV_ANIM_OFF);
    }
}

void audio_gui_update_state(bool is_playing)
{
    if (is_playing) {
        lv_label_set_text(play_pause_label, LV_SYMBOL_PAUSE);
    } else {
        lv_label_set_text(play_pause_label, LV_SYMBOL_PLAY);
    }
}

void audio_gui_update_track_info(const char *title, const char *artist, const char *album)
{
    if (title && strlen(title) > 0) {
        lv_label_set_text(title_label, title);
    } else {
        lv_label_set_text(title_label, "Unknown Title");
    }
    
    if (artist && strlen(artist) > 0) {
        lv_label_set_text(artist_label, artist);
    } else {
        lv_label_set_text(artist_label, "Unknown Artist");
    }
    
    if (album && strlen(album) > 0) {
        lv_label_set_text(album_label, album);
    } else {
        lv_label_set_text(album_label, "Unknown Album");
    }
}

