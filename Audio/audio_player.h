/**
 * @file audio_player.h
 * @brief Audio playback backend for MP3 and WAV files
 */

#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    AUDIO_STATE_STOPPED,
    AUDIO_STATE_PLAYING,
    AUDIO_STATE_PAUSED
} audio_state_t;

typedef struct {
    char title[256];
    char artist[256];
    char album[256];
    uint32_t duration_ms;  // Total duration in milliseconds
    uint32_t sample_rate;
    uint8_t channels;
} audio_metadata_t;

// Callback function types
typedef void (*audio_position_cb_t)(uint32_t position_ms, uint32_t duration_ms);
typedef void (*audio_state_cb_t)(audio_state_t state);

/**
 * @brief Initialize the audio player backend
 * @return true on success, false on failure
 */
bool audio_player_init(void);

/**
 * @brief Clean up audio player resources
 */
void audio_player_cleanup(void);

/**
 * @brief Load an audio file (MP3 or WAV)
 * @param filepath Path to the audio file
 * @return true on success, false on failure
 */
bool audio_player_load(const char *filepath);

/**
 * @brief Start or resume playback
 */
void audio_player_play(void);

/**
 * @brief Pause playback
 */
void audio_player_pause(void);

/**
 * @brief Stop playback
 */
void audio_player_stop(void);

/**
 * @brief Seek to a position in the audio
 * @param position_ms Position in milliseconds
 */
void audio_player_seek(uint32_t position_ms);

/**
 * @brief Set volume level
 * @param volume Volume level (0-100)
 */
void audio_player_set_volume(uint8_t volume);

/**
 * @brief Get current volume level
 * @return Volume level (0-100)
 */
uint8_t audio_player_get_volume(void);

/**
 * @brief Get current playback state
 * @return Current audio state
 */
audio_state_t audio_player_get_state(void);

/**
 * @brief Get current playback position
 * @return Position in milliseconds
 */
uint32_t audio_player_get_position(void);

/**
 * @brief Get audio metadata
 * @param metadata Pointer to metadata structure to fill
 * @return true if metadata is available, false otherwise
 */
bool audio_player_get_metadata(audio_metadata_t *metadata);

/**
 * @brief Set callback for position updates
 * @param callback Callback function
 */
void audio_player_set_position_callback(audio_position_cb_t callback);

/**
 * @brief Set callback for state changes
 * @param callback Callback function
 */
void audio_player_set_state_callback(audio_state_cb_t callback);

#endif // AUDIO_PLAYER_H

