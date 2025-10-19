/**
 * @file main.c
 * @brief Audio Player Application with LVGL
 * 
 * Display Resolution: 320x640
 * Supported Formats: MP3, WAV
 */

#include "lvgl/lvgl.h"
#include "lv_conf.h"
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "audio_player.h"
#include "audio_gui.h"

#define DISP_HOR_RES 320
#define DISP_VER_RES 640

static volatile bool quit_flag = false;

static void signal_handler(int signum)
{
    (void)signum;
    quit_flag = true;
}

int main(void)
{
    // Set up signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Initialize LVGL
    lv_init();

    // Create SDL display with 320x640 resolution
    lv_display_t *disp = lv_sdl_window_create(DISP_HOR_RES, DISP_VER_RES);
    (void)disp;  // Display is managed by LVGL
    
    // Create SDL mouse input device
    lv_indev_t *mouse = lv_sdl_mouse_create();
    (void)mouse;  // Input device is managed by LVGL
    
    // Create SDL keyboard input device
    lv_indev_t *kb = lv_sdl_keyboard_create();
    (void)kb;  // Keyboard is managed by LVGL

    // Initialize audio player backend
    audio_player_init();

    // Create the audio player GUI
    audio_gui_create();

    printf("Audio Player Started (320x640)\n");
    printf("Press Q or ESC to quit\n");

    // Main loop
    while (!quit_flag) {
        uint32_t time_till_next = lv_timer_handler();
        usleep(time_till_next * 1000);
    }

    // Cleanup
    audio_player_cleanup();
    
    printf("Audio Player Stopped\n");

    return 0;
}

