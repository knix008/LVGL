/*
 * firmware_downloader.h - Firmware Download and Verification Module
 *
 * This module provides functionality for downloading firmware images,
 * verifying file integrity using hash values, decrypting AES-256 encrypted
 * firmware images, decompressing them, and verifying CRC checksums.
 *
 * Usage:
 *   1. Create a downloader instance
 *   2. Download firmware from source
 *   3. Verify hash of downloaded file
 *   4. Decrypt firmware image (if encrypted)
 *   5. Decompress firmware image
 *   6. Verify CRC checksum
 *   7. Save to output location
 */

#ifndef FIRMWARE_DOWNLOADER_H
#define FIRMWARE_DOWNLOADER_H

#include <time.h>
#include <stdint.h>
#include <stddef.h>
#include "firmware_builder.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * Data Types and Structures
 * ============================================================================ */

/**
 * Download status and progress tracking
 */
typedef enum {
    DL_IDLE,          /**< Not downloading */
    DL_CONNECTING,    /**< Establishing connection */
    DL_DOWNLOADING,   /**< Download in progress */
    DL_VERIFYING,     /**< Verifying hash */
    DL_DECRYPTING,    /**< Decrypting firmware */
    DL_DECOMPRESSING, /**< Decompressing firmware */
    DL_CRC_CHECK,     /**< Verifying CRC */
    DL_COMPLETE,      /**< Download complete */
    DL_ERROR          /**< Error occurred */
} download_status_t;

/**
 * Download statistics and metadata
 */
typedef struct {
    size_t total_bytes;         /**< Total firmware size in bytes */
    size_t downloaded_bytes;    /**< Bytes downloaded so far */
    size_t decompressed_bytes;  /**< Size after decompression */
    uint32_t estimated_time_ms; /**< Estimated time remaining in milliseconds */
    float download_speed_kbps;  /**< Current download speed in KBps */
    time_t start_time;          /**< Download start timestamp */
    time_t end_time;            /**< Download end timestamp */
} download_stats_t;

/**
 * Download verification metadata
 */
typedef struct {
    char *expected_hash;        /**< Expected hash value (hex string) */
    hash_algorithm_t algorithm; /**< Hash algorithm used */
    char *calculated_hash;      /**< Calculated hash of downloaded file */
    int hash_verified;          /**< 1 if hash matches, 0 otherwise */
    uint32_t expected_crc32;    /**< Expected CRC32 of decompressed firmware */
    uint32_t calculated_crc32;  /**< Calculated CRC32 of decompressed firmware */
    int crc_verified;           /**< 1 if CRC matches, 0 otherwise */
} verification_data_t;

/**
 * Firmware downloader instance
 *
 * Opaque structure containing:
 * - Download state and buffers
 * - Encryption/decryption context
 * - Verification metadata
 * - Progress tracking information
 */
typedef struct firmware_downloader firmware_downloader_t;

/**
 * Download progress callback function type
 *
 * @param bytes_downloaded Number of bytes downloaded
 * @param total_bytes Total firmware size
 * @param user_data User-provided context pointer
 * @return 0 to continue, non-zero to cancel
 */
typedef int (*download_progress_callback_t)(
    size_t bytes_downloaded,
    size_t total_bytes,
    void *user_data
);

/**
 * Verification callback function type
 *
 * @param status Current verification status
 * @param message Status message
 * @param user_data User-provided context pointer
 * @return 0 to continue, non-zero to cancel
 */
typedef int (*verification_callback_t)(
    download_status_t status,
    const char *message,
    void *user_data
);

/* ============================================================================
 * API Functions
 * ============================================================================ */

/**
 * Create a new firmware downloader instance
 *
 * @return New downloader instance on success, NULL on failure
 */
firmware_downloader_t *fw_downloader_create(void);

/**
 * Free firmware downloader instance and all associated resources
 *
 * @param downloader Downloader instance to free
 */
void fw_downloader_free(firmware_downloader_t *downloader);

/**
 * Download firmware from a local file
 *
 * Simulates downloading firmware by copying from a source file.
 * This is useful for testing and local firmware management.
 *
 * @param downloader Downloader instance
 * @param source_file Path to source firmware file
 * @param dest_file Path to destination file
 * @param progress_callback Optional callback for download progress (NULL to skip)
 * @param user_data Optional user data for callback
 * @return 0 on success, -1 on failure
 */
int fw_downloader_from_file(
    firmware_downloader_t *downloader,
    const char *source_file,
    const char *dest_file,
    download_progress_callback_t progress_callback,
    void *user_data
);

/**
 * Verify downloaded file hash
 *
 * Computes hash of downloaded file and compares with expected hash.
 * Supports SHA256, SHA512, and MD5 algorithms.
 *
 * @param downloader Downloader instance
 * @param filename Path to downloaded file
 * @param expected_hash Expected hash value (hex string)
 * @param algorithm Hash algorithm to use
 * @return 0 if hash matches, -1 on failure
 */
int fw_downloader_verify_hash(
    firmware_downloader_t *downloader,
    const char *filename,
    const char *expected_hash,
    hash_algorithm_t algorithm
);

/**
 * Decrypt AES-256 encrypted firmware file
 *
 * Decrypts firmware image using AES-256-CBC with PBKDF2 key derivation.
 * Password is used as the decryption password.
 *
 * @param downloader Downloader instance
 * @param encrypted_file Path to encrypted firmware file
 * @param decrypted_file Path to output decrypted file
 * @param password Decryption password
 * @return 0 on success, -1 on failure
 */
int fw_downloader_decrypt(
    firmware_downloader_t *downloader,
    const char *encrypted_file,
    const char *decrypted_file,
    const char *password
);

/**
 * Decompress firmware image
 *
 * Decompresses gzip-compressed firmware image to plaintext format.
 * Uses zlib for decompression with streaming for large files.
 *
 * @param downloader Downloader instance
 * @param compressed_file Path to compressed firmware file
 * @param decompressed_file Path to output decompressed file
 * @return 0 on success, -1 on failure
 */
int fw_downloader_decompress(
    firmware_downloader_t *downloader,
    const char *compressed_file,
    const char *decompressed_file
);

/**
 * Verify CRC32 checksum of firmware image
 *
 * Computes CRC32 of decompressed firmware and compares with expected value.
 *
 * @param downloader Downloader instance
 * @param filename Path to firmware file
 * @param expected_crc32 Expected CRC32 value
 * @return 0 if CRC matches, -1 on failure
 */
int fw_downloader_verify_crc(
    firmware_downloader_t *downloader,
    const char *filename,
    uint32_t expected_crc32
);

/**
 * Complete firmware download and verification pipeline
 *
 * Orchestrates the complete process:
 * 1. Download firmware (from file/URL)
 * 2. Verify hash of downloaded file
 * 3. Decrypt if encrypted
 * 4. Decompress
 * 5. Verify CRC checksum
 * 6. Save to output location
 *
 * @param downloader Downloader instance
 * @param source_file Source firmware file or URL
 * @param output_file Final output file path
 * @param password Decryption password (NULL if not encrypted)
 * @param expected_hash Expected SHA256 hash (hex string)
 * @param expected_crc32 Expected CRC32 of decompressed firmware
 * @param is_encrypted Whether firmware is AES-256 encrypted
 * @param is_compressed Whether firmware is gzip compressed
 * @param progress_callback Optional download progress callback
 * @param verify_callback Optional verification status callback
 * @param user_data Optional user data for callbacks
 * @return 0 on success, -1 on failure
 */
int fw_downloader_process(
    firmware_downloader_t *downloader,
    const char *source_file,
    const char *output_file,
    const char *password,
    const char *expected_hash,
    uint32_t expected_crc32,
    int is_encrypted,
    int is_compressed,
    download_progress_callback_t progress_callback,
    verification_callback_t verify_callback,
    void *user_data
);

/**
 * Get current download status
 *
 * @param downloader Downloader instance
 * @return Current download status
 */
download_status_t fw_downloader_get_status(firmware_downloader_t *downloader);

/**
 * Get download statistics
 *
 * @param downloader Downloader instance
 * @param stats Pointer to stats structure to fill
 * @return 0 on success, -1 on failure
 */
int fw_downloader_get_stats(
    firmware_downloader_t *downloader,
    download_stats_t *stats
);

/**
 * Get verification data
 *
 * @param downloader Downloader instance
 * @param verification Pointer to verification structure to fill
 * @return 0 on success, -1 on failure
 */
int fw_downloader_get_verification(
    firmware_downloader_t *downloader,
    verification_data_t *verification
);

/**
 * Get error message from last operation
 *
 * @param downloader Downloader instance
 * @return Error message (constant string), NULL if no error
 */
const char *fw_downloader_get_error(firmware_downloader_t *downloader);

/**
 * Clear error state
 *
 * @param downloader Downloader instance
 */
void fw_downloader_clear_error(firmware_downloader_t *downloader);

#ifdef __cplusplus
}
#endif

#endif /* FIRMWARE_DOWNLOADER_H */
