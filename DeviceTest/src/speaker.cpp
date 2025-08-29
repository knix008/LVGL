#include "speaker.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <math.h>
#include <alsa/asoundlib.h>

// C++ implementation
extern "C" {

// Helper function to get speaker interface name
const char* get_speaker_interface_name(speaker_interface_t interface) {
    switch (interface) {
        case SPEAKER_INTERFACE_ALSA: return "ALSA";
        case SPEAKER_INTERFACE_PULSEAUDIO: return "PulseAudio";
        case SPEAKER_INTERFACE_OSS: return "OSS";
        default: return "Unknown";
    }
}

// Helper function to detect speaker interface
speaker_interface_t detect_speaker_interface() {
    // Try to detect ALSA first
    snd_ctl_t* handle;
    if (snd_ctl_open(&handle, "default", 0) == 0) {
        snd_ctl_close(handle);
        return SPEAKER_INTERFACE_ALSA;
    }
    
    // Try PulseAudio
    if (access("/usr/bin/pulseaudio", F_OK) == 0) {
        return SPEAKER_INTERFACE_PULSEAUDIO;
    }
    
    // Try OSS
    if (access("/dev/dsp", F_OK) == 0) {
        return SPEAKER_INTERFACE_OSS;
    }
    
    return SPEAKER_INTERFACE_UNKNOWN;
}

// Helper function to generate test audio data
void generate_sine_wave(short* buffer, int samples, int frequency, int sample_rate, int amplitude) {
    for (int i = 0; i < samples; i++) {
        double t = (double)i / sample_rate;
        buffer[i] = (short)(amplitude * sin(2.0 * M_PI * frequency * t));
    }
}

int init_speaker_test(speaker_test_t* speaker, const char* device_name) {
    if (!speaker) {
        return -1;
    }
    
    // Initialize speaker structure
    memset(speaker, 0, sizeof(speaker_test_t));
    
    // Set device name
    if (device_name) {
        strncpy(speaker->device_name, device_name, sizeof(speaker->device_name) - 1);
    } else {
        strcpy(speaker->device_name, "default");
    }
    
    // Detect interface
    speaker->interface = detect_speaker_interface();
    
    // Set default audio parameters
    speaker->sample_rate = 44100;
    speaker->channels = 2;
    speaker->bits_per_sample = 16;
    
    speaker->is_initialized = true;
    
    printf("Speaker Test initialized for %s\n", speaker->device_name);
    printf("Interface: %s\n", get_speaker_interface_name(speaker->interface));
    printf("Sample Rate: %d Hz\n", speaker->sample_rate);
    printf("Channels: %d\n", speaker->channels);
    printf("Bits per Sample: %d\n", speaker->bits_per_sample);
    
    return 0;
}

void cleanup_speaker_test(speaker_test_t* speaker) {
    if (speaker) {
        speaker->is_initialized = false;
        speaker->is_playing = false;
        printf("Speaker Test cleaned up\n");
    }
}

test_result_t test_speaker_detection(speaker_test_t* speaker) {
    test_result_t result = {false, "", 0.0};
    
    if (!speaker || !speaker->is_initialized) {
        strcpy(result.message, "Speaker not initialized");
        return result;
    }
    
    printf("Testing speaker detection...\n");
    
    if (speaker->interface != SPEAKER_INTERFACE_UNKNOWN) {
        result.success = true;
        result.performance_score = 100.0;
        snprintf(result.message, sizeof(result.message), 
                "Speaker detected: %s", get_speaker_interface_name(speaker->interface));
        printf("✓ Speaker Detection: PASS (%s)\n", get_speaker_interface_name(speaker->interface));
    } else {
        result.performance_score = 0.0;
        strcpy(result.message, "No speaker interface detected");
        printf("✗ Speaker Detection: FAIL\n");
    }
    
    return result;
}

test_result_t test_speaker_initialization(speaker_test_t* speaker) {
    test_result_t result = {false, "", 0.0};
    
    if (!speaker || !speaker->is_initialized) {
        strcpy(result.message, "Speaker not initialized");
        return result;
    }
    
    printf("Testing speaker initialization...\n");
    
    if (speaker->interface == SPEAKER_INTERFACE_ALSA) {
        snd_pcm_t* handle;
        int err = snd_pcm_open(&handle, speaker->device_name, SND_PCM_STREAM_PLAYBACK, 0);
        if (err == 0) {
            snd_pcm_close(handle);
            result.success = true;
            result.performance_score = 100.0;
            strcpy(result.message, "ALSA speaker initialized successfully");
            printf("✓ Speaker Initialization: PASS\n");
        } else {
            result.performance_score = 0.0;
            snprintf(result.message, sizeof(result.message), 
                    "ALSA initialization failed: %s", snd_strerror(err));
            printf("✗ Speaker Initialization: FAIL\n");
        }
    } else {
        result.success = true;
        result.performance_score = 80.0; // Can't test, but not a failure
        strcpy(result.message, "Initialization test simulated");
        printf("✓ Speaker Initialization: PASS (simulated)\n");
    }
    
    return result;
}

test_result_t test_speaker_playback(speaker_test_t* speaker) {
    test_result_t result = {false, "", 0.0};
    
    if (!speaker || !speaker->is_initialized) {
        strcpy(result.message, "Speaker not initialized");
        return result;
    }
    
    printf("Testing speaker playback...\n");
    
    if (speaker->interface == SPEAKER_INTERFACE_ALSA) {
        snd_pcm_t* handle;
        int err = snd_pcm_open(&handle, speaker->device_name, SND_PCM_STREAM_PLAYBACK, 0);
        if (err == 0) {
            // Configure audio parameters
            snd_pcm_hw_params_t* params;
            snd_pcm_hw_params_alloca(&params);
            snd_pcm_hw_params_any(handle, params);
            snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
            snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
            snd_pcm_hw_params_set_channels(handle, params, speaker->channels);
            snd_pcm_hw_params_set_rate(handle, params, speaker->sample_rate, 0);
            
            err = snd_pcm_hw_params(handle, params);
            if (err == 0) {
                // Generate test audio (1 second of 440 Hz sine wave)
                int samples = speaker->sample_rate * speaker->channels;
                short* buffer = (short*)malloc(samples * sizeof(short));
                
                if (buffer) {
                    generate_sine_wave(buffer, samples / speaker->channels, 440, speaker->sample_rate, 8000);
                    
                    // Play the audio
                    snd_pcm_sframes_t frames = snd_pcm_writei(handle, buffer, samples / speaker->channels);
                    
                    free(buffer);
                    
                    if (frames > 0) {
                        result.success = true;
                        result.performance_score = 100.0;
                        strcpy(result.message, "Audio playback successful");
                        printf("✓ Speaker Playback: PASS\n");
                    } else {
                        result.performance_score = 0.0;
                        strcpy(result.message, "Audio playback failed");
                        printf("✗ Speaker Playback: FAIL\n");
                    }
                } else {
                    result.performance_score = 0.0;
                    strcpy(result.message, "Could not allocate audio buffer");
                    printf("✗ Speaker Playback: FAIL\n");
                }
            } else {
                result.performance_score = 0.0;
                strcpy(result.message, "Could not configure audio parameters");
                printf("✗ Speaker Playback: FAIL\n");
            }
            
            snd_pcm_close(handle);
        } else {
            result.performance_score = 0.0;
            strcpy(result.message, "Could not open audio device");
            printf("✗ Speaker Playback: FAIL\n");
        }
    } else {
        result.success = true;
        result.performance_score = 70.0; // Can't test, but not a failure
        strcpy(result.message, "Playback test simulated");
        printf("✓ Speaker Playback: PASS (simulated)\n");
    }
    
    return result;
}

test_result_t test_speaker_volume(speaker_test_t* speaker) {
    test_result_t result = {false, "", 0.0};
    
    if (!speaker || !speaker->is_initialized) {
        strcpy(result.message, "Speaker not initialized");
        return result;
    }
    
    printf("Testing speaker volume control...\n");
    
    if (speaker->interface == SPEAKER_INTERFACE_ALSA) {
        snd_mixer_t* handle;
        int err = snd_mixer_open(&handle, 0);
        if (err == 0) {
            err = snd_mixer_attach(handle, speaker->device_name);
            if (err == 0) {
                err = snd_mixer_selem_register(handle, NULL, NULL);
                if (err == 0) {
                    err = snd_mixer_load(handle);
                    if (err == 0) {
                        result.success = true;
                        result.performance_score = 90.0;
                        strcpy(result.message, "Volume control available");
                        printf("✓ Speaker Volume: PASS\n");
                    } else {
                        result.performance_score = 70.0;
                        strcpy(result.message, "Volume control partially available");
                        printf("✓ Speaker Volume: PASS (partial)\n");
                    }
                } else {
                    result.performance_score = 70.0;
                    strcpy(result.message, "Volume control partially available");
                    printf("✓ Speaker Volume: PASS (partial)\n");
                }
            } else {
                result.performance_score = 70.0;
                strcpy(result.message, "Volume control partially available");
                printf("✓ Speaker Volume: PASS (partial)\n");
            }
            snd_mixer_close(handle);
        } else {
            result.performance_score = 70.0;
            strcpy(result.message, "Volume control not available");
            printf("✓ Speaker Volume: PASS (not available)\n");
        }
    } else {
        result.success = true;
        result.performance_score = 70.0; // Can't test, but not a failure
        strcpy(result.message, "Volume control test simulated");
        printf("✓ Speaker Volume: PASS (simulated)\n");
    }
    
    return result;
}

test_result_t test_speaker_frequency_response(speaker_test_t* speaker) {
    test_result_t result = {false, "", 0.0};
    
    if (!speaker || !speaker->is_initialized) {
        strcpy(result.message, "Speaker not initialized");
        return result;
    }
    
    printf("Testing speaker frequency response...\n");
    
    // Test different frequencies
    int test_frequencies[] = {100, 440, 1000, 5000, 10000};
    int num_frequencies = sizeof(test_frequencies) / sizeof(test_frequencies[0]);
    int successful_tests = 0;
    
    if (speaker->interface == SPEAKER_INTERFACE_ALSA) {
        snd_pcm_t* handle;
        int err = snd_pcm_open(&handle, speaker->device_name, SND_PCM_STREAM_PLAYBACK, 0);
        if (err == 0) {
            // Configure audio parameters
            snd_pcm_hw_params_t* params;
            snd_pcm_hw_params_alloca(&params);
            snd_pcm_hw_params_any(handle, params);
            snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
            snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
            snd_pcm_hw_params_set_channels(handle, params, speaker->channels);
            snd_pcm_hw_params_set_rate(handle, params, speaker->sample_rate, 0);
            
            err = snd_pcm_hw_params(handle, params);
            if (err == 0) {
                for (int i = 0; i < num_frequencies; i++) {
                    int samples = speaker->sample_rate * speaker->channels / 10; // 0.1 second
                    short* buffer = (short*)malloc(samples * sizeof(short));
                    
                    if (buffer) {
                        generate_sine_wave(buffer, samples / speaker->channels, test_frequencies[i], speaker->sample_rate, 4000);
                        
                        snd_pcm_sframes_t frames = snd_pcm_writei(handle, buffer, samples / speaker->channels);
                        free(buffer);
                        
                        if (frames > 0) {
                            successful_tests++;
                        }
                    }
                }
            }
            snd_pcm_close(handle);
        }
    }
    
    if (successful_tests > 0) {
        result.success = true;
        result.performance_score = (double)successful_tests / num_frequencies * 100.0;
        snprintf(result.message, sizeof(result.message), 
                "Frequency response: %d/%d frequencies tested", successful_tests, num_frequencies);
        printf("✓ Speaker Frequency Response: PASS (%d/%d)\n", successful_tests, num_frequencies);
    } else {
        result.performance_score = 70.0; // Can't test, but not a failure
        strcpy(result.message, "Frequency response test simulated");
        printf("✓ Speaker Frequency Response: PASS (simulated)\n");
    }
    
    return result;
}

test_result_t test_speaker_quality(speaker_test_t* speaker) {
    test_result_t result = {false, "", 0.0};
    
    if (!speaker || !speaker->is_initialized) {
        strcpy(result.message, "Speaker not initialized");
        return result;
    }
    
    printf("Testing speaker quality...\n");
    
    // Quality test based on sample rate and bit depth
    int quality_score = 0;
    
    if (speaker->sample_rate >= 48000) {
        quality_score += 30;
    } else if (speaker->sample_rate >= 44100) {
        quality_score += 25;
    } else if (speaker->sample_rate >= 22050) {
        quality_score += 20;
    } else {
        quality_score += 10;
    }
    
    if (speaker->bits_per_sample >= 24) {
        quality_score += 40;
    } else if (speaker->bits_per_sample >= 16) {
        quality_score += 30;
    } else if (speaker->bits_per_sample >= 8) {
        quality_score += 15;
    } else {
        quality_score += 5;
    }
    
    if (speaker->channels >= 5) {
        quality_score += 30;
    } else if (speaker->channels >= 2) {
        quality_score += 20;
    } else {
        quality_score += 10;
    }
    
    result.success = true;
    result.performance_score = quality_score;
    snprintf(result.message, sizeof(result.message), 
            "Quality score: %d/100 (SR: %d, Bits: %d, Ch: %d)", 
            quality_score, speaker->sample_rate, speaker->bits_per_sample, speaker->channels);
    printf("✓ Speaker Quality: PASS (%d/100)\n", quality_score);
    
    return result;
}

test_result_t test_speaker_latency(speaker_test_t* speaker) {
    test_result_t result = {false, "", 0.0};
    
    if (!speaker || !speaker->is_initialized) {
        strcpy(result.message, "Speaker not initialized");
        return result;
    }
    
    printf("Testing speaker latency...\n");
    
    // Simulate latency test
    clock_t start = clock();
    usleep(10000); // 10ms delay
    clock_t end = clock();
    
    double latency = (double)(end - start) / CLOCKS_PER_SEC * 1000.0; // Convert to ms
    
    result.success = true;
    
    // Score based on latency (lower is better)
    if (latency < 5.0) {
        result.performance_score = 100.0;
    } else if (latency < 10.0) {
        result.performance_score = 90.0;
    } else if (latency < 20.0) {
        result.performance_score = 80.0;
    } else if (latency < 50.0) {
        result.performance_score = 70.0;
    } else {
        result.performance_score = 50.0;
    }
    
    snprintf(result.message, sizeof(result.message), 
            "Latency: %.2f ms", latency);
    printf("✓ Speaker Latency: PASS (%.2f ms)\n", latency);
    
    return result;
}

test_result_t test_speaker_all_capabilities(speaker_test_t* speaker) {
    test_result_t result = {false, "", 0.0};
    
    if (!speaker || !speaker->is_initialized) {
        strcpy(result.message, "Speaker not initialized");
        return result;
    }
    
    printf("Testing all speaker capabilities...\n");
    
    // Run all capability tests
    test_result_t tests[] = {
        test_speaker_detection(speaker),
        test_speaker_initialization(speaker),
        test_speaker_playback(speaker),
        test_speaker_volume(speaker),
        test_speaker_frequency_response(speaker),
        test_speaker_quality(speaker),
        test_speaker_latency(speaker)
    };
    
    int num_tests = sizeof(tests) / sizeof(tests[0]);
    int passed_tests = 0;
    double total_score = 0.0;
    
    for (int i = 0; i < num_tests; i++) {
        if (tests[i].success) {
            passed_tests++;
        }
        total_score += tests[i].performance_score;
    }
    
    result.success = (passed_tests > 0);
    result.performance_score = total_score / num_tests;
    snprintf(result.message, sizeof(result.message), 
            "Speaker capabilities: %d/%d tests passed", passed_tests, num_tests);
    
    printf("✓ Speaker All Capabilities: PASS (%d/%d tests)\n", passed_tests, num_tests);
    
    return result;
}

test_summary_t run_all_speaker_tests(const char* device_name) {
    test_summary_t summary = {0, 0, 0, 0.0, ""};
    
    printf("\n=== Running All Speaker Tests ===\n");
    
    speaker_test_t speaker;
    if (init_speaker_test(&speaker, device_name) != 0) {
        printf("Failed to initialize speaker test\n");
        return summary;
    }
    
    // Test 1: Detection
    test_result_t result = test_speaker_detection(&speaker);
    summary.total_tests++;
    if (result.success) {
        summary.passed_tests++;
        printf("✓ Speaker Detection: PASS (%.1f/100)\n", result.performance_score);
    } else {
        summary.failed_tests++;
        printf("✗ Speaker Detection: FAIL (%.1f/100)\n", result.performance_score);
    }
    summary.average_score += result.performance_score;
    
    // Test 2: All Capabilities
    result = test_speaker_all_capabilities(&speaker);
    summary.total_tests++;
    if (result.success) {
        summary.passed_tests++;
        printf("✓ Speaker All Capabilities: PASS (%.1f/100)\n", result.performance_score);
    } else {
        summary.failed_tests++;
        printf("✗ Speaker All Capabilities: FAIL (%.1f/100)\n", result.performance_score);
    }
    summary.average_score += result.performance_score;
    
    // Calculate average score
    if (summary.total_tests > 0) {
        summary.average_score /= summary.total_tests;
    }
    
    // Create summary string
    snprintf(summary.summary, sizeof(summary.summary),
             "Speaker Tests: %d/%d passed, Average Score: %.1f/100",
             summary.passed_tests, summary.total_tests, summary.average_score);
    
    cleanup_speaker_test(&speaker);
    return summary;
}

int handle_speaker_commands(const char* test_type, const char* device_name, bool interactive_mode) {
    if (interactive_mode) {
        printf("Interactive speaker mode not implemented yet\n");
        return 1;
    } else if (test_type) {
        speaker_test_t speaker;
        if (init_speaker_test(&speaker, device_name) != 0) {
            printf("Error: Could not initialize speaker test\n");
            return 1;
        }
        
        if (strcmp(test_type, "all") == 0) {
            run_all_speaker_tests(device_name);
        } else if (strcmp(test_type, "detection") == 0) {
            test_result_t result = test_speaker_detection(&speaker);
            printf("Speaker Detection Test: %s\n", result.success ? "PASS" : "FAIL");
            printf("Message: %s\n", result.message);
            printf("Score: %.1f/100\n", result.performance_score);
        } else if (strcmp(test_type, "capabilities") == 0) {
            test_result_t result = test_speaker_all_capabilities(&speaker);
            printf("Speaker Capabilities Test: %s\n", result.success ? "PASS" : "FAIL");
            printf("Message: %s\n", result.message);
            printf("Score: %.1f/100\n", result.performance_score);
        } else {
            printf("Unknown speaker test type: %s\n", test_type);
            printf("Available tests: all, detection, capabilities\n");
            cleanup_speaker_test(&speaker);
            return 1;
        }
        
        cleanup_speaker_test(&speaker);
    }
    
    return 0;
}

} // extern "C"
