/*
 * firmware_downloader.c - Firmware Download and Verification Implementation
 *
 * Implements firmware downloading, hash verification, decryption, decompression,
 * and CRC verification functionality.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <stdarg.h>

#include "firmware_downloader.h"
#include "encryptor.h"
#include "compressor.h"
#include "hasher.h"
#include "crc.h"

/* ============================================================================
 * Constants and Macros
 * ============================================================================ */

#define DOWNLOAD_BUFFER_SIZE (256 * 1024)  /* 256 KB read buffer */
#define ERROR_BUFFER_SIZE 256

/* ============================================================================
 * Internal Data Structures
 * ============================================================================ */

/**
 * Internal firmware downloader structure
 */
struct firmware_downloader {
    download_status_t status;
    char error_message[ERROR_BUFFER_SIZE];
    download_stats_t stats;
    verification_data_t verification;

    /* Buffers */
    uint8_t *download_buffer;
    size_t download_buffer_size;

    /* Progress tracking */
    download_progress_callback_t progress_callback;
    void *progress_user_data;

    /* Verification tracking */
    verification_callback_t verify_callback;
    void *verify_user_data;
};

/* ============================================================================
 * Helper Functions
 * ============================================================================ */

/**
 * Set error message
 */
static void _set_error(firmware_downloader_t *dl, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsnprintf(dl->error_message, ERROR_BUFFER_SIZE, fmt, args);
    va_end(args);
    dl->status = DL_ERROR;
}

/**
 * Call progress callback if registered
 */
static int _call_progress_callback(
    firmware_downloader_t *dl,
    size_t bytes_downloaded,
    size_t total_bytes
)
{
    if (dl->progress_callback) {
        return dl->progress_callback(bytes_downloaded, total_bytes, dl->progress_user_data);
    }
    return 0;
}

/**
 * Call verification callback if registered
 */
static int _call_verify_callback(
    firmware_downloader_t *dl,
    download_status_t status,
    const char *message
)
{
    if (dl->verify_callback) {
        return dl->verify_callback(status, message, dl->verify_user_data);
    }
    return 0;
}

/**
 * Get file size
 */
static long _get_file_size(const char *filename)
{
    FILE *f = fopen(filename, "rb");
    if (!f) return -1;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fclose(f);
    return size;
}

/**
 * Copy file with progress tracking
 */
static int _copy_file_with_progress(
    firmware_downloader_t *dl,
    const char *source,
    const char *dest
)
{
    FILE *src = fopen(source, "rb");
    if (!src) {
        _set_error(dl, "Cannot open source file: %s", source);
        return -1;
    }

    FILE *dst = fopen(dest, "wb");
    if (!dst) {
        fclose(src);
        _set_error(dl, "Cannot create destination file: %s", dest);
        return -1;
    }

    long total = _get_file_size(source);
    if (total < 0) {
        fclose(src);
        fclose(dst);
        _set_error(dl, "Cannot get source file size");
        return -1;
    }

    dl->stats.total_bytes = total;
    dl->stats.start_time = time(NULL);
    dl->stats.downloaded_bytes = 0;

    size_t bytes_read;
    size_t total_read = 0;

    while ((bytes_read = fread(dl->download_buffer, 1, dl->download_buffer_size, src)) > 0) {
        if (fwrite(dl->download_buffer, 1, bytes_read, dst) != bytes_read) {
            fclose(src);
            fclose(dst);
            _set_error(dl, "Write error to destination file");
            return -1;
        }

        total_read += bytes_read;
        dl->stats.downloaded_bytes = total_read;

        /* Call progress callback */
        if (_call_progress_callback(dl, total_read, total) != 0) {
            fclose(src);
            fclose(dst);
            _set_error(dl, "Download cancelled by user");
            return -1;
        }

        /* Calculate download speed */
        time_t elapsed = time(NULL) - dl->stats.start_time;
        if (elapsed > 0) {
            dl->stats.download_speed_kbps = (float)(total_read / 1024.0) / elapsed;

            /* Estimate remaining time */
            if (dl->stats.download_speed_kbps > 0) {
                size_t remaining = total - total_read;
                dl->stats.estimated_time_ms = (uint32_t)((remaining / 1024.0) / dl->stats.download_speed_kbps * 1000);
            }
        }
    }

    fclose(src);
    fclose(dst);

    dl->stats.end_time = time(NULL);
    return 0;
}

/**
 * Convert hash algorithm to string
 */
static const char *_hash_algo_to_string(hash_algorithm_t algo)
{
    switch (algo) {
        case HASH_SHA256: return "sha256";
        case HASH_SHA512: return "sha512";
        case HASH_MD5: return "md5";
        default: return "unknown";
    }
}

/* ============================================================================
 * Public API Implementation
 * ============================================================================ */

firmware_downloader_t *fw_downloader_create(void)
{
    firmware_downloader_t *dl = malloc(sizeof(firmware_downloader_t));
    if (!dl) {
        return NULL;
    }

    memset(dl, 0, sizeof(firmware_downloader_t));

    dl->status = DL_IDLE;
    dl->error_message[0] = '\0';

    /* Allocate download buffer */
    dl->download_buffer_size = DOWNLOAD_BUFFER_SIZE;
    dl->download_buffer = malloc(dl->download_buffer_size);
    if (!dl->download_buffer) {
        free(dl);
        return NULL;
    }

    /* Initialize verification data */
    memset(&dl->verification, 0, sizeof(verification_data_t));
    memset(&dl->stats, 0, sizeof(download_stats_t));

    return dl;
}

void fw_downloader_free(firmware_downloader_t *downloader)
{
    if (!downloader) {
        return;
    }

    if (downloader->download_buffer) {
        free(downloader->download_buffer);
    }

    if (downloader->verification.expected_hash) {
        free(downloader->verification.expected_hash);
    }

    if (downloader->verification.calculated_hash) {
        free(downloader->verification.calculated_hash);
    }

    free(downloader);
}

int fw_downloader_from_file(
    firmware_downloader_t *downloader,
    const char *source_file,
    const char *dest_file,
    download_progress_callback_t progress_callback,
    void *user_data
)
{
    if (!downloader || !source_file || !dest_file) {
        return -1;
    }

    downloader->status = DL_CONNECTING;
    downloader->progress_callback = progress_callback;
    downloader->progress_user_data = user_data;

    if (_call_verify_callback(downloader, DL_CONNECTING, "Connecting to firmware source...") != 0) {
        _set_error(downloader, "Download cancelled");
        return -1;
    }

    downloader->status = DL_DOWNLOADING;

    if (_call_verify_callback(downloader, DL_DOWNLOADING, "Downloading firmware...") != 0) {
        _set_error(downloader, "Download cancelled");
        return -1;
    }

    int result = _copy_file_with_progress(downloader, source_file, dest_file);
    if (result != 0) {
        return -1;
    }

    return 0;
}

int fw_downloader_verify_hash(
    firmware_downloader_t *downloader,
    const char *filename,
    const char *expected_hash,
    hash_algorithm_t algorithm
)
{
    if (!downloader || !filename || !expected_hash) {
        return -1;
    }

    downloader->status = DL_VERIFYING;

    if (_call_verify_callback(downloader, DL_VERIFYING, "Verifying file hash...") != 0) {
        _set_error(downloader, "Hash verification cancelled");
        return -1;
    }

    /* Store verification metadata */
    if (downloader->verification.expected_hash) {
        free(downloader->verification.expected_hash);
    }
    downloader->verification.expected_hash = malloc(strlen(expected_hash) + 1);
    if (!downloader->verification.expected_hash) {
        _set_error(downloader, "Memory allocation failed");
        return -1;
    }
    strcpy(downloader->verification.expected_hash, expected_hash);
    downloader->verification.algorithm = algorithm;

    /* Compute hash of downloaded file */
    const char *algo_str = _hash_algo_to_string(algorithm);
    char *calculated_hash = NULL;

    if (hash_file(filename, algo_str, &calculated_hash) != 0) {
        _set_error(downloader, "Failed to compute file hash");
        return -1;
    }

    if (!calculated_hash) {
        _set_error(downloader, "Failed to compute file hash");
        return -1;
    }

    /* Store calculated hash */
    if (downloader->verification.calculated_hash) {
        free(downloader->verification.calculated_hash);
    }
    downloader->verification.calculated_hash = calculated_hash;

    /* Compare hashes */
    if (strcmp(calculated_hash, expected_hash) != 0) {
        _set_error(downloader, "Hash mismatch: expected %s, got %s", expected_hash, calculated_hash);
        downloader->verification.hash_verified = 0;
        return -1;
    }

    downloader->verification.hash_verified = 1;

    char msg[256];
    snprintf(msg, sizeof(msg), "Hash verified successfully (%s)", algo_str);
    _call_verify_callback(downloader, DL_VERIFYING, msg);

    return 0;
}

int fw_downloader_decrypt(
    firmware_downloader_t *downloader,
    const char *encrypted_file,
    const char *decrypted_file,
    const char *password
)
{
    if (!downloader || !encrypted_file || !decrypted_file || !password) {
        return -1;
    }

    downloader->status = DL_DECRYPTING;

    if (_call_verify_callback(downloader, DL_DECRYPTING, "Decrypting firmware image...") != 0) {
        _set_error(downloader, "Decryption cancelled");
        return -1;
    }

    /* Decrypt using AES-256 */
    int result = decrypt_file(encrypted_file, decrypted_file, password);

    if (result != 0) {
        _set_error(downloader, "AES-256 decryption failed");
        return -1;
    }

    _call_verify_callback(downloader, DL_DECRYPTING, "Decryption completed successfully");

    return 0;
}

int fw_downloader_decompress(
    firmware_downloader_t *downloader,
    const char *compressed_file,
    const char *decompressed_file
)
{
    if (!downloader || !compressed_file || !decompressed_file) {
        return -1;
    }

    downloader->status = DL_DECOMPRESSING;

    if (_call_verify_callback(downloader, DL_DECOMPRESSING, "Decompressing firmware image...") != 0) {
        _set_error(downloader, "Decompression cancelled");
        return -1;
    }

    /* Decompress using zlib */
    int result = decompress_file(compressed_file, decompressed_file);

    if (result != 0) {
        _set_error(downloader, "Firmware decompression failed");
        return -1;
    }

    long decomp_size = _get_file_size(decompressed_file);
    if (decomp_size >= 0) {
        downloader->stats.decompressed_bytes = decomp_size;
    }

    _call_verify_callback(downloader, DL_DECOMPRESSING, "Decompression completed successfully");

    return 0;
}

int fw_downloader_verify_crc(
    firmware_downloader_t *downloader,
    const char *filename,
    uint32_t expected_crc32
)
{
    if (!downloader || !filename) {
        return -1;
    }

    downloader->status = DL_CRC_CHECK;

    if (_call_verify_callback(downloader, DL_CRC_CHECK, "Verifying CRC checksum...") != 0) {
        _set_error(downloader, "CRC verification cancelled");
        return -1;
    }

    /* Calculate CRC32 of file */
    uint32_t calculated_crc = crc32_file(filename);

    downloader->verification.expected_crc32 = expected_crc32;
    downloader->verification.calculated_crc32 = calculated_crc;

    /* Compare CRC values */
    if (calculated_crc != expected_crc32) {
        _set_error(downloader, "CRC mismatch: expected 0x%08x, got 0x%08x",
                   expected_crc32, calculated_crc);
        downloader->verification.crc_verified = 0;
        return -1;
    }

    downloader->verification.crc_verified = 1;

    char msg[256];
    snprintf(msg, sizeof(msg), "CRC verified successfully (0x%08x)", calculated_crc);
    _call_verify_callback(downloader, DL_CRC_CHECK, msg);

    return 0;
}

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
)
{
    if (!downloader || !source_file || !output_file || !expected_hash) {
        return -1;
    }

    if (is_encrypted && !password) {
        _set_error(downloader, "Password required for encrypted firmware");
        return -1;
    }

    /* Set callbacks */
    downloader->progress_callback = progress_callback;
    downloader->progress_user_data = user_data;
    downloader->verify_callback = verify_callback;
    downloader->verify_user_data = user_data;

    /* Step 1: Download firmware */
    char temp_download[512];
    snprintf(temp_download, sizeof(temp_download), "%s.download", output_file);

    if (fw_downloader_from_file(downloader, source_file, temp_download,
                                 progress_callback, user_data) != 0) {
        return -1;
    }

    /* Step 2: Verify hash of downloaded file */
    if (fw_downloader_verify_hash(downloader, temp_download, expected_hash, HASH_SHA256) != 0) {
        remove(temp_download);
        return -1;
    }

    char *current_file = temp_download;
    char temp_decrypt[512] = {0};
    char temp_decomp[512] = {0};

    /* Step 3: Decrypt if encrypted */
    if (is_encrypted) {
        snprintf(temp_decrypt, sizeof(temp_decrypt), "%s.decrypted", output_file);

        if (fw_downloader_decrypt(downloader, current_file, temp_decrypt, password) != 0) {
            remove(current_file);
            return -1;
        }

        remove(current_file);
        current_file = temp_decrypt;
    }

    /* Step 4: Decompress if compressed */
    if (is_compressed) {
        snprintf(temp_decomp, sizeof(temp_decomp), "%s.decompressed", output_file);

        if (fw_downloader_decompress(downloader, current_file, temp_decomp) != 0) {
            remove(current_file);
            return -1;
        }

        remove(current_file);
        current_file = temp_decomp;
    }

    /* Step 5: Verify CRC of final firmware */
    if (fw_downloader_verify_crc(downloader, current_file, expected_crc32) != 0) {
        remove(current_file);
        return -1;
    }

    /* Step 6: Move final file to output location */
    if (rename(current_file, output_file) != 0) {
        _set_error(downloader, "Failed to move firmware to final location");
        remove(current_file);
        return -1;
    }

    downloader->status = DL_COMPLETE;
    _call_verify_callback(downloader, DL_COMPLETE, "Firmware download and verification complete!");

    return 0;
}

download_status_t fw_downloader_get_status(firmware_downloader_t *downloader)
{
    if (!downloader) {
        return DL_ERROR;
    }
    return downloader->status;
}

int fw_downloader_get_stats(
    firmware_downloader_t *downloader,
    download_stats_t *stats
)
{
    if (!downloader || !stats) {
        return -1;
    }

    memcpy(stats, &downloader->stats, sizeof(download_stats_t));
    return 0;
}

int fw_downloader_get_verification(
    firmware_downloader_t *downloader,
    verification_data_t *verification
)
{
    if (!downloader || !verification) {
        return -1;
    }

    /* Create a copy with allocated strings */
    memset(verification, 0, sizeof(verification_data_t));

    verification->algorithm = downloader->verification.algorithm;
    verification->hash_verified = downloader->verification.hash_verified;
    verification->expected_crc32 = downloader->verification.expected_crc32;
    verification->calculated_crc32 = downloader->verification.calculated_crc32;
    verification->crc_verified = downloader->verification.crc_verified;

    /* Copy hash strings */
    if (downloader->verification.expected_hash) {
        verification->expected_hash = malloc(strlen(downloader->verification.expected_hash) + 1);
        if (verification->expected_hash) {
            strcpy(verification->expected_hash, downloader->verification.expected_hash);
        }
    }

    if (downloader->verification.calculated_hash) {
        verification->calculated_hash = malloc(strlen(downloader->verification.calculated_hash) + 1);
        if (verification->calculated_hash) {
            strcpy(verification->calculated_hash, downloader->verification.calculated_hash);
        }
    }

    return 0;
}

const char *fw_downloader_get_error(firmware_downloader_t *downloader)
{
    if (!downloader || downloader->error_message[0] == '\0') {
        return NULL;
    }
    return downloader->error_message;
}

void fw_downloader_clear_error(firmware_downloader_t *downloader)
{
    if (!downloader) {
        return;
    }
    downloader->error_message[0] = '\0';
    if (downloader->status == DL_ERROR) {
        downloader->status = DL_IDLE;
    }
}
