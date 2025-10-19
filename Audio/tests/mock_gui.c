/**
 * @file mock_gui.c
 * @brief Mock GUI functions for testing audio player backend
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

// Mock implementations of GUI update functions
void audio_gui_update_position(uint32_t position_ms, uint32_t duration_ms)
{
    // Mock: just print for debugging if needed
    (void)position_ms;
    (void)duration_ms;
    // printf("[MOCK GUI] Position: %u/%u ms\n", position_ms, duration_ms);
}

void audio_gui_update_state(bool is_playing)
{
    // Mock: just print for debugging if needed
    (void)is_playing;
    // printf("[MOCK GUI] State: %s\n", is_playing ? "PLAYING" : "STOPPED/PAUSED");
}

void audio_gui_update_track_info(const char *title, const char *artist, const char *album)
{
    // Mock: just print for debugging if needed
    (void)title;
    (void)artist;
    (void)album;
    // printf("[MOCK GUI] Track: %s - %s (%s)\n", title, artist, album);
}

