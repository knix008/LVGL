/**
 * @file test_config.h
 * @brief Test configuration and constants
 */

#ifndef TEST_CONFIG_H
#define TEST_CONFIG_H

// Test configuration
#define TEST_VIDEO_DURATION_SECONDS 5
#define TEST_VIDEO_WIDTH 320
#define TEST_VIDEO_HEIGHT 240
#define TEST_VIDEO_FPS 30
#define TEST_AUDIO_FREQUENCY 1000
#define TEST_AUDIO_DURATION_SECONDS 5

// Test file paths
#define TEST_VIDEO_DIR "../video"
#define TEST_VIDEO_FILE "test_video.mp4"
#define TEST_VIDEO_PATH TEST_VIDEO_DIR "/" TEST_VIDEO_FILE

// Test timeouts (in milliseconds)
#define TEST_TIMEOUT_SHORT 100
#define TEST_TIMEOUT_MEDIUM 1000
#define TEST_TIMEOUT_LONG 5000

// Test volume levels
#define TEST_VOLUME_MIN 0
#define TEST_VOLUME_MAX 100
#define TEST_VOLUME_DEFAULT 50
#define TEST_VOLUME_LOW 25
#define TEST_VOLUME_HIGH 75

// Test position values
#define TEST_POSITION_START 0
#define TEST_POSITION_MIDDLE 30
#define TEST_POSITION_END 120

// Test state values
#define TEST_STATE_STOPPED 0
#define TEST_STATE_PLAYING 1
#define TEST_STATE_PAUSED 2

// Test error codes
#define TEST_ERROR_NONE 0
#define TEST_ERROR_FILE_NOT_FOUND -1
#define TEST_ERROR_INVALID_STATE -2
#define TEST_ERROR_TIMEOUT -3

// Test helper macros
#define TEST_ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#define TEST_MIN(a, b) ((a) < (b) ? (a) : (b))
#define TEST_MAX(a, b) ((a) > (b) ? (a) : (b))
#define TEST_CLAMP(val, min, max) TEST_MIN(TEST_MAX(val, min), max)

// Test assertion macros
#define TEST_ASSERT_TRUE(condition) \
    do { \
        if (!(condition)) { \
            printf("Assertion failed: %s is not true\n", #condition); \
            return 1; \
        } \
    } while(0)

#define TEST_ASSERT_FALSE(condition) \
    do { \
        if (condition) { \
            printf("Assertion failed: %s is not false\n", #condition); \
            return 1; \
        } \
    } while(0)

#define TEST_ASSERT_EQUAL(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            printf("Assertion failed: expected %d, got %d\n", (expected), (actual)); \
            return 1; \
        } \
    } while(0)

#define TEST_ASSERT_NOT_EQUAL(expected, actual) \
    do { \
        if ((expected) == (actual)) { \
            printf("Assertion failed: expected not %d, got %d\n", (expected), (actual)); \
            return 1; \
        } \
    } while(0)

#define TEST_ASSERT_STR_EQUAL(expected, actual) \
    do { \
        if (strcmp(expected, actual) != 0) { \
            printf("Assertion failed: expected '%s', got '%s'\n", (expected), (actual)); \
            return 1; \
        } \
    } while(0)

#define TEST_ASSERT_NULL(ptr) \
    do { \
        if ((ptr) != NULL) { \
            printf("Assertion failed: expected NULL, got non-NULL\n"); \
            return 1; \
        } \
    } while(0)

#define TEST_ASSERT_NOT_NULL(ptr) \
    do { \
        if ((ptr) == NULL) { \
            printf("Assertion failed: expected non-NULL, got NULL\n"); \
            return 1; \
        } \
    } while(0)

// Test logging macros
#define TEST_LOG_INFO(fmt, ...) \
    printf("[INFO] " fmt "\n", ##__VA_ARGS__)

#define TEST_LOG_DEBUG(fmt, ...) \
    printf("[DEBUG] " fmt "\n", ##__VA_ARGS__)

#define TEST_LOG_ERROR(fmt, ...) \
    printf("[ERROR] " fmt "\n", ##__VA_ARGS__)

#define TEST_LOG_WARNING(fmt, ...) \
    printf("[WARNING] " fmt "\n", ##__VA_ARGS__)

// Test timing macros
#define TEST_START_TIMER() \
    clock_t test_start_time = clock()

#define TEST_END_TIMER() \
    clock_t test_end_time = clock()

#define TEST_GET_ELAPSED_MS() \
    ((test_end_time - test_start_time) * 1000 / CLOCKS_PER_SEC)

// Test file operations
#define TEST_FILE_EXISTS(path) \
    (access(path, F_OK) == 0)

#define TEST_FILE_SIZE(path) \
    ({ \
        struct stat st; \
        stat(path, &st) == 0 ? st.st_size : -1; \
    })

// Test directory operations
#define TEST_CREATE_DIR(path) \
    mkdir(path, 0755)

#define TEST_REMOVE_FILE(path) \
    unlink(path)

// Test system operations
#define TEST_SYSTEM_COMMAND(cmd) \
    system(cmd)

// Test memory operations
#define TEST_MALLOC(size) \
    malloc(size)

#define TEST_FREE(ptr) \
    do { \
        if (ptr) { \
            free(ptr); \
            ptr = NULL; \
        } \
    } while(0)

// Test string operations
#define TEST_STR_LEN(str) \
    strlen(str)

#define TEST_STR_COPY(dest, src, size) \
    strncpy(dest, src, size)

#define TEST_STR_CAT(dest, src, size) \
    strncat(dest, src, size)

// Test math operations
#define TEST_ABS(val) \
    ((val) < 0 ? -(val) : (val))

#define TEST_ROUND(val) \
    ((int)((val) + 0.5))

#define TEST_FLOOR(val) \
    ((int)(val))

#define TEST_CEIL(val) \
    ((int)((val) + 0.999999))

// Test random operations
#define TEST_RANDOM() \
    rand()

#define TEST_RANDOM_RANGE(min, max) \
    ((rand() % ((max) - (min) + 1)) + (min))

// Test sleep operations
#define TEST_SLEEP_MS(ms) \
    usleep((ms) * 1000)

#define TEST_SLEEP_SEC(sec) \
    sleep(sec)

#endif // TEST_CONFIG_H
