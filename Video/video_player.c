#include "video_player.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>

// Video player state variables
static player_state_t current_state = PLAYER_STOPPED;
static char current_file[256] = {0};
static int current_position = 0;
static int total_duration = 0;
static int volume_level = 50;
static time_t last_update_time = 0;

// LVGL video player object
static lv_obj_t *video_player_obj = NULL;

// Audio playback context
static AVFormatContext *audio_format_ctx = NULL;
static AVCodecContext *audio_codec_ctx = NULL;
static int audio_stream_index = -1;
static SDL_AudioDeviceID audio_device_id = 0;
static SwrContext *swr_ctx = NULL;
static uint8_t *audio_buf = NULL;
static unsigned int audio_buf_size = 0;
static unsigned int audio_buf_index = 0;
static AVPacket *audio_pkt = NULL;
static AVFrame *audio_frame = NULL;
static int audio_volume = 50;

// SDL audio callback
static void audio_callback(void *userdata, Uint8 *stream, int len) {
    (void)userdata;
    memset(stream, 0, len); // Start with silence

    if (!audio_format_ctx || audio_stream_index < 0 || current_state != PLAYER_PLAYING) {
        return;
    }

    while (len > 0) {
        if (audio_buf_index >= audio_buf_size) {
            // Need more audio data - decode next frame
            int ret = av_read_frame(audio_format_ctx, audio_pkt);
            if (ret < 0) {
                break; // End of file or error
            }

            if (audio_pkt->stream_index == audio_stream_index) {
                ret = avcodec_send_packet(audio_codec_ctx, audio_pkt);
                if (ret == 0) {
                    ret = avcodec_receive_frame(audio_codec_ctx, audio_frame);
                    if (ret == 0) {
                        // Resample audio to SDL format (stereo, 16-bit, 44100Hz)
                        int out_samples = swr_convert(swr_ctx,
                                                     &audio_buf, audio_frame->nb_samples,
                                                     (const uint8_t **)audio_frame->data, audio_frame->nb_samples);
                        if (out_samples > 0) {
                            audio_buf_size = out_samples * 4; // 2 channels * 2 bytes per sample
                            audio_buf_index = 0;
                        }
                    }
                }
            }
            av_packet_unref(audio_pkt);
        }

        if (audio_buf_index < audio_buf_size) {
            int remaining = audio_buf_size - audio_buf_index;
            int to_copy = (len < remaining) ? len : remaining;

            // Apply volume
            int16_t *samples = (int16_t *)(audio_buf + audio_buf_index);
            int16_t *out = (int16_t *)stream;
            for (int i = 0; i < to_copy / 2; i++) {
                out[i] = (int16_t)(samples[i] * audio_volume / 100);
            }

            stream += to_copy;
            len -= to_copy;
            audio_buf_index += to_copy;
        } else {
            break;
        }
    }
}

// Initialize video player
void video_player_init(void) {
    printf("Video player initialized\n");
    current_state = PLAYER_STOPPED;
    current_file[0] = '\0';
    current_position = 0;
    total_duration = 0;
    volume_level = 50;
    audio_volume = 50;
    last_update_time = time(NULL);
    video_player_obj = NULL;

    // Initialize FFmpeg
    av_log_set_level(AV_LOG_ERROR);

    // Allocate audio packet and frame
    audio_pkt = av_packet_alloc();
    audio_frame = av_frame_alloc();
}

// Cleanup audio resources
static void cleanup_audio(void) {
    if (audio_device_id) {
        SDL_PauseAudioDevice(audio_device_id, 1);
        SDL_CloseAudioDevice(audio_device_id);
        audio_device_id = 0;
    }

    if (audio_buf) {
        av_freep(&audio_buf);
        audio_buf = NULL;
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

    audio_stream_index = -1;
    audio_buf_size = 0;
    audio_buf_index = 0;
}

// Initialize audio for a video file
static int init_audio(const char *filepath) {
    // Open video file for audio
    if (avformat_open_input(&audio_format_ctx, filepath, NULL, NULL) < 0) {
        printf("Error: Could not open file for audio: %s\n", filepath);
        return -1;
    }

    if (avformat_find_stream_info(audio_format_ctx, NULL) < 0) {
        printf("Error: Could not find stream information\n");
        avformat_close_input(&audio_format_ctx);
        return -1;
    }

    // Find audio stream
    audio_stream_index = av_find_best_stream(audio_format_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (audio_stream_index < 0) {
        printf("Warning: No audio stream found in file\n");
        avformat_close_input(&audio_format_ctx);
        return -1;
    }

    // Get audio codec
    const AVCodec *audio_codec = avcodec_find_decoder(audio_format_ctx->streams[audio_stream_index]->codecpar->codec_id);
    if (!audio_codec) {
        printf("Error: Could not find audio codec\n");
        avformat_close_input(&audio_format_ctx);
        return -1;
    }

    // Create codec context
    audio_codec_ctx = avcodec_alloc_context3(audio_codec);
    if (!audio_codec_ctx) {
        printf("Error: Could not allocate audio codec context\n");
        avformat_close_input(&audio_format_ctx);
        return -1;
    }

    if (avcodec_parameters_to_context(audio_codec_ctx, audio_format_ctx->streams[audio_stream_index]->codecpar) < 0) {
        printf("Error: Could not copy codec parameters\n");
        avcodec_free_context(&audio_codec_ctx);
        avformat_close_input(&audio_format_ctx);
        return -1;
    }

    if (avcodec_open2(audio_codec_ctx, audio_codec, NULL) < 0) {
        printf("Error: Could not open audio codec\n");
        avcodec_free_context(&audio_codec_ctx);
        avformat_close_input(&audio_format_ctx);
        return -1;
    }

    // Setup resampler to convert to SDL audio format (stereo, 16-bit, 44100Hz)
    swr_ctx = swr_alloc();
    av_opt_set_int(swr_ctx, "in_channel_layout", audio_codec_ctx->channel_layout ? audio_codec_ctx->channel_layout : AV_CH_LAYOUT_STEREO, 0);
    av_opt_set_int(swr_ctx, "out_channel_layout", AV_CH_LAYOUT_STEREO, 0);
    av_opt_set_int(swr_ctx, "in_sample_rate", audio_codec_ctx->sample_rate, 0);
    av_opt_set_int(swr_ctx, "out_sample_rate", 44100, 0);
    av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", audio_codec_ctx->sample_fmt, 0);
    av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);
    swr_init(swr_ctx);

    // Allocate audio buffer
    audio_buf_size = 192000; // 1 second of 48khz stereo 16-bit audio
    audio_buf = (uint8_t *)av_malloc(audio_buf_size);
    audio_buf_index = audio_buf_size; // Start empty

    // Setup SDL audio
    SDL_AudioSpec wanted_spec, obtained_spec;
    wanted_spec.freq = 44100;
    wanted_spec.format = AUDIO_S16SYS;
    wanted_spec.channels = 2;
    wanted_spec.silence = 0;
    wanted_spec.samples = 1024;
    wanted_spec.callback = audio_callback;
    wanted_spec.userdata = NULL;

    audio_device_id = SDL_OpenAudioDevice(NULL, 0, &wanted_spec, &obtained_spec, 0);
    if (audio_device_id == 0) {
        printf("Error: Could not open SDL audio device: %s\n", SDL_GetError());
        cleanup_audio();
        return -1;
    }

    printf("Audio initialized: %s, %dHz, %d channels\n", audio_codec->name, obtained_spec.freq, obtained_spec.channels);
    return 0;
}

// Cleanup video player
void video_player_cleanup(void) {
    printf("Video player cleanup\n");
    video_player_stop();
    cleanup_audio();

    if (audio_pkt) {
        av_packet_free(&audio_pkt);
    }
    if (audio_frame) {
        av_frame_free(&audio_frame);
    }

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
        // Initialize audio separately
        init_audio(full_path + 2); // Skip "A:" prefix
        return;
    }

    // Try 2: Absolute path
    snprintf(full_path, sizeof(full_path), "/home/shkwon/Projects/LVGL/Video/video/%s", filename);
    printf("Trying absolute path: %s\n", full_path);
    result = lv_ffmpeg_player_set_src(video_player_obj, full_path);
    if (result == LV_RESULT_OK) {
        printf("Video loaded successfully with absolute path: %s\n", full_path);
        total_duration = 120;
        init_audio(full_path);
        return;
    }

    // Try 3: Relative path
    snprintf(full_path, sizeof(full_path), "video/%s", filename);
    printf("Trying relative path: %s\n", full_path);
    result = lv_ffmpeg_player_set_src(video_player_obj, full_path);
    if (result == LV_RESULT_OK) {
        printf("Video loaded successfully with relative path: %s\n", full_path);
        total_duration = 120;
        init_audio(full_path);
        return;
    }

    // Try 4: Just filename
    printf("Trying filename only: %s\n", filename);
    result = lv_ffmpeg_player_set_src(video_player_obj, filename);
    if (result == LV_RESULT_OK) {
        printf("Video loaded successfully with filename: %s\n", filename);
        total_duration = 120;
        init_audio(filename);
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

    if (current_state == PLAYER_PAUSED) {
        // Resume from pause
        current_state = PLAYER_PLAYING;
        printf("Resuming playback from pause\n");
    } else if (current_state == PLAYER_STOPPED) {
        // Start from beginning
        current_state = PLAYER_PLAYING;
        current_position = 0;
        // Reset audio to beginning
        if (audio_format_ctx && audio_stream_index >= 0) {
            av_seek_frame(audio_format_ctx, audio_stream_index, 0, AVSEEK_FLAG_BACKWARD);
            audio_buf_index = audio_buf_size; // Empty the buffer
        }
        printf("Starting playback from beginning\n");
    } else {
        // Already playing, do nothing
        printf("Already playing\n");
        return;
    }

    // Use LVGL FFmpeg player command for video
    lv_ffmpeg_player_set_cmd(video_player_obj, LV_FFMPEG_PLAYER_CMD_START);

    // Start audio playback
    if (audio_device_id) {
        SDL_PauseAudioDevice(audio_device_id, 0);
        printf("Audio playback started\n");
    }

    printf("Video playback started\n");
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
        printf("Video paused\n");
    }

    // Pause audio
    if (audio_device_id) {
        SDL_PauseAudioDevice(audio_device_id, 1);
        printf("Audio paused\n");
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
        printf("Video stopped\n");
    }

    // Stop audio
    if (audio_device_id) {
        SDL_PauseAudioDevice(audio_device_id, 1);
        printf("Audio stopped\n");
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
    audio_volume = volume;
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