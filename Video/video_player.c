#include "video_player.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>

// Video player state variables
static player_state_t current_state = PLAYER_STOPPED;
static char current_file[256] = {0};
static int current_position = 0;
static int total_duration = 0;
static int volume_level = 50;
static time_t last_update_time = 0;

// LVGL video player object
static lv_obj_t *video_player_obj = NULL;

// Audio context for direct FFmpeg audio output
static AVFormatContext *audio_format_ctx = NULL;
static AVCodecContext *audio_codec_ctx = NULL;
static SwrContext *swr_ctx = NULL;
static int audio_stream_index = -1;
static SDL_AudioDeviceID audio_device = 0;
static uint8_t *audio_buffer = NULL;
static int audio_buffer_size = 0;
static int audio_buffer_pos = 0;
static bool audio_playing = false;
static bool audio_needs_reset = false;

// Audio callback function for SDL
void audio_callback(void *userdata, Uint8 *stream, int len) {
    (void)userdata; // Suppress unused parameter warning
    
    if (!audio_playing || !audio_format_ctx) {
        memset(stream, 0, len);
        return;
    }
    
    // Reset audio stream position if needed (only when coming from stopped state)
    if (audio_needs_reset) {
        av_seek_frame(audio_format_ctx, audio_stream_index, 0, AVSEEK_FLAG_BACKWARD);
        audio_needs_reset = false;
        printf("Audio stream reset to beginning\n");
    }
    
    // Decode audio from video file
    AVPacket packet;
    AVFrame *frame = av_frame_alloc();
    int ret;
    
    while (av_read_frame(audio_format_ctx, &packet) >= 0) {
        if (packet.stream_index == audio_stream_index) {
            ret = avcodec_send_packet(audio_codec_ctx, &packet);
            if (ret < 0) {
                av_packet_unref(&packet);
                continue;
            }
            
            ret = avcodec_receive_frame(audio_codec_ctx, frame);
            if (ret == 0) {
                // Convert audio to SDL format
                int16_t *samples = (int16_t*)stream;
                int num_samples = len / 4; // 16-bit stereo = 4 bytes per sample
                
                // Proper audio conversion based on format
                if (frame->format == AV_SAMPLE_FMT_FLTP) {
                    // Float planar format (AAC)
                    float *left = (float*)frame->data[0];
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
                    float *right = (frame->channels > 1) ? (float*)frame->data[1] : left;
#pragma GCC diagnostic pop
                    
                    for (int i = 0; i < num_samples && i < frame->nb_samples; i++) {
                        int16_t left_sample = (int16_t)(left[i] * 32767.0f * volume_level / 100.0f);
                        int16_t right_sample = (int16_t)(right[i] * 32767.0f * volume_level / 100.0f);
                        
                        samples[i * 2] = left_sample;     // Left channel
                        samples[i * 2 + 1] = right_sample; // Right channel
                    }
                } else if (frame->format == AV_SAMPLE_FMT_S16P) {
                    // 16-bit planar format
                    int16_t *left = (int16_t*)frame->data[0];
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
                    int16_t *right = (frame->channels > 1) ? (int16_t*)frame->data[1] : left;
#pragma GCC diagnostic pop
                    
                    for (int i = 0; i < num_samples && i < frame->nb_samples; i++) {
                        int16_t left_sample = (int16_t)(left[i] * volume_level / 100);
                        int16_t right_sample = (int16_t)(right[i] * volume_level / 100);
                        
                        samples[i * 2] = left_sample;     // Left channel
                        samples[i * 2 + 1] = right_sample; // Right channel
                    }
                } else {
                    // Fallback: convert to silence
                    memset(stream, 0, len);
                }
                
                av_frame_unref(frame);
                av_packet_unref(&packet);
                return;
            }
        }
        av_packet_unref(&packet);
    }
    
    av_frame_free(&frame);
    memset(stream, 0, len);
}

// Initialize video player
void video_player_init(void) {
    printf("Video player initialized\n");
    current_state = PLAYER_STOPPED;
    current_file[0] = '\0';
    current_position = 0;
    total_duration = 0;
    volume_level = 50;
    last_update_time = time(NULL);
    video_player_obj = NULL;
    
    // Initialize FFmpeg
    av_log_set_level(AV_LOG_ERROR);
}

// Initialize audio for a file
static int init_audio(const char *filename) {
    // Open input file
    if (avformat_open_input(&audio_format_ctx, filename, NULL, NULL) < 0) {
        printf("Error: Could not open audio file: %s\n", filename);
        return -1;
    }
    
    if (avformat_find_stream_info(audio_format_ctx, NULL) < 0) {
        printf("Error: Could not find stream information\n");
        return -1;
    }
    
    // Find audio stream
    audio_stream_index = av_find_best_stream(audio_format_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (audio_stream_index < 0) {
        printf("Error: Could not find audio stream\n");
        return -1;
    }
    
    printf("Audio stream found at index %d\n", audio_stream_index);
    
    // Get audio codec
    const AVCodec *audio_codec = avcodec_find_decoder(audio_format_ctx->streams[audio_stream_index]->codecpar->codec_id);
    if (!audio_codec) {
        printf("Error: Could not find audio codec\n");
        return -1;
    }
    
    // Initialize audio codec context
    audio_codec_ctx = avcodec_alloc_context3(audio_codec);
    if (!audio_codec_ctx) {
        printf("Error: Could not allocate audio codec context\n");
        return -1;
    }
    
    if (avcodec_parameters_to_context(audio_codec_ctx, audio_format_ctx->streams[audio_stream_index]->codecpar) < 0) {
        printf("Error: Could not copy audio codec parameters\n");
        return -1;
    }
    
    if (avcodec_open2(audio_codec_ctx, audio_codec, NULL) < 0) {
        printf("Error: Could not open audio codec\n");
        return -1;
    }
    
    printf("Audio codec: %s\n", audio_codec->name);
    
    // Initialize SDL audio
    SDL_AudioSpec desired, obtained;
    desired.freq = 44100;
    desired.format = AUDIO_S16SYS;
    desired.channels = 2;
    desired.samples = 1024;
    desired.callback = audio_callback;
    desired.userdata = NULL;
    
    audio_device = SDL_OpenAudioDevice(NULL, 0, &desired, &obtained, 0);
    if (audio_device == 0) {
        printf("Error: Could not open SDL audio device\n");
        return -1;
    }
    
    printf("Audio initialized: %dHz, %d channels\n", obtained.freq, obtained.channels);
    return 0;
}

// Cleanup audio
static void cleanup_audio(void) {
    if (audio_device) {
        SDL_CloseAudioDevice(audio_device);
        audio_device = 0;
    }
    
    if (audio_buffer) {
        free(audio_buffer);
        audio_buffer = NULL;
    }
    
    if (swr_ctx) {
        swr_free(&swr_ctx);
    }
    
    if (audio_codec_ctx) {
        avcodec_free_context(&audio_codec_ctx);
    }
    
    if (audio_format_ctx) {
        avformat_close_input(&audio_format_ctx);
    }
    
    audio_buffer_size = 0;
    audio_buffer_pos = 0;
    audio_playing = false;
}

// Cleanup video player
void video_player_cleanup(void) {
    printf("Video player cleanup\n");
    video_player_stop();
    cleanup_audio();
    video_player_obj = NULL;
}

// Set the LVGL video player object (called from GUI)
void video_player_set_object(lv_obj_t *player) {
    video_player_obj = player;
}

// Get the LVGL video player object
lv_obj_t *video_player_get_object(void) {
    return video_player_obj;
}

// Load a video file
void video_player_load_file(const char *filename) {
    if (!filename) {
        printf("Error: No filename provided\n");
        return;
    }
    
    if (!video_player_obj) {
        printf("Error: Video player object not set\n");
        return;
    }
    
    // Stop current playback
    video_player_stop();
    
    // Cleanup previous audio
    cleanup_audio();

    // Copy filename
    strncpy(current_file, filename, sizeof(current_file) - 1);
    current_file[sizeof(current_file) - 1] = '\0';

    // Reset position
    current_position = 0;

    printf("Loading video file: %s\n", filename);
    
    // Initialize audio for the file
    char audio_path[512];
    snprintf(audio_path, sizeof(audio_path), "/home/shkwon/Projects/LVGL/Video/video/%s", filename);
    if (init_audio(audio_path) < 0) {
        printf("Warning: Could not initialize audio for %s\n", filename);
    } else {
        // Set reset flag for new file
        audio_needs_reset = true;
    }
    
    // Try different path formats for LVGL FFmpeg player
    char full_path[512];
    lv_result_t result = LV_RESULT_INVALID;
    
    // Try 1: LVGL file system path
    snprintf(full_path, sizeof(full_path), "A:video/%s", filename);
    printf("Trying LVGL path: %s\n", full_path);
    result = lv_ffmpeg_player_set_src(video_player_obj, full_path);
    if (result == LV_RESULT_OK) {
        printf("Video loaded successfully with LVGL path: %s\n", full_path);
        total_duration = 120;
        return;
    }
    
    // Try 2: Absolute path
    snprintf(full_path, sizeof(full_path), "/home/shkwon/Projects/LVGL/Video/video/%s", filename);
    printf("Trying absolute path: %s\n", full_path);
    result = lv_ffmpeg_player_set_src(video_player_obj, full_path);
    if (result == LV_RESULT_OK) {
        printf("Video loaded successfully with absolute path: %s\n", full_path);
        total_duration = 120;
        return;
    }
    
    // Try 3: Relative path
    snprintf(full_path, sizeof(full_path), "video/%s", filename);
    printf("Trying relative path: %s\n", full_path);
    result = lv_ffmpeg_player_set_src(video_player_obj, full_path);
    if (result == LV_RESULT_OK) {
        printf("Video loaded successfully with relative path: %s\n", full_path);
        total_duration = 120;
        return;
    }
    
    // Try 4: Just filename
    printf("Trying filename only: %s\n", filename);
    result = lv_ffmpeg_player_set_src(video_player_obj, filename);
    if (result == LV_RESULT_OK) {
        printf("Video loaded successfully with filename: %s\n", filename);
        total_duration = 120;
        return;
    }
    
    printf("Error: Could not load video file with any path format (result: %d)\n", result);
    current_file[0] = '\0';
    total_duration = 0;
}

// Play video
void video_player_play(void) {
    if (current_file[0] == '\0') {
        printf("No file loaded\n");
        return;
    }

    if (!video_player_obj) {
        printf("Error: Video player object not set\n");
        return;
    }
    
    // Determine if we need to reset audio stream
    bool needs_audio_reset = (current_state == PLAYER_STOPPED);
    
    if (current_state == PLAYER_PAUSED) {
        // Resume from pause - do NOT reset audio stream
        current_state = PLAYER_PLAYING;
        printf("Resuming playback from pause\n");
    } else if (current_state == PLAYER_STOPPED) {
        // Start from beginning - reset audio stream
        current_state = PLAYER_PLAYING;
        current_position = 0;
        printf("Starting playback from beginning\n");
    } else {
        // Already playing, do nothing
        printf("Already playing\n");
        return;
    }
    
    // Use LVGL FFmpeg player command for video
    lv_ffmpeg_player_set_cmd(video_player_obj, LV_FFMPEG_PLAYER_CMD_START);
    
    // Handle audio playback
    if (audio_device) {
        audio_playing = true;
        if (needs_audio_reset) {
            audio_needs_reset = true; // Reset audio stream position
            printf("Audio will reset to beginning\n");
        } else {
            printf("Audio will continue from current position\n");
        }
        SDL_PauseAudioDevice(audio_device, 0);
        printf("Audio playback started\n");
    }
    
    last_update_time = time(NULL);
}

// Pause video
void video_player_pause(void) {
    if (current_state != PLAYER_PLAYING) {
        printf("Cannot pause - not currently playing\n");
        return;
    }
    
    current_state = PLAYER_PAUSED;
    printf("Playback paused\n");
    
    // Pause video
    if (video_player_obj) {
        lv_ffmpeg_player_set_cmd(video_player_obj, LV_FFMPEG_PLAYER_CMD_PAUSE);
    }
    
    // Pause audio (do NOT reset position)
    if (audio_device) {
        audio_playing = false;
        SDL_PauseAudioDevice(audio_device, 1);
        printf("Audio paused (position preserved)\n");
    }
}

// Stop video
void video_player_stop(void) {
    if (current_state == PLAYER_STOPPED) {
        printf("Already stopped\n");
        return;
    }
    
    current_state = PLAYER_STOPPED;
    current_position = 0;
    printf("Playback stopped\n");
    
    // Stop video
    if (video_player_obj) {
        lv_ffmpeg_player_set_cmd(video_player_obj, LV_FFMPEG_PLAYER_CMD_STOP);
    }
    
    // Stop audio and reset position for next play
    if (audio_device) {
        audio_playing = false;
        audio_needs_reset = true; // Reset audio stream position for next play
        SDL_PauseAudioDevice(audio_device, 1);
        printf("Audio stopped (will reset on next play)\n");
    }
}

// Seek to position
void video_player_seek(int position) {
    if (position < 0) position = 0;
    if (position > total_duration) position = total_duration;
    
    current_position = position;
    printf("Seeked to position: %d seconds\n", position);
}

// Set volume
void video_player_set_volume(int volume) {
    if (volume < 0) volume = 0;
    if (volume > 100) volume = 100;
    
    volume_level = volume;
    printf("Volume set to: %d%%\n", volume);
}

// Get video frame for rendering (not used in current implementation)
bool video_player_get_frame(uint8_t **frame_data, int *width, int *height) {
    (void)frame_data; // Suppress unused parameter warning
    (void)width;      // Suppress unused parameter warning
    (void)height;     // Suppress unused parameter warning
    return false;
}

// Release frame after rendering (not used in current implementation)
void video_player_release_frame(void) {
    // No-op in current implementation
}

// Update video player (called periodically)
void video_player_update(void) {
    if (current_state == PLAYER_PLAYING) {
        // Simple time-based simulation for demo
        time_t current_time = time(NULL);
        int time_diff = (int)(current_time - last_update_time);
        
        if (time_diff > 0) {
            current_position += time_diff;
            last_update_time = current_time;
            
            if (current_position >= total_duration) {
                current_position = total_duration;
                current_state = PLAYER_STOPPED;
                printf("Playback completed\n");
            }
        }
    }
}

// Get current state
player_state_t video_player_get_state(void) {
    return current_state;
}

// Get current file
const char* video_player_get_current_file(void) {
    return current_file;
}

// Get current position
int video_player_get_position(void) {
    return current_position;
}

// Get total duration
int video_player_get_duration(void) {
    return total_duration;
}

// Get volume level
int video_player_get_volume(void) {
    return volume_level;
}