/**
 * @file test_video_player.c
 * @brief Test cases for video player functionality
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/stat.h>
#include <SDL2/SDL.h>
#include "../video_player.h"
#include "../korean_fonts.h"
#include "../lvgl/lvgl.h"

// Test configuration
#define TEST_VIDEO_FILE "test_video.mp4"
#define TEST_VIDEO_PATH "../video/" TEST_VIDEO_FILE
#define TEST_TIMEOUT_MS 1000

// Test state tracking
static int tests_passed = 0;
static int tests_failed = 0;
static int total_tests = 0;

// Test helper macros
#define TEST_ASSERT(condition, message) \
    do { \
        total_tests++; \
        if (condition) { \
            printf("✓ PASS: %s\n", message); \
            tests_passed++; \
        } else { \
            printf("✗ FAIL: %s\n", message); \
            tests_failed++; \
        } \
    } while(0)

#define TEST_ASSERT_EQUAL(expected, actual, message) \
    TEST_ASSERT((expected) == (actual), message)

#define TEST_ASSERT_NOT_NULL(ptr, message) \
    TEST_ASSERT((ptr) != NULL, message)

#define TEST_ASSERT_NULL(ptr, message) \
    TEST_ASSERT((ptr) == NULL, message)

#define TEST_ASSERT_STR_EQUAL(expected, actual, message) \
    TEST_ASSERT(strcmp(expected, actual) == 0, message)

// Test helper functions
static bool file_exists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0;
}

static void create_test_video_file(void) {
    // Create a simple test video file using FFmpeg
    char command[512];
    snprintf(command, sizeof(command), 
        "ffmpeg -f lavfi -i testsrc=duration=5:size=320x240:rate=30 -f lavfi -i sine=frequency=1000:duration=5 -c:v libx264 -c:a aac -shortest -y %s",
        TEST_VIDEO_PATH);
    
    printf("Creating test video file...\n");
    int result = system(command);
    if (result != 0) {
        printf("Warning: Could not create test video file (ffmpeg may not be available)\n");
    }
}

static void cleanup_test_video_file(void) {
    if (file_exists(TEST_VIDEO_PATH)) {
        unlink(TEST_VIDEO_PATH);
    }
}

// Test helper functions
static void test_setup_lvgl(void) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("Warning: Could not initialize SDL: %s\n", SDL_GetError());
        return;
    }
    
    // Initialize LVGL
    lv_init();
    
    // Create a minimal display for testing
    lv_display_t *disp = lv_sdl_window_create(640, 480);
    if (disp) {
        lv_display_set_default(disp);
    }
    
    // Create a minimal input device
    lv_indev_t *indev = lv_sdl_mouse_create();
    if (indev) {
        lv_indev_set_group(indev, lv_group_get_default());
    }
}

static void test_cleanup_lvgl(void) {
    // Cleanup LVGL
    lv_deinit();
    
    // Cleanup SDL
    SDL_Quit();
}

// Test cases
static void test_video_player_init(void) {
    printf("\n=== Testing Video Player Initialization ===\n");
    
    video_player_init();
    
    // Test initial state
    TEST_ASSERT_EQUAL(PLAYER_STOPPED, video_player_get_state(), "Initial state should be STOPPED");
    TEST_ASSERT_EQUAL(0, video_player_get_position(), "Initial position should be 0");
    TEST_ASSERT_EQUAL(0, video_player_get_duration(), "Initial duration should be 0");
    TEST_ASSERT_EQUAL(50, video_player_get_volume(), "Initial volume should be 50");
    TEST_ASSERT_NULL(video_player_get_current_file(), "Initial file should be NULL");
}

static void test_video_player_cleanup(void) {
    printf("\n=== Testing Video Player Cleanup ===\n");
    
    video_player_cleanup();
    
    // Test state after cleanup
    TEST_ASSERT_EQUAL(PLAYER_STOPPED, video_player_get_state(), "State after cleanup should be STOPPED");
    TEST_ASSERT_EQUAL(0, video_player_get_position(), "Position after cleanup should be 0");
}

static void test_video_player_file_loading(void) {
    printf("\n=== Testing Video Player File Loading ===\n");
    
    // Create a mock video player object for testing
    lv_obj_t *mock_player = lv_obj_create(lv_screen_active());
    if (mock_player) {
        video_player_set_object(mock_player);
    }
    
    // Test loading non-existent file
    video_player_load_file("nonexistent.mp4");
    TEST_ASSERT_EQUAL(PLAYER_STOPPED, video_player_get_state(), "State should remain STOPPED for non-existent file");
    
    // Test loading valid file (if test video exists)
    if (file_exists(TEST_VIDEO_PATH)) {
        video_player_load_file(TEST_VIDEO_FILE);
        TEST_ASSERT_STR_EQUAL(TEST_VIDEO_FILE, video_player_get_current_file(), "Current file should be set correctly");
        TEST_ASSERT_EQUAL(PLAYER_STOPPED, video_player_get_state(), "State should be STOPPED after loading");
    } else {
        printf("Skipping file loading test - test video not available\n");
    }
}

static void test_video_player_playback_controls(void) {
    printf("\n=== Testing Video Player Playback Controls ===\n");
    
    if (!file_exists(TEST_VIDEO_PATH)) {
        printf("Skipping playback control tests - test video not available\n");
        return;
    }
    
    // Create a mock video player object for testing
    lv_obj_t *mock_player = lv_obj_create(lv_screen_active());
    if (mock_player) {
        video_player_set_object(mock_player);
    }
    
    // Load test video
    video_player_load_file(TEST_VIDEO_FILE);
    
    // Test play from stopped state
    video_player_play();
    TEST_ASSERT_EQUAL(PLAYER_PLAYING, video_player_get_state(), "State should be PLAYING after play");
    
    // Test pause
    video_player_pause();
    TEST_ASSERT_EQUAL(PLAYER_PAUSED, video_player_get_state(), "State should be PAUSED after pause");
    
    // Test resume from pause
    video_player_play();
    TEST_ASSERT_EQUAL(PLAYER_PLAYING, video_player_get_state(), "State should be PLAYING after resume");
    
    // Test stop
    video_player_stop();
    TEST_ASSERT_EQUAL(PLAYER_STOPPED, video_player_get_state(), "State should be STOPPED after stop");
    TEST_ASSERT_EQUAL(0, video_player_get_position(), "Position should be 0 after stop");
    
    // Test play from stopped state
    video_player_play();
    TEST_ASSERT_EQUAL(PLAYER_PLAYING, video_player_get_state(), "State should be PLAYING after play from stop");
}

static void test_video_player_volume_control(void) {
    printf("\n=== Testing Video Player Volume Control ===\n");
    
    // Test volume setting
    video_player_set_volume(75);
    TEST_ASSERT_EQUAL(75, video_player_get_volume(), "Volume should be set to 75");
    
    // Test volume clamping (should not exceed 100)
    video_player_set_volume(150);
    TEST_ASSERT_EQUAL(100, video_player_get_volume(), "Volume should be clamped to 100");
    
    // Test volume clamping (should not go below 0)
    video_player_set_volume(-50);
    TEST_ASSERT_EQUAL(0, video_player_get_volume(), "Volume should be clamped to 0");
    
    // Test normal volume setting
    video_player_set_volume(25);
    TEST_ASSERT_EQUAL(25, video_player_get_volume(), "Volume should be set to 25");
}

static void test_video_player_seek(void) {
    printf("\n=== Testing Video Player Seek ===\n");
    
    if (!file_exists(TEST_VIDEO_PATH)) {
        printf("Skipping seek test - test video not available\n");
        return;
    }
    
    // Create a mock video player object for testing
    lv_obj_t *mock_player = lv_obj_create(lv_screen_active());
    if (mock_player) {
        video_player_set_object(mock_player);
    }
    
    // Load test video
    video_player_load_file(TEST_VIDEO_FILE);
    
    // Test seek to valid position
    video_player_seek(30);
    TEST_ASSERT_EQUAL(30, video_player_get_position(), "Position should be set to 30");
    
    // Test seek to negative position (should clamp to 0)
    video_player_seek(-10);
    TEST_ASSERT_EQUAL(0, video_player_get_position(), "Position should be clamped to 0");
    
    // Test seek beyond duration (should clamp to duration)
    video_player_seek(9999);
    TEST_ASSERT_EQUAL(video_player_get_duration(), video_player_get_position(), "Position should be clamped to duration");
}

static void test_video_player_state_transitions(void) {
    printf("\n=== Testing Video Player State Transitions ===\n");
    
    if (!file_exists(TEST_VIDEO_PATH)) {
        printf("Skipping state transition tests - test video not available\n");
        return;
    }
    
    // Create a mock video player object for testing
    lv_obj_t *mock_player = lv_obj_create(lv_screen_active());
    if (mock_player) {
        video_player_set_object(mock_player);
    }
    
    // Load test video
    video_player_load_file(TEST_VIDEO_FILE);
    
    // Test invalid transitions
    video_player_pause(); // Should not work from STOPPED state
    TEST_ASSERT_EQUAL(PLAYER_STOPPED, video_player_get_state(), "State should remain STOPPED when pausing from stopped");
    
    video_player_stop(); // Should not work from STOPPED state
    TEST_ASSERT_EQUAL(PLAYER_STOPPED, video_player_get_state(), "State should remain STOPPED when stopping from stopped");
    
    // Test valid transitions
    video_player_play();
    TEST_ASSERT_EQUAL(PLAYER_PLAYING, video_player_get_state(), "State should be PLAYING after play");
    
    video_player_pause();
    TEST_ASSERT_EQUAL(PLAYER_PAUSED, video_player_get_state(), "State should be PAUSED after pause");
    
    video_player_play(); // Resume from pause
    TEST_ASSERT_EQUAL(PLAYER_PLAYING, video_player_get_state(), "State should be PLAYING after resume");
    
    video_player_stop();
    TEST_ASSERT_EQUAL(PLAYER_STOPPED, video_player_get_state(), "State should be STOPPED after stop");
}

static void test_video_player_edge_cases(void) {
    printf("\n=== Testing Video Player Edge Cases ===\n");
    
    // Test operations without loading a file
    video_player_play();
    TEST_ASSERT_EQUAL(PLAYER_STOPPED, video_player_get_state(), "State should remain STOPPED when playing without file");
    
    video_player_pause();
    TEST_ASSERT_EQUAL(PLAYER_STOPPED, video_player_get_state(), "State should remain STOPPED when pausing without file");
    
    // Test multiple consecutive operations
    video_player_stop();
    video_player_stop();
    video_player_stop();
    TEST_ASSERT_EQUAL(PLAYER_STOPPED, video_player_get_state(), "State should remain STOPPED after multiple stops");
    
    // Test play when already playing
    if (file_exists(TEST_VIDEO_PATH)) {
        video_player_load_file(TEST_VIDEO_FILE);
        video_player_play();
        video_player_play(); // Should not change state
        TEST_ASSERT_EQUAL(PLAYER_PLAYING, video_player_get_state(), "State should remain PLAYING when playing again");
        video_player_stop();
    }
}

static void test_video_player_performance(void) {
    printf("\n=== Testing Video Player Performance ===\n");
    
    // Test rapid state changes
    if (file_exists(TEST_VIDEO_PATH)) {
        // Create a mock video player object for testing
        lv_obj_t *mock_player = lv_obj_create(lv_screen_active());
        if (mock_player) {
            video_player_set_object(mock_player);
        }
        
        video_player_load_file(TEST_VIDEO_FILE);
        
        for (int i = 0; i < 10; i++) {
            video_player_play();
            video_player_pause();
        }
        
        TEST_ASSERT_EQUAL(PLAYER_PAUSED, video_player_get_state(), "State should be PAUSED after rapid play/pause cycles");
        
        // Test rapid volume changes
        for (int i = 0; i < 100; i++) {
            video_player_set_volume(i % 101);
        }
        
        TEST_ASSERT_EQUAL(0, video_player_get_volume(), "Volume should be 0 after rapid volume changes");
        
        video_player_stop();
    }
}

// Main test runner
int main(void) {
    printf("========================================\n");
    printf("    Video Player Test Suite\n");
    printf("========================================\n");
    
    // Initialize LVGL for testing
    test_setup_lvgl();
    
    // Create test video file
    create_test_video_file();
    
    // Run all tests
    test_video_player_init();
    test_video_player_file_loading();
    test_video_player_playback_controls();
    test_video_player_volume_control();
    test_video_player_seek();
    test_video_player_state_transitions();
    test_video_player_edge_cases();
    test_video_player_performance();
    test_video_player_cleanup();
    
    // Print results
    printf("\n========================================\n");
    printf("    Test Results Summary\n");
    printf("========================================\n");
    printf("Total Tests: %d\n", total_tests);
    printf("Passed: %d\n", tests_passed);
    printf("Failed: %d\n", tests_failed);
    printf("Success Rate: %.1f%%\n", (float)tests_passed / total_tests * 100);
    
    if (tests_failed == 0) {
        printf("\n🎉 All tests passed!\n");
    } else {
        printf("\n❌ Some tests failed!\n");
    }
    
    // Cleanup
    cleanup_test_video_file();
    test_cleanup_lvgl();
    
    return (tests_failed == 0) ? 0 : 1;
}
