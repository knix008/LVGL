#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>

static AVFormatContext *format_ctx = NULL;
static AVCodecContext *codec_ctx = NULL;
static SwrContext *swr_ctx = NULL;
static int audio_stream_index = -1;
static SDL_AudioDeviceID audio_device = 0;

void audio_callback(void *userdata, Uint8 *stream, int len) {
    // This is a placeholder - in a real implementation, you would
    // decode audio frames and fill the stream buffer
    memset(stream, 0, len);
}

int audio_player_init(const char *filename) {
    // Initialize FFmpeg
    av_log_set_level(AV_LOG_ERROR);
    
    // Open input file
    if (avformat_open_input(&format_ctx, filename, NULL, NULL) < 0) {
        printf("Error: Could not open audio file\n");
        return -1;
    }
    
    if (avformat_find_stream_info(format_ctx, NULL) < 0) {
        printf("Error: Could not find stream information\n");
        return -1;
    }
    
    // Find audio stream
    audio_stream_index = av_find_best_stream(format_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (audio_stream_index < 0) {
        printf("Error: Could not find audio stream\n");
        return -1;
    }
    
    printf("Audio stream found at index %d\n", audio_stream_index);
    
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
    
    printf("Audio player initialized: %dHz, %d channels\n", obtained.freq, obtained.channels);
    SDL_PauseAudioDevice(audio_device, 0);
    
    return 0;
}

void audio_player_cleanup(void) {
    if (audio_device) {
        SDL_CloseAudioDevice(audio_device);
        audio_device = 0;
    }
    
    if (swr_ctx) {
        swr_free(&swr_ctx);
    }
    
    if (codec_ctx) {
        avcodec_free_context(&codec_ctx);
    }
    
    if (format_ctx) {
        avformat_close_input(&format_ctx);
    }
}
