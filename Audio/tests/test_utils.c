/**
 * @file test_utils.c
 * @brief Unit tests for utility functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <dirent.h>
#include <sys/stat.h>

// Test counter
static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

// Color codes
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

// Helper function (duplicated from audio_gui.c for testing)
static bool is_audio_file(const char *filename)
{
    size_t len = strlen(filename);
    if (len < 5) return false;  // Minimum: "a.mp3" or "a.wav"
    
    const char *ext = filename + len - 4;
    return (strcasecmp(ext, ".mp3") == 0 || strcasecmp(ext, ".wav") == 0);
}

// Test cases
static void test_audio_file_detection_mp3(void)
{
    TEST_START("Audio file detection - MP3");
    
    ASSERT(is_audio_file("song.mp3") == true, "Should detect .mp3");
    ASSERT(is_audio_file("SONG.MP3") == true, "Should detect .MP3");
    ASSERT(is_audio_file("my.favorite.song.mp3") == true, "Should detect .mp3 with dots");
    
    TEST_PASS();
}

static void test_audio_file_detection_wav(void)
{
    TEST_START("Audio file detection - WAV");
    
    ASSERT(is_audio_file("sound.wav") == true, "Should detect .wav");
    ASSERT(is_audio_file("SOUND.WAV") == true, "Should detect .WAV");
    ASSERT(is_audio_file("test.file.wav") == true, "Should detect .wav with dots");
    
    TEST_PASS();
}

static void test_audio_file_detection_invalid(void)
{
    TEST_START("Audio file detection - Invalid files");
    
    ASSERT(is_audio_file("file.txt") == false, "Should reject .txt");
    ASSERT(is_audio_file("file.jpg") == false, "Should reject .jpg");
    ASSERT(is_audio_file("file.mp4") == false, "Should reject .mp4");
    ASSERT(is_audio_file("file") == false, "Should reject no extension");
    ASSERT(is_audio_file("mp3") == false, "Should reject too short");
    ASSERT(is_audio_file(".mp3") == false, "Should reject hidden .mp3");
    
    TEST_PASS();
}

static void test_directory_exists(void)
{
    TEST_START("Audio directory exists");
    
    DIR *dir = opendir("../audio");
    ASSERT(dir != NULL, "../audio directory should exist");
    closedir(dir);
    
    TEST_PASS();
}

static void test_scan_music_directory(void)
{
    TEST_START("Scan audio directory");
    
    DIR *dir = opendir("../audio");
    ASSERT(dir != NULL, "Should open directory");
    
    struct dirent *entry;
    int audio_file_count = 0;
    
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') {
            continue;
        }
        
        if (is_audio_file(entry->d_name)) {
            audio_file_count++;
        }
    }
    
    closedir(dir);
    
    ASSERT(audio_file_count > 0, "Should find at least one audio file");
    printf("(found %d files) ", audio_file_count);
    
    TEST_PASS();
}

static void test_time_formatting(void)
{
    TEST_START("Time formatting");
    
    // Helper function for time formatting
    char buf[16];
    uint32_t time_ms;
    
    // Test 0:00
    time_ms = 0;
    uint32_t total_seconds = time_ms / 1000;
    uint32_t minutes = total_seconds / 60;
    uint32_t seconds = total_seconds % 60;
    sprintf(buf, "%02u:%02u", minutes, seconds);
    ASSERT(strcmp(buf, "00:00") == 0, "Should format 0ms as 00:00");
    
    // Test 1:30 (90000ms)
    time_ms = 90000;
    total_seconds = time_ms / 1000;
    minutes = total_seconds / 60;
    seconds = total_seconds % 60;
    sprintf(buf, "%02u:%02u", minutes, seconds);
    ASSERT(strcmp(buf, "01:30") == 0, "Should format 90000ms as 01:30");
    
    // Test 3:45 (225000ms)
    time_ms = 225000;
    total_seconds = time_ms / 1000;
    minutes = total_seconds / 60;
    seconds = total_seconds % 60;
    sprintf(buf, "%02u:%02u", minutes, seconds);
    ASSERT(strcmp(buf, "03:45") == 0, "Should format 225000ms as 03:45");
    
    TEST_PASS();
}

static void test_file_paths(void)
{
    TEST_START("File path construction");
    
    char filepath[512];
    const char *filename = "test.mp3";
    
    snprintf(filepath, sizeof(filepath), "audio/%s", filename);
    ASSERT(strcmp(filepath, "audio/test.mp3") == 0, "Should construct correct path");
    
    TEST_PASS();
}

// Test suite runner
void run_all_tests(void)
{
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║          UTILITY FUNCTIONS TEST SUITE                     ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    // Run all tests
    test_audio_file_detection_mp3();
    test_audio_file_detection_wav();
    test_audio_file_detection_invalid();
    test_directory_exists();
    test_scan_music_directory();
    test_time_formatting();
    test_file_paths();
    
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

