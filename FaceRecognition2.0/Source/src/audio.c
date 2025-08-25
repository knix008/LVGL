#include "audio.h"
#include "lvgl.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Audio system state
static bool g_audio_initialized = false;
static bool g_audio_playing = false;
static bool g_audio_paused = false;
static bool g_audio_muted = false;
static int g_audio_volume = 100;
static char g_audio_current_file[256] = "";
static char g_audio_status[64] = "Not initialized";

// SDL audio state
static SDL_AudioDeviceID g_audio_device = 0;
static SDL_AudioSpec g_audio_spec;
static Uint8* g_audio_buffer = NULL;
static Uint32 g_audio_buffer_len = 0;
static Uint32 g_audio_buffer_pos = 0;

// Audio callback function for SDL
static void audio_callback(void* userdata, Uint8* stream, int len) {
    if (!g_audio_playing || g_audio_paused || g_audio_muted) {
        memset(stream, 0, len);
        return;
    }
    
    if (g_audio_buffer == NULL) {
        memset(stream, 0, len);
        return;
    }
    
    // Calculate how much audio data to copy
    Uint32 remaining = g_audio_buffer_len - g_audio_buffer_pos;
    Uint32 to_copy = (remaining < (Uint32)len) ? remaining : (Uint32)len;
    
    if (to_copy > 0) {
        // Copy audio data
        memcpy(stream, g_audio_buffer + g_audio_buffer_pos, to_copy);
        g_audio_buffer_pos += to_copy;
        
        // Apply volume
        if (g_audio_volume < 100) {
            float volume_factor = g_audio_volume / 100.0f;
            for (int i = 0; i < to_copy; i++) {
                stream[i] = (Uint8)(stream[i] * volume_factor);
            }
        }
        
        // Fill remaining with silence
        if (to_copy < (Uint32)len) {
            memset(stream + to_copy, 0, len - to_copy);
        }
    } else {
        // End of audio data
        memset(stream, 0, len);
        g_audio_playing = false;
        strcpy(g_audio_status, "Finished");
    }
}

// Initialize audio system
void audio_init(void) {
    if (g_audio_initialized) {
        return;
    }
    
    // Initialize SDL audio subsystem
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        printf("Audio: Failed to initialize SDL audio: %s\n", SDL_GetError());
        strcpy(g_audio_status, "Init failed");
        return;
    }
    
    // Set up audio specification
    SDL_zero(g_audio_spec);
    g_audio_spec.freq = 44100;
    g_audio_spec.format = AUDIO_S16SYS;
    g_audio_spec.channels = 2;
    g_audio_spec.samples = 4096;
    g_audio_spec.callback = audio_callback;
    g_audio_spec.userdata = NULL;
    
    // Open audio device
    g_audio_device = SDL_OpenAudioDevice(NULL, 0, &g_audio_spec, NULL, 0);
    if (g_audio_device == 0) {
        printf("Audio: Failed to open audio device: %s\n", SDL_GetError());
        strcpy(g_audio_status, "Device error");
        return;
    }
    
    g_audio_initialized = true;
    strcpy(g_audio_status, "Ready");
    printf("Audio: Initialized successfully\n");
}

// Deinitialize audio system
void audio_deinit(void) {
    if (!g_audio_initialized) {
        return;
    }
    
    audio_stop();
    
    if (g_audio_buffer) {
        SDL_FreeWAV(g_audio_buffer);
        g_audio_buffer = NULL;
        g_audio_buffer_len = 0;
    }
    
    if (g_audio_device) {
        SDL_CloseAudioDevice(g_audio_device);
        g_audio_device = 0;
    }
    
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    g_audio_initialized = false;
    strcpy(g_audio_status, "Not initialized");
    printf("Audio: Deinitialized\n");
}

// Check if audio is initialized
bool audio_is_initialized(void) {
    return g_audio_initialized;
}

// Load audio file using LVGL file system
bool audio_load_file(const char* file_path) {
    if (!g_audio_initialized) {
        printf("Audio: Not initialized\n");
        return false;
    }
    
    // Stop current audio
    audio_stop();
    
    // Free previous buffer
    if (g_audio_buffer) {
        SDL_FreeWAV(g_audio_buffer);
        g_audio_buffer = NULL;
        g_audio_buffer_len = 0;
    }
    
    // Open file using LVGL file system
    lv_fs_file_t file;
    lv_fs_res_t res = lv_fs_open(&file, file_path, LV_FS_MODE_RD);
    if (res != LV_FS_RES_OK) {
        printf("Audio: Failed to open file: %s - LVGL error: %d\n", file_path, res);
        strcpy(g_audio_status, "File open failed");
        return false;
    }
    
    // Get file size
    lv_fs_seek(&file, 0, LV_FS_SEEK_END);
    uint32_t file_size = 0;
    lv_fs_tell(&file, &file_size);
    lv_fs_seek(&file, 0, LV_FS_SEEK_SET);
    
    // Allocate buffer for file data
    Uint8* file_data = (Uint8*)malloc(file_size);
    if (!file_data) {
        printf("Audio: Failed to allocate memory for file data\n");
        lv_fs_close(&file);
        strcpy(g_audio_status, "Memory allocation failed");
        return false;
    }
    
    // Read file data
    uint32_t bytes_read = 0;
    res = lv_fs_read(&file, file_data, file_size, &bytes_read);
    lv_fs_close(&file);
    
    if (res != LV_FS_RES_OK || bytes_read != file_size) {
        printf("Audio: Failed to read file data\n");
        free(file_data);
        strcpy(g_audio_status, "File read failed");
        return false;
    }
    
    // Create SDL_RWops from memory buffer
    SDL_RWops* rw = SDL_RWFromMem(file_data, file_size);
    if (!rw) {
        printf("Audio: Failed to create SDL_RWops\n");
        free(file_data);
        strcpy(g_audio_status, "SDL_RWops creation failed");
        return false;
    }
    
    // Load WAV from memory
    SDL_AudioSpec wav_spec;
    Uint8* wav_buffer;
    Uint32 wav_length;
    
    if (SDL_LoadWAV_RW(rw, 1, &wav_spec, &wav_buffer, &wav_length) == NULL) {
        printf("Audio: Failed to load WAV from memory: %s\n", SDL_GetError());
        free(file_data);
        strcpy(g_audio_status, "WAV load failed");
        return false;
    }
    
    // Convert audio format if necessary
    SDL_AudioCVT cvt;
    if (SDL_BuildAudioCVT(&cvt, wav_spec.format, wav_spec.channels, wav_spec.freq,
                          g_audio_spec.format, g_audio_spec.channels, g_audio_spec.freq) < 0) {
        printf("Audio: Failed to build audio converter: %s\n", SDL_GetError());
        SDL_FreeWAV(wav_buffer);
        free(file_data);
        strcpy(g_audio_status, "Convert failed");
        return false;
    }
    
    if (cvt.needed) {
        cvt.buf = (Uint8*)malloc(wav_length * cvt.len_mult);
        cvt.len = wav_length;
        memcpy(cvt.buf, wav_buffer, wav_length);
        SDL_FreeWAV(wav_buffer);
        
        if (SDL_ConvertAudio(&cvt) < 0) {
            printf("Audio: Failed to convert audio: %s\n", SDL_GetError());
            free(cvt.buf);
            free(file_data);
            strcpy(g_audio_status, "Convert failed");
            return false;
        }
        
        g_audio_buffer = cvt.buf;
        g_audio_buffer_len = cvt.len_cvt;
    } else {
        g_audio_buffer = wav_buffer;
        g_audio_buffer_len = wav_length;
    }
    
    g_audio_buffer_pos = 0;
    strcpy(g_audio_current_file, file_path);
    strcpy(g_audio_status, "Loaded");
    printf("Audio: Loaded file successfully: %s\n", file_path);
    
    // Clean up file data
    free(file_data);
    return true;
}

// Play audio file
bool audio_play_file(const char* file_path) {
    if (!audio_load_file(file_path)) {
        return false;
    }
    return audio_resume();
}

// Stop audio
void audio_stop(void) {
    if (!g_audio_initialized) {
        return;
    }
    
    SDL_PauseAudioDevice(g_audio_device, 1);
    g_audio_playing = false;
    g_audio_paused = false;
    g_audio_buffer_pos = 0;
    strcpy(g_audio_status, "Stopped");
    printf("Audio: Stopped\n");
}

// Pause audio
void audio_pause(void) {
    if (!g_audio_initialized || !g_audio_playing) {
        return;
    }
    
    SDL_PauseAudioDevice(g_audio_device, 1);
    g_audio_paused = true;
    strcpy(g_audio_status, "Paused");
    printf("Audio: Paused\n");
}

// Resume audio
bool audio_resume(void) {
    if (!g_audio_initialized || g_audio_buffer == NULL) {
        return false;
    }
    
    SDL_PauseAudioDevice(g_audio_device, 0);
    g_audio_playing = true;
    g_audio_paused = false;
    strcpy(g_audio_status, "Playing");
    printf("Audio: Resumed\n");
    return true;
}

// Check if audio is playing
bool audio_is_playing(void) {
    return g_audio_playing && !g_audio_paused;
}

// Check if audio is paused
bool audio_is_paused(void) {
    return g_audio_paused;
}

// Set volume (0-100)
void audio_set_volume(int volume) {
    if (volume < 0) volume = 0;
    if (volume > 100) volume = 100;
    g_audio_volume = volume;
    printf("Audio: Volume set to %d%%\n", volume);
}

// Get volume
int audio_get_volume(void) {
    return g_audio_volume;
}

// Set mute
void audio_set_mute(bool mute) {
    g_audio_muted = mute;
    printf("Audio: %s\n", mute ? "Muted" : "Unmuted");
}

// Check if muted
bool audio_is_muted(void) {
    return g_audio_muted;
}

// Get current file
const char* audio_get_current_file(void) {
    return g_audio_current_file;
}

// Get status string
const char* audio_get_status_string(void) {
    return g_audio_status;
}
