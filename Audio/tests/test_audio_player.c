/**
 * @file test_audio_player.c
 * @brief Unit tests for audio player backend
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include "../audio_player.h"

// Test counter
static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

// Color codes for output
#define COLOR_GREEN "\033[0;32m"
#define COLOR_RED "\033[0;31m"
#define COLOR_YELLOW "\033[0;33m"
#define COLOR_BLUE "\033[0;34m"
#define COLOR_RESET "\033[0m"

// Test macros
#define TEST_START(name) \
    do { \
        tests_run++; \
        printf(COLOR_BLUE "TEST %d: %s" COLOR_RESET " ... ", tests_run, name); \
        fflush(stdout); \
    } while(0)

#define TEST_PASS() \
    do { \
        tests_passed++; \
        printf(COLOR_GREEN "PASS" COLOR_RESET "\n"); \
    } while(0)

#define TEST_FAIL(msg) \
    do { \
        tests_failed++; \
        printf(COLOR_RED "FAIL" COLOR_RESET " - %s\n", msg); \
    } while(0)

#define ASSERT(condition, msg) \
    do { \
        if (!(condition)) { \
            TEST_FAIL(msg); \
            return; \
        } \
    } while(0)

// Test fixtures
static void setup_test(void)
{
    // Initialize audio player
    audio_player_init();
}

static void teardown_test(void)
{
    // Cleanup
    audio_player_cleanup();
}

// Test cases
static void test_initialization(void)
{
    TEST_START("Audio player initialization");
    
    bool result = audio_player_init();
    ASSERT(result == true, "Failed to initialize audio player");
    
    audio_state_t state = audio_player_get_state();
    ASSERT(state == AUDIO_STATE_STOPPED, "Initial state should be STOPPED");
    
    audio_player_cleanup();
    TEST_PASS();
}

static void test_volume_control(void)
{
    TEST_START("Volume control");
    setup_test();
    
    // Test setting volume
    audio_player_set_volume(50);
    uint8_t volume = audio_player_get_volume();
    ASSERT(volume == 50, "Volume should be 50");
    
    // Test boundary values
    audio_player_set_volume(0);
    volume = audio_player_get_volume();
    ASSERT(volume == 0, "Volume should be 0");
    
    audio_player_set_volume(100);
    volume = audio_player_get_volume();
    ASSERT(volume == 100, "Volume should be 100");
    
    // Test overflow protection
    audio_player_set_volume(150);
    volume = audio_player_get_volume();
    ASSERT(volume == 100, "Volume should be capped at 100");
    
    teardown_test();
    TEST_PASS();
}

static void test_initial_state(void)
{
    TEST_START("Initial player state");
    setup_test();
    
    audio_state_t state = audio_player_get_state();
    ASSERT(state == AUDIO_STATE_STOPPED, "Initial state should be STOPPED");
    
    uint32_t position = audio_player_get_position();
    ASSERT(position == 0, "Initial position should be 0");
    
    teardown_test();
    TEST_PASS();
}

static void test_load_valid_file(void)
{
    TEST_START("Load valid audio file");
    setup_test();
    
    // Try to load a test file
    bool result = audio_player_load("../audio/test_440hz_A4.wav");
    
    if (result) {
        audio_state_t state = audio_player_get_state();
        ASSERT(state == AUDIO_STATE_STOPPED, "State should be STOPPED after load");
        
        audio_metadata_t metadata;
        bool has_metadata = audio_player_get_metadata(&metadata);
        ASSERT(has_metadata == true, "Should have metadata");
        ASSERT(strlen(metadata.title) > 0, "Title should not be empty");
    } else {
        printf(COLOR_YELLOW "SKIP" COLOR_RESET " - Test file not found\n");
        tests_run--;
        teardown_test();
        return;
    }
    
    teardown_test();
    TEST_PASS();
}

static void test_load_invalid_file(void)
{
    TEST_START("Load invalid audio file");
    setup_test();
    
    bool result = audio_player_load("nonexistent_file.mp3");
    ASSERT(result == false, "Should fail to load nonexistent file");
    
    audio_state_t state = audio_player_get_state();
    ASSERT(state == AUDIO_STATE_STOPPED, "State should remain STOPPED");
    
    teardown_test();
    TEST_PASS();
}

static void test_load_null_path(void)
{
    TEST_START("Load with NULL path");
    setup_test();
    
    bool result = audio_player_load(NULL);
    ASSERT(result == false, "Should fail with NULL path");
    
    teardown_test();
    TEST_PASS();
}

static void test_playback_without_file(void)
{
    TEST_START("Playback without loaded file");
    setup_test();
    
    // Try to play without loading a file
    audio_player_play();
    
    audio_state_t state = audio_player_get_state();
    ASSERT(state == AUDIO_STATE_STOPPED, "Should remain STOPPED without file");
    
    teardown_test();
    TEST_PASS();
}

static void test_metadata_extraction(void)
{
    TEST_START("Metadata extraction");
    setup_test();
    
    bool loaded = audio_player_load("../audio/chord_C_major.wav");
    if (!loaded) {
        printf(COLOR_YELLOW "SKIP" COLOR_RESET " - Test file not found\n");
        tests_run--;
        teardown_test();
        return;
    }
    
    audio_metadata_t metadata;
    bool has_metadata = audio_player_get_metadata(&metadata);
    ASSERT(has_metadata == true, "Should have metadata");
    
    ASSERT(strlen(metadata.title) > 0, "Title should not be empty");
    ASSERT(metadata.duration_ms > 0, "Duration should be > 0");
    ASSERT(metadata.sample_rate > 0, "Sample rate should be > 0");
    ASSERT(metadata.channels > 0, "Channels should be > 0");
    
    teardown_test();
    TEST_PASS();
}

static void test_state_transitions(void)
{
    TEST_START("State transitions");
    setup_test();
    
    bool loaded = audio_player_load("../audio/test_262hz_C4.wav");
    if (!loaded) {
        printf(COLOR_YELLOW "SKIP" COLOR_RESET " - Test file not found\n");
        tests_run--;
        teardown_test();
        return;
    }
    
    // STOPPED -> PLAYING
    audio_player_play();
    usleep(100000); // Wait 100ms
    audio_state_t state = audio_player_get_state();
    ASSERT(state == AUDIO_STATE_PLAYING, "State should be PLAYING");
    
    // PLAYING -> PAUSED
    audio_player_pause();
    state = audio_player_get_state();
    ASSERT(state == AUDIO_STATE_PAUSED, "State should be PAUSED");
    
    // PAUSED -> PLAYING
    audio_player_play();
    usleep(100000); // Wait 100ms
    state = audio_player_get_state();
    ASSERT(state == AUDIO_STATE_PLAYING, "State should be PLAYING again");
    
    // PLAYING -> STOPPED
    audio_player_stop();
    state = audio_player_get_state();
    ASSERT(state == AUDIO_STATE_STOPPED, "State should be STOPPED");
    
    teardown_test();
    TEST_PASS();
}

static void test_seek_functionality(void)
{
    TEST_START("Seek functionality");
    setup_test();
    
    bool loaded = audio_player_load("../audio/test_880hz_A5.wav");
    if (!loaded) {
        printf(COLOR_YELLOW "SKIP" COLOR_RESET " - Test file not found\n");
        tests_run--;
        teardown_test();
        return;
    }
    
    // Seek should not crash even if not supported
    audio_player_seek(1000); // Seek to 1 second
    audio_player_seek(0);    // Seek to start
    
    teardown_test();
    TEST_PASS();
}

static void test_multiple_loads(void)
{
    TEST_START("Multiple file loads");
    setup_test();
    
    // Load first file
    bool result1 = audio_player_load("../audio/test_440hz_A4.wav");
    if (!result1) {
        printf(COLOR_YELLOW "SKIP" COLOR_RESET " - Test files not found\n");
        tests_run--;
        teardown_test();
        return;
    }
    
    // Load second file (should replace first)
    bool result2 = audio_player_load("../audio/beep_1000hz.wav");
    ASSERT(result2 == true, "Should load second file");
    
    audio_metadata_t metadata;
    audio_player_get_metadata(&metadata);
    ASSERT(strstr(metadata.title, "beep") != NULL, "Should have second file loaded");
    
    teardown_test();
    TEST_PASS();
}

// Test suite runner
void run_all_tests(void)
{
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║          AUDIO PLAYER UNIT TEST SUITE                     ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    // Run all tests
    test_initialization();
    test_volume_control();
    test_initial_state();
    test_load_valid_file();
    test_load_invalid_file();
    test_load_null_path();
    test_playback_without_file();
    test_metadata_extraction();
    test_state_transitions();
    test_seek_functionality();
    test_multiple_loads();
    
    // Print summary
    printf("\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("TEST SUMMARY\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("Total tests:  %d\n", tests_run);
    printf(COLOR_GREEN "Passed:       %d" COLOR_RESET "\n", tests_passed);
    if (tests_failed > 0) {
        printf(COLOR_RED "Failed:       %d" COLOR_RESET "\n", tests_failed);
    } else {
        printf("Failed:       %d\n", tests_failed);
    }
    printf("Success rate: %.1f%%\n", tests_run > 0 ? (tests_passed * 100.0 / tests_run) : 0);
    printf("════════════════════════════════════════════════════════════\n");
    
    if (tests_failed > 0) {
        printf(COLOR_RED "\nSome tests FAILED!" COLOR_RESET "\n\n");
    } else {
        printf(COLOR_GREEN "\nAll tests PASSED!" COLOR_RESET "\n\n");
    }
}

int main(void)
{
    run_all_tests();
    
    return (tests_failed > 0) ? 1 : 0;
}

