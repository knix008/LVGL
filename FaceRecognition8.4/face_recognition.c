#include "face_recognition.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>
#include <float.h>
#include <jpeglib.h>

// Database directory for storing embeddings
#define EMBEDDINGS_DB_DIR "face_embeddings"
#define EMBEDDINGS_EXTENSION ".emb"

// Images directory for storing registration photos
#define IMAGES_DB_DIR "images"

// Temporary embeddings during registration (not finalized)
typedef struct {
    char person_name[128];
    float *embeddings;  // Continuous array of embeddings (embedding_count * FACE_EMBEDDING_DIM floats)
    int embedding_count;
    int max_embeddings;  // Allocated size (in number of embeddings, not floats)
} TemporaryEmbeddings;

// Global state
static bool initialized = false;
static FaceEmbedding *registered_faces = NULL;
static int registered_face_count = 0;
static int registered_face_capacity = 0;

static TemporaryEmbeddings temp_embeddings[10];  // Support up to 10 registrations in progress
static int temp_embeddings_count = 0;

// Forward declarations
static void extract_face_embedding(const uint8_t *rgb_data, int width, int height, float *embedding);
static float compute_embedding_distance(const float *emb1, const float *emb2);

/**
 * Create embeddings database directory if it doesn't exist
 */
static bool ensure_embeddings_dir(void) {
    struct stat st;
    if (stat(EMBEDDINGS_DB_DIR, &st) == -1) {
        if (mkdir(EMBEDDINGS_DB_DIR, 0755) == -1) {
            fprintf(stderr, "Failed to create embeddings directory\n");
            return false;
        }
        printf("Created embeddings directory: %s\n", EMBEDDINGS_DB_DIR);
    }
    return true;
}

/**
 * Create images database directory if it doesn't exist
 */
static bool ensure_images_dir(void) {
    struct stat st;
    if (stat(IMAGES_DB_DIR, &st) == -1) {
        if (mkdir(IMAGES_DB_DIR, 0755) == -1) {
            fprintf(stderr, "Failed to create images directory\n");
            return false;
        }
        printf("Created images directory: %s\n", IMAGES_DB_DIR);
    }
    return true;
}

/**
 * Save RGB image to JPEG file
 */
static bool save_rgb_to_jpeg(const char *filename, const uint8_t *rgb_data, int width, int height) {
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE *outfile;
    JSAMPROW row_pointer[1];

    // Open output file
    outfile = fopen(filename, "wb");
    if (!outfile) {
        fprintf(stderr, "Cannot open %s for writing\n", filename);
        return false;
    }

    // Initialize JPEG compression object
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, outfile);

    // Set image parameters
    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;

    // Set compression parameters (quality=85)
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 85, TRUE);

    // Start compression
    jpeg_start_compress(&cinfo, TRUE);

    // Write scanlines
    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = (JSAMPROW)&rgb_data[cinfo.next_scanline * width * 3];
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    // Finish compression
    jpeg_finish_compress(&cinfo);
    fclose(outfile);
    jpeg_destroy_compress(&cinfo);

    printf("Saved image to %s\n", filename);
    return true;
}

/**
 * Compute Euclidean distance between two embeddings
 */
static float compute_embedding_distance(const float *emb1, const float *emb2) {
    float distance = 0.0f;
    for (int i = 0; i < FACE_EMBEDDING_DIM; i++) {
        float diff = emb1[i] - emb2[i];
        distance += diff * diff;
    }
    return sqrtf(distance);
}

/**
 * Extract face embedding from image using histogram equalization and feature extraction
 * This is a simplified embedding using image statistics
 * For production, use a CNN-based model (FaceNet, VGGFace2, etc.)
 */
static void extract_face_embedding(const uint8_t *rgb_data, int width, int height, float *embedding) {
    // Simple histogram-based embedding
    // In production, this would use a deep learning model

    memset(embedding, 0, FACE_EMBEDDING_DIM * sizeof(float));

    // Compute histogram bins (0-255 intensity range into FACE_EMBEDDING_DIM bins)
    int total_pixels = width * height * 3;

    for (int i = 0; i < total_pixels; i++) {
        int bin = (int)((rgb_data[i] / 255.0f) * (FACE_EMBEDDING_DIM - 1));
        if (bin >= FACE_EMBEDDING_DIM) bin = FACE_EMBEDDING_DIM - 1;
        embedding[bin] += 1.0f;
    }

    // Normalize
    float sum = 0.0f;
    for (int i = 0; i < FACE_EMBEDDING_DIM; i++) {
        sum += embedding[i];
    }

    if (sum > 0) {
        for (int i = 0; i < FACE_EMBEDDING_DIM; i++) {
            embedding[i] /= sum;
        }
    }
}

/**
 * Save embedding to file
 */
static bool save_embedding_to_file(const char *person_name, const FaceEmbedding *embedding) {
    char filename[256];
    snprintf(filename, sizeof(filename), "%s/%s%s", EMBEDDINGS_DB_DIR, person_name, EMBEDDINGS_EXTENSION);

    FILE *f = fopen(filename, "wb");
    if (!f) {
        fprintf(stderr, "Failed to open embedding file for writing: %s\n", filename);
        return false;
    }

    // Write header
    if (fwrite(&embedding->sample_count, sizeof(int), 1, f) != 1) {
        fprintf(stderr, "Failed to write sample count\n");
        fclose(f);
        return false;
    }

    // Write embedding
    if (fwrite(embedding->embedding, sizeof(float), FACE_EMBEDDING_DIM, f) != FACE_EMBEDDING_DIM) {
        fprintf(stderr, "Failed to write embedding data\n");
        fclose(f);
        return false;
    }

    fclose(f);
    printf("Saved embedding for %s to %s\n", person_name, filename);
    return true;
}

/**
 * Load embedding from file
 */
static bool load_embedding_from_file(const char *person_name, FaceEmbedding *embedding) {
    char filename[256];
    snprintf(filename, sizeof(filename), "%s/%s%s", EMBEDDINGS_DB_DIR, person_name, EMBEDDINGS_EXTENSION);

    FILE *f = fopen(filename, "rb");
    if (!f) {
        return false;
    }

    // Read header
    if (fread(&embedding->sample_count, sizeof(int), 1, f) != 1) {
        fprintf(stderr, "Failed to read sample count\n");
        fclose(f);
        return false;
    }

    // Read embedding
    if (fread(embedding->embedding, sizeof(float), FACE_EMBEDDING_DIM, f) != FACE_EMBEDDING_DIM) {
        fprintf(stderr, "Failed to read embedding data\n");
        fclose(f);
        return false;
    }

    fclose(f);
    strncpy(embedding->person_name, person_name, sizeof(embedding->person_name) - 1);
    embedding->person_name[sizeof(embedding->person_name) - 1] = '\0';
    return true;
}

/**
 * Load all registered face embeddings from database
 */
static bool load_all_embeddings(void) {
    DIR *dir = opendir(EMBEDDINGS_DB_DIR);
    if (!dir) {
        printf("No existing embeddings directory\n");
        return true;  // Not an error, just empty database
    }

    struct dirent *entry;
    int count = 0;

    while ((entry = readdir(dir)) != NULL) {
        // Check if file ends with .emb extension
        int len = strlen(entry->d_name);
        if (len > 4 && strcmp(&entry->d_name[len - 4], EMBEDDINGS_EXTENSION) == 0) {
            // Extract person name (remove extension)
            char person_name[128];
            memcpy(person_name, entry->d_name, len - 4);
            person_name[len - 4] = '\0';

            // Load embedding
            if (registered_face_count >= registered_face_capacity) {
                registered_face_capacity += 10;
                FaceEmbedding *new_faces = realloc(registered_faces,
                                                   registered_face_capacity * sizeof(FaceEmbedding));
                if (!new_faces) {
                    fprintf(stderr, "Failed to allocate memory for registered faces\n");
                    closedir(dir);
                    return false;
                }
                registered_faces = new_faces;
            }

            if (load_embedding_from_file(person_name, &registered_faces[registered_face_count])) {
                registered_face_count++;
                count++;
            }
        }
    }

    closedir(dir);
    printf("Loaded %d registered faces\n", count);
    return true;
}

bool face_recognition_init(void) {
    if (initialized) {
        return true;
    }

    // Create embeddings database directory
    if (!ensure_embeddings_dir()) {
        return false;
    }

    // Load existing embeddings
    if (!load_all_embeddings()) {
        return false;
    }

    // Initialize temporary embeddings array
    memset(temp_embeddings, 0, sizeof(temp_embeddings));
    temp_embeddings_count = 0;

    initialized = true;
    printf("Face recognition initialized\n");
    return true;
}

bool face_recognition_register_face(const char *person_name, const uint8_t *rgb_data, int width, int height) {
    if (!initialized || !person_name || !rgb_data) {
        fprintf(stderr, "Invalid parameters for face registration\n");
        return false;
    }

    // Find or create temporary embeddings for this person
    int temp_idx = -1;
    for (int i = 0; i < temp_embeddings_count; i++) {
        if (strcmp(temp_embeddings[i].person_name, person_name) == 0) {
            temp_idx = i;
            break;
        }
    }

    if (temp_idx == -1) {
        // Create new temporary embedding entry
        if (temp_embeddings_count >= 10) {
            fprintf(stderr, "Maximum registrations in progress exceeded\n");
            return false;
        }
        temp_idx = temp_embeddings_count++;
        strncpy(temp_embeddings[temp_idx].person_name, person_name, sizeof(temp_embeddings[temp_idx].person_name) - 1);
        temp_embeddings[temp_idx].person_name[sizeof(temp_embeddings[temp_idx].person_name) - 1] = '\0';
        temp_embeddings[temp_idx].max_embeddings = 5;  // Start with capacity for 5 samples
        temp_embeddings[temp_idx].embeddings = malloc(temp_embeddings[temp_idx].max_embeddings * FACE_EMBEDDING_DIM * sizeof(float));
        if (!temp_embeddings[temp_idx].embeddings) {
            fprintf(stderr, "Failed to allocate memory for embeddings\n");
            temp_embeddings_count--;
            return false;
        }
        temp_embeddings[temp_idx].embedding_count = 0;
    }

    // Extract and store embedding
    if (temp_embeddings[temp_idx].embedding_count >= temp_embeddings[temp_idx].max_embeddings) {
        // Expand capacity
        temp_embeddings[temp_idx].max_embeddings += 5;
        float *new_embeddings = realloc(temp_embeddings[temp_idx].embeddings,
                                       temp_embeddings[temp_idx].max_embeddings * FACE_EMBEDDING_DIM * sizeof(float));
        if (!new_embeddings) {
            fprintf(stderr, "Failed to expand embedding storage\n");
            return false;
        }
        temp_embeddings[temp_idx].embeddings = new_embeddings;
    }

    float *embedding = &temp_embeddings[temp_idx].embeddings[temp_embeddings[temp_idx].embedding_count * FACE_EMBEDDING_DIM];
    extract_face_embedding(rgb_data, width, height, embedding);
    temp_embeddings[temp_idx].embedding_count++;

    printf("Captured face sample %d for %s\n", temp_embeddings[temp_idx].embedding_count, person_name);
    return true;
}

bool face_recognition_finalize_registration(const char *person_name) {
    if (!initialized || !person_name) {
        return false;
    }

    // Find temporary embeddings for this person
    int temp_idx = -1;
    for (int i = 0; i < temp_embeddings_count; i++) {
        if (strcmp(temp_embeddings[i].person_name, person_name) == 0) {
            temp_idx = i;
            break;
        }
    }

    if (temp_idx == -1) {
        fprintf(stderr, "No temporary embeddings found for %s\n", person_name);
        return false;
    }

    if (temp_embeddings[temp_idx].embedding_count == 0) {
        fprintf(stderr, "No face samples captured for %s\n", person_name);
        return false;
    }

    // Average all embeddings together
    FaceEmbedding final_embedding;
    memset(&final_embedding.embedding, 0, FACE_EMBEDDING_DIM * sizeof(float));

    for (int i = 0; i < temp_embeddings[temp_idx].embedding_count; i++) {
        float *embedding = &temp_embeddings[temp_idx].embeddings[i * FACE_EMBEDDING_DIM];
        for (int j = 0; j < FACE_EMBEDDING_DIM; j++) {
            final_embedding.embedding[j] += embedding[j];
        }
    }

    // Normalize by dividing by count
    for (int i = 0; i < FACE_EMBEDDING_DIM; i++) {
        final_embedding.embedding[i] /= temp_embeddings[temp_idx].embedding_count;
    }

    final_embedding.sample_count = temp_embeddings[temp_idx].embedding_count;
    strncpy(final_embedding.person_name, person_name, sizeof(final_embedding.person_name) - 1);
    final_embedding.person_name[sizeof(final_embedding.person_name) - 1] = '\0';

    // Save to disk
    if (!save_embedding_to_file(person_name, &final_embedding)) {
        fprintf(stderr, "Failed to save embedding to file\n");
        return false;
    }

    // Update registered faces in memory
    // Check if person already exists
    int registered_idx = -1;
    for (int i = 0; i < registered_face_count; i++) {
        if (strcmp(registered_faces[i].person_name, person_name) == 0) {
            registered_idx = i;
            break;
        }
    }

    if (registered_idx == -1) {
        // Add new registered face
        if (registered_face_count >= registered_face_capacity) {
            registered_face_capacity += 10;
            FaceEmbedding *new_faces = realloc(registered_faces,
                                               registered_face_capacity * sizeof(FaceEmbedding));
            if (!new_faces) {
                fprintf(stderr, "Failed to allocate memory\n");
                return false;
            }
            registered_faces = new_faces;
        }
        registered_faces[registered_face_count] = final_embedding;
        registered_face_count++;
    } else {
        // Update existing face
        registered_faces[registered_idx] = final_embedding;
    }

    // Free temporary embeddings
    free(temp_embeddings[temp_idx].embeddings);

    // Shift remaining temporary embeddings
    for (int i = temp_idx; i < temp_embeddings_count - 1; i++) {
        temp_embeddings[i] = temp_embeddings[i + 1];
    }
    temp_embeddings_count--;

    printf("Registration finalized for %s with %d samples\n", person_name, final_embedding.sample_count);
    return true;
}

bool face_recognition_recognize(const uint8_t *rgb_data, int width, int height, RecognitionResult *result) {
    if (!initialized || !rgb_data || !result) {
        return false;
    }

    // Extract embedding from current frame
    float current_embedding[FACE_EMBEDDING_DIM];
    extract_face_embedding(rgb_data, width, height, current_embedding);

    // Compare against all registered faces
    float min_distance = FLT_MAX;
    int best_match_idx = -1;

    for (int i = 0; i < registered_face_count; i++) {
        float distance = compute_embedding_distance(current_embedding, registered_faces[i].embedding);
        if (distance < min_distance) {
            min_distance = distance;
            best_match_idx = i;
        }
    }

    // Initialize result
    result->is_recognized = false;
    result->confidence = 0.0f;
    strncpy(result->person_name, "Unknown", sizeof(result->person_name) - 1);
    result->person_name[sizeof(result->person_name) - 1] = '\0';

    // Check if best match is within threshold
    // Normalize distance to 0-1 confidence (lower distance = higher confidence)
    if (best_match_idx != -1) {
        // Distance threshold (empirical value, adjust based on testing)
        float distance_threshold = 0.3f;

        if (min_distance < distance_threshold) {
            result->is_recognized = true;
            result->confidence = 1.0f - (min_distance / distance_threshold);
            memcpy(result->person_name, registered_faces[best_match_idx].person_name,
                   sizeof(result->person_name) - 1);
            result->person_name[sizeof(result->person_name) - 1] = '\0';
        } else {
            // Not recognized but show similarity confidence anyway
            result->confidence = 1.0f - (min_distance / distance_threshold);
            if (result->confidence < 0.0f) result->confidence = 0.0f;
        }
    }

    return true;
}

bool face_recognition_save_registration_image(const char *person_name, int sample_number,
                                              const uint8_t *rgb_data, int width, int height) {
    printf("[DEBUG] face_recognition_save_registration_image called: name=%s, sample=%d, data=%p, w=%d, h=%d\n",
           person_name, sample_number, (void *)rgb_data, width, height);

    if (!person_name || !rgb_data) {
        fprintf(stderr, "Invalid parameters for image saving\n");
        return false;
    }

    // Ensure images directory exists
    if (!ensure_images_dir()) {
        fprintf(stderr, "Failed to ensure images directory\n");
        return false;
    }

    // Create person subdirectory
    char person_dir[256];
    snprintf(person_dir, sizeof(person_dir), "%s/%s", IMAGES_DB_DIR, person_name);
    printf("[DEBUG] Person directory: %s\n", person_dir);

    struct stat st;
    if (stat(person_dir, &st) == -1) {
        printf("[DEBUG] Creating person directory...\n");
        if (mkdir(person_dir, 0755) == -1) {
            fprintf(stderr, "Failed to create person directory: %s\n", person_dir);
            return false;
        }
        printf("[DEBUG] Person directory created\n");
    } else {
        printf("[DEBUG] Person directory already exists\n");
    }

    // Create filename with sample number
    char filename[PATH_MAX];
    snprintf(filename, PATH_MAX, "%s/sample_%d.jpg", person_dir, sample_number);
    printf("[DEBUG] Saving image to: %s\n", filename);

    // Save image as JPEG
    if (save_rgb_to_jpeg(filename, rgb_data, width, height)) {
        printf("Saved registration image for %s (sample %d)\n", person_name, sample_number);
        return true;
    }

    fprintf(stderr, "Failed to save JPEG image\n");
    return false;
}

int face_recognition_get_registered_persons(char **names, int max_names) {
    if (!initialized || !names) {
        return 0;
    }

    int count = 0;
    for (int i = 0; i < registered_face_count && i < max_names; i++) {
        names[i] = registered_faces[i].person_name;
        count++;
    }

    return count;
}

bool face_recognition_is_registered(const char *person_name) {
    if (!initialized || !person_name) {
        return false;
    }

    for (int i = 0; i < registered_face_count; i++) {
        if (strcmp(registered_faces[i].person_name, person_name) == 0) {
            return true;
        }
    }

    return false;
}

void face_recognition_cleanup(void) {
    if (initialized) {
        // Free registered faces
        if (registered_faces) {
            free(registered_faces);
            registered_faces = NULL;
        }
        registered_face_count = 0;
        registered_face_capacity = 0;

        // Free temporary embeddings
        for (int i = 0; i < temp_embeddings_count; i++) {
            if (temp_embeddings[i].embeddings) {
                free(temp_embeddings[i].embeddings);
            }
        }
        temp_embeddings_count = 0;

        initialized = false;
        printf("Face recognition cleanup complete\n");
    }
}

bool face_recognition_is_initialized(void) {
    return initialized;
}
