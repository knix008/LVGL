#ifndef SPEAKER_H
#define SPEAKER_H

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

// Speaker interface types
typedef enum {
    SPEAKER_INTERFACE_ALSA,
    SPEAKER_INTERFACE_PULSEAUDIO,
    SPEAKER_INTERFACE_OSS,
    SPEAKER_INTERFACE_UNKNOWN
} speaker_interface_t;

// Speaker test structure
typedef struct {
    speaker_interface_t interface;
    char device_name[64];
    char card_name[64];
    int sample_rate;
    int channels;
    int bits_per_sample;
    bool is_initialized;
    bool is_playing;
} speaker_test_t;

// Function declarations
int init_speaker_test(speaker_test_t* speaker, const char* device_name);
void cleanup_speaker_test(speaker_test_t* speaker);

// Test functions
test_result_t test_speaker_detection(speaker_test_t* speaker);
test_result_t test_speaker_initialization(speaker_test_t* speaker);
test_result_t test_speaker_playback(speaker_test_t* speaker);
test_result_t test_speaker_volume(speaker_test_t* speaker);
test_result_t test_speaker_frequency_response(speaker_test_t* speaker);
test_result_t test_speaker_quality(speaker_test_t* speaker);
test_result_t test_speaker_latency(speaker_test_t* speaker);
test_result_t test_speaker_all_capabilities(speaker_test_t* speaker);

// Main test runner
test_summary_t run_all_speaker_tests(const char* device_name);

// Command handler
int handle_speaker_commands(const char* test_type, const char* device_name, bool interactive_mode);

#ifdef __cplusplus
}
#endif

#endif // SPEAKER_H
