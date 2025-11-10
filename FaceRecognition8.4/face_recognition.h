#ifndef FACE_RECOGNITION_H
#define FACE_RECOGNITION_H

#include <stdint.h>
#include <stdbool.h>

// Face embedding dimension (using LBPH face recognizer)
#define FACE_EMBEDDING_DIM 256

// Recognition confidence threshold (0-1)
#define RECOGNITION_THRESHOLD 0.5f

// Structure to hold face embeddings
typedef struct {
    float embedding[FACE_EMBEDDING_DIM];
    char person_name[128];
    int sample_count;  // Number of samples used to train this embedding
} FaceEmbedding;

// Structure for recognition result
typedef struct {
    char person_name[128];
    float confidence;  // How confident we are (0-1, higher is better)
    bool is_recognized;
} RecognitionResult;

/**
 * Initialize face recognition system
 * Loads registered faces database
 * @return true on success, false on failure
 */
bool face_recognition_init(void);

/**
 * Register a new person face
 * @param person_name Name of the person to register
 * @param rgb_data RGB24 image data (width * height * 3 bytes)
 * @param width Image width in pixels
 * @param height Image height in pixels
 * @param save_image If true, also save the image to images/ directory (optional)
 * @return true on success, false on failure
 */
bool face_recognition_register_face(const char *person_name, const uint8_t *rgb_data, int width, int height);

/**
 * Save registration image to disk
 * @param person_name Name of the person
 * @param sample_number Sample number (1, 2, 3, etc.)
 * @param rgb_data RGB24 image data
 * @param width Image width
 * @param height Image height
 * @return true on success, false on failure
 */
bool face_recognition_save_registration_image(const char *person_name, int sample_number,
                                              const uint8_t *rgb_data, int width, int height);

/**
 * Finalize person registration (save embeddings to database)
 * @param person_name Name of the person being registered
 * @return true on success, false on failure
 */
bool face_recognition_finalize_registration(const char *person_name);

/**
 * Recognize a face from image data
 * @param rgb_data RGB24 image data (width * height * 3 bytes)
 * @param width Image width in pixels
 * @param height Image height in pixels
 * @param result Output recognition result
 * @return true if face was processed (recognized or not), false on error
 */
bool face_recognition_recognize(const uint8_t *rgb_data, int width, int height, RecognitionResult *result);

/**
 * Get list of registered persons
 * @param names Output array of person names
 * @param max_names Maximum number of names to return
 * @return Number of registered persons
 */
int face_recognition_get_registered_persons(char **names, int max_names);

/**
 * Check if a person is registered
 * @param person_name Name of the person
 * @return true if registered, false otherwise
 */
bool face_recognition_is_registered(const char *person_name);

/**
 * Cleanup face recognition resources
 */
void face_recognition_cleanup(void);

/**
 * Check if face recognition is initialized
 * @return true if initialized, false otherwise
 */
bool face_recognition_is_initialized(void);

#endif // FACE_RECOGNITION_H
