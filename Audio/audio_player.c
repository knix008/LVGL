/**
 * @file audio_player.c
 * @brief Audio playback backend implementation using SDL2_mixer
 */

#include "audio_player.h"
#include "audio_gui.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/stat.h>

// Audio player state
static struct {
    Mix_Music *music;
    audio_state_t state;
    uint8_t volume;
    uint32_t duration_ms;
    char current_file[512];
    audio_metadata_t metadata;
    audio_position_cb_t position_callback;
    audio_state_cb_t state_callback;
    pthread_t update_thread;
    bool thread_running;
} player = {
    .music = NULL,
    .state = AUDIO_STATE_STOPPED,
    .volume = 70,
    .duration_ms = 0,
    .current_file = "",
    .position_callback = NULL,
    .state_callback = NULL,
    .thread_running = false
};

// Position update thread
static void *position_update_thread(void *arg)
{
    (void)arg;
    static uint32_t play_start_time = 0;
    static uint32_t pause_offset = 0;
    
    while (player.thread_running) {
        if (player.state == AUDIO_STATE_PLAYING && player.music != NULL) {
            uint32_t position_ms = 0;
            
            // Try to get position from SDL_mixer (works for OGG, MP3 with some formats)
            double pos_sec = Mix_GetMusicPosition(player.music);
            
            if (pos_sec >= 0 && pos_sec < (player.duration_ms / 1000.0)) {
                // SDL_mixer returned valid position
                position_ms = (uint32_t)(pos_sec * 1000.0);
            } else {
                // Fallback: Calculate position based on time elapsed
                if (play_start_time == 0) {
                    play_start_time = SDL_GetTicks();
                }
                uint32_t elapsed = SDL_GetTicks() - play_start_time;
                position_ms = elapsed + pause_offset;
                
                // Cap at duration
                if (position_ms > player.duration_ms) {
                    position_ms = player.duration_ms;
                }
            }
            
            // Update GUI
            if (player.position_callback) {
                player.position_callback(position_ms, player.duration_ms);
            }
            audio_gui_update_position(position_ms, player.duration_ms);
        } else if (player.state == AUDIO_STATE_PAUSED) {
            // Store current position when paused
            if (play_start_time > 0) {
                pause_offset += SDL_GetTicks() - play_start_time;
                play_start_time = 0;
            }
        } else {
            // Reset when stopped
            play_start_time = 0;
            pause_offset = 0;
        }
        
        usleep(100000);  // Update every 100ms
    }
    
    return NULL;
}

// Helper function to estimate WAV file duration
static uint32_t estimate_wav_duration(const char *filepath)
{
    FILE *fp = fopen(filepath, "rb");
    if (!fp) {
        return 180000;  // Default 3 minutes
    }
    
    // Read WAV header to get duration
    uint8_t header[44];
    if (fread(header, 1, 44, fp) != 44) {
        fclose(fp);
        return 180000;
    }
    
    // Check if it's a valid WAV file (RIFF header)
    if (header[0] != 'R' || header[1] != 'I' || header[2] != 'F' || header[3] != 'F') {
        fclose(fp);
        return 180000;
    }
    
    // Get data size (bytes 40-43)
    uint32_t data_size = header[40] | (header[41] << 8) | (header[42] << 16) | (header[43] << 24);
    
    // Get byte rate (bytes 28-31) - more accurate for duration calculation
    uint32_t byte_rate = header[28] | (header[29] << 8) | (header[30] << 16) | (header[31] << 24);
    
    fclose(fp);
    
    if (byte_rate > 0) {
        // Duration = data_size / byte_rate (in seconds)
        uint32_t duration_ms = (uint32_t)((data_size * 1000ULL) / byte_rate);
        printf("WAV duration detected: %u ms (%.1f seconds)\n", duration_ms, duration_ms / 1000.0);
        return duration_ms;
    }
    
    return 180000;  // Default if calculation fails
}

// Music finished callback
static void music_finished_callback(void)
{
    player.state = AUDIO_STATE_STOPPED;
    audio_gui_update_state(false);
    
    if (player.state_callback) {
        player.state_callback(player.state);
    }
}

bool audio_player_init(void)
{
    // Initialize SDL Audio
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL Audio initialization failed: %s\n", SDL_GetError());
        return false;
    }
    
    // Initialize SDL_mixer
    int flags = MIX_INIT_MP3;
    if ((Mix_Init(flags) & flags) != flags) {
        fprintf(stderr, "SDL_mixer initialization failed: %s\n", Mix_GetError());
        SDL_Quit();
        return false;
    }
    
    // Open audio device
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "SDL_mixer open audio failed: %s\n", Mix_GetError());
        Mix_Quit();
        SDL_Quit();
        return false;
    }
    
    // Set up music finished callback
    Mix_HookMusicFinished(music_finished_callback);
    
    // Set initial volume
    Mix_VolumeMusic(MIX_MAX_VOLUME * player.volume / 100);
    
    // Start position update thread
    player.thread_running = true;
    if (pthread_create(&player.update_thread, NULL, position_update_thread, NULL) != 0) {
        fprintf(stderr, "Failed to create update thread\n");
        Mix_CloseAudio();
        Mix_Quit();
        SDL_Quit();
        return false;
    }
    
    printf("Audio player initialized successfully\n");
    printf("SDL_mixer version: %d.%d.%d\n", 
           MIX_MAJOR_VERSION, MIX_MINOR_VERSION, MIX_PATCHLEVEL);
    
    return true;
}

void audio_player_cleanup(void)
{
    // Stop update thread
    player.thread_running = false;
    if (player.update_thread) {
        pthread_join(player.update_thread, NULL);
    }
    
    // Stop and free music
    if (player.music) {
        Mix_HaltMusic();
        Mix_FreeMusic(player.music);
        player.music = NULL;
    }
    
    // Close audio
    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();
    
    printf("Audio player cleaned up\n");
}

bool audio_player_load(const char *filepath)
{
    if (!filepath) {
        return false;
    }
    
    // Stop current playback
    if (player.music) {
        Mix_HaltMusic();
        Mix_FreeMusic(player.music);
        player.music = NULL;
    }
    
    // Load new music
    player.music = Mix_LoadMUS(filepath);
    if (!player.music) {
        fprintf(stderr, "Failed to load music '%s': %s\n", filepath, Mix_GetError());
        return false;
    }
    
    // Store filepath
    strncpy(player.current_file, filepath, sizeof(player.current_file) - 1);
    player.current_file[sizeof(player.current_file) - 1] = '\0';
    
    // Get duration - try to calculate from WAV header or use default
    const char *ext = strrchr(filepath, '.');
    if (ext && (strcasecmp(ext, ".wav") == 0)) {
        player.duration_ms = estimate_wav_duration(filepath);
    } else {
        // For MP3 and other formats, use a default or estimate from file size
        struct stat st;
        if (stat(filepath, &st) == 0) {
            // Rough estimate: assume 128kbps bitrate for MP3
            // Duration (seconds) ≈ file_size_bytes / (bitrate_kbps * 125)
            uint32_t estimated_duration = (uint32_t)((st.st_size * 8) / (128 * 1000));
            player.duration_ms = estimated_duration * 1000;
            printf("Estimated MP3 duration: %u ms (%.1f seconds)\n", 
                   player.duration_ms, player.duration_ms / 1000.0);
        } else {
            player.duration_ms = 180000;  // Default 3 minutes
        }
    }
    
    // Extract metadata from filename (basic implementation)
    const char *filename = strrchr(filepath, '/');
    if (filename) {
        filename++;  // Skip the '/'
    } else {
        filename = filepath;
    }
    
    // Copy filename as title (remove extension)
    strncpy(player.metadata.title, filename, sizeof(player.metadata.title) - 1);
    char *title_ext = strrchr(player.metadata.title, '.');
    if (title_ext) {
        *title_ext = '\0';
    }
    
    // Set default metadata
    strcpy(player.metadata.artist, "Unknown Artist");
    strcpy(player.metadata.album, "Unknown Album");
    player.metadata.duration_ms = player.duration_ms;
    player.metadata.sample_rate = 44100;
    player.metadata.channels = 2;
    
    player.state = AUDIO_STATE_STOPPED;
    
    printf("Loaded audio file: %s\n", filepath);
    
    return true;
}

void audio_player_play(void)
{
    if (!player.music) {
        fprintf(stderr, "No music loaded\n");
        return;
    }
    
    if (player.state == AUDIO_STATE_PAUSED) {
        // Resume playback
        Mix_ResumeMusic();
    } else {
        // Start playback
        if (Mix_PlayMusic(player.music, 0) < 0) {
            fprintf(stderr, "Failed to play music: %s\n", Mix_GetError());
            return;
        }
    }
    
    player.state = AUDIO_STATE_PLAYING;
    audio_gui_update_state(true);
    
    if (player.state_callback) {
        player.state_callback(player.state);
    }
    
    printf("Playback started\n");
}

void audio_player_pause(void)
{
    if (player.state != AUDIO_STATE_PLAYING) {
        return;
    }
    
    Mix_PauseMusic();
    player.state = AUDIO_STATE_PAUSED;
    audio_gui_update_state(false);
    
    if (player.state_callback) {
        player.state_callback(player.state);
    }
    
    printf("Playback paused\n");
}

void audio_player_stop(void)
{
    Mix_HaltMusic();
    player.state = AUDIO_STATE_STOPPED;
    audio_gui_update_state(false);
    
    // Reset position
    audio_gui_update_position(0, player.duration_ms);
    
    if (player.state_callback) {
        player.state_callback(player.state);
    }
    
    printf("Playback stopped\n");
}

void audio_player_seek(uint32_t position_ms)
{
    if (!player.music) {
        return;
    }
    
    // SDL_mixer's seeking support is limited
    // Mix_SetMusicPosition works for OGG and some MP3 formats
    double position_sec = position_ms / 1000.0;
    
    bool was_playing = (player.state == AUDIO_STATE_PLAYING);
    
    // Try SDL_mixer's built-in seeking first
    if (Mix_SetMusicPosition(position_sec) == 0) {
        printf("Seeked to %u ms using Mix_SetMusicPosition\n", position_ms);
        return;
    }
    
    // Fallback: For WAV files and formats that don't support seeking,
    // we need to restart playback from the beginning
    // This is a limitation of SDL_mixer with WAV files
    printf("Direct seek not supported, using restart method\n");
    
    // Stop current playback
    Mix_HaltMusic();
    
    // Reload and restart the music
    Mix_FreeMusic(player.music);
    player.music = Mix_LoadMUS(player.current_file);
    
    if (player.music && was_playing) {
        // Restart playback
        Mix_PlayMusic(player.music, 0);
        player.state = AUDIO_STATE_PLAYING;
        
        // Note: We can't actually seek in WAV files with SDL_mixer
        // The best we can do is restart from beginning
        // For proper WAV seeking, would need a different library
        printf("Note: WAV seeking limited - restarted from beginning\n");
        printf("Target position was %u ms\n", position_ms);
    }
}

void audio_player_set_volume(uint8_t volume)
{
    if (volume > 100) {
        volume = 100;
    }
    
    player.volume = volume;
    
    // Only call Mix_VolumeMusic if SDL_mixer is initialized
    // This prevents crashes if volume is set before audio_player_init()
    if (Mix_QuerySpec(NULL, NULL, NULL)) {
        Mix_VolumeMusic(MIX_MAX_VOLUME * volume / 100);
    }
}

uint8_t audio_player_get_volume(void)
{
    return player.volume;
}

audio_state_t audio_player_get_state(void)
{
    return player.state;
}

uint32_t audio_player_get_position(void)
{
    if (!player.music || player.state != AUDIO_STATE_PLAYING) {
        return 0;
    }
    
    double pos = Mix_GetMusicPosition(player.music);
    if (pos < 0) {
        return 0;
    }
    
    return (uint32_t)(pos * 1000.0);
}

bool audio_player_get_metadata(audio_metadata_t *metadata)
{
    if (!metadata || !player.music) {
        return false;
    }
    
    memcpy(metadata, &player.metadata, sizeof(audio_metadata_t));
    return true;
}

void audio_player_set_position_callback(audio_position_cb_t callback)
{
    player.position_callback = callback;
}

void audio_player_set_state_callback(audio_state_cb_t callback)
{
    player.state_callback = callback;
}

