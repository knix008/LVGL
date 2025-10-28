#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "firmware_builder.h"
#include "compressor.h"
#include "crc.h"
#include "encryptor.h"
#include "hasher.h"

/**
 * Create a new firmware builder
 */
fw_package_t* fw_builder_create(const char *source, const char *output_dir, fw_build_options_t options)
{
    if (!source || !output_dir) {
        fprintf(stderr, "Invalid parameters\n");
        return NULL;
    }

    /* Check if source exists */
    struct stat st;
    if (stat(source, &st) != 0) {
        fprintf(stderr, "Source path does not exist: %s\n", source);
        return NULL;
    }

    /* Create output directory if it doesn't exist */
    mkdir(output_dir, 0755);

    /* Allocate package structure */
    fw_package_t *pkg = (fw_package_t *)malloc(sizeof(fw_package_t));
    if (!pkg) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    memset(pkg, 0, sizeof(fw_package_t));

    /* Set package metadata */
    pkg->name = (char *)malloc(256);
    pkg->version = (char *)malloc(32);
    pkg->compression_algorithm = (char *)malloc(32);
    pkg->crc_algorithm = (char *)malloc(32);
    pkg->encryption_algorithm = (char *)malloc(32);
    pkg->hash_algorithm = (char *)malloc(32);

    if (!pkg->name || !pkg->version || !pkg->compression_algorithm ||
        !pkg->crc_algorithm || !pkg->encryption_algorithm || !pkg->hash_algorithm) {
        fprintf(stderr, "Memory allocation failed\n");
        fw_builder_free(pkg);
        return NULL;
    }

    snprintf(pkg->name, 256, "%s", source);
    snprintf(pkg->version, 32, "1.0.0");
    snprintf(pkg->compression_algorithm, 32, "gzip");
    snprintf(pkg->crc_algorithm, 32, "CRC32");
    snprintf(pkg->encryption_algorithm, 32, "aes-256-cbc");
    snprintf(pkg->hash_algorithm, 32, "sha256");

    pkg->timestamp = time(NULL);
    pkg->compression_enabled = options.compress;
    pkg->compression_level = options.compression_level;
    pkg->crc_enabled = options.add_crc;
    pkg->encryption_enabled = options.encrypt;
    pkg->hash_enabled = options.generate_hash;

    /* Allocate file paths */
    pkg->compressed_file = (char *)malloc(512);
    pkg->encrypted_file = (char *)malloc(512);
    pkg->final_file = (char *)malloc(512);
    pkg->manifest_file = (char *)malloc(512);

    if (!pkg->compressed_file || !pkg->encrypted_file ||
        !pkg->final_file || !pkg->manifest_file) {
        fprintf(stderr, "Memory allocation failed\n");
        fw_builder_free(pkg);
        return NULL;
    }

    /* Set file paths */
    snprintf(pkg->compressed_file, 512, "%s/firmware.tar.gz", output_dir);
    snprintf(pkg->encrypted_file, 512, "%s/firmware.tar.gz.enc", output_dir);
    snprintf(pkg->final_file, 512, "%s/firmware.bin", output_dir);
    snprintf(pkg->manifest_file, 512, "%s/manifest.json", output_dir);

    return pkg;
}

/**
 * Build the firmware package
 */
int fw_builder_build(fw_package_t *pkg)
{
    if (!pkg) {
        fprintf(stderr, "Invalid package\n");
        return -1;
    }

    printf("🔨 Starting firmware build...\n");
    printf("📦 Source: %s\n", pkg->name);
    printf("📁 Output: %s\n", pkg->manifest_file);

    /* Step 1: Compression */
    if (pkg->compression_enabled) {
        printf("📦 Compressing...\n");
        if (compress_file(pkg->name, pkg->compressed_file, pkg->compression_level) != 0) {
            fprintf(stderr, "Compression failed\n");
            return -1;
        }
    }

    /* Step 2: CRC Calculation */
    if (pkg->crc_enabled) {
        printf("🔐 Calculating CRC...\n");
        const char *file_to_crc = pkg->compression_enabled ? pkg->compressed_file : pkg->name;

        uint32_t crc_value = crc32_file(file_to_crc);
        pkg->crc_value = (char *)malloc(16);
        if (pkg->crc_value) {
            crc32_to_string(crc_value, pkg->crc_value, 16);
            printf("✓ CRC32: %s\n", pkg->crc_value);
        }
    }

    /* Step 3: Encryption (if enabled) */
    if (pkg->encryption_enabled) {
        printf("🔒 Encrypting...\n");
        /* Encryption would go here - requires password from user */
    }

    /* Step 4: Hash Generation */
    if (pkg->hash_enabled) {
        printf("🔗 Generating hash...\n");
        const char *file_to_hash = pkg->compression_enabled ? pkg->compressed_file : pkg->name;

        if (hash_file(file_to_hash, pkg->hash_algorithm, &pkg->hash_value) != 0) {
            fprintf(stderr, "Hash generation failed\n");
            return -1;
        }

        /* Get file size */
        struct stat st;
        if (stat(file_to_hash, &st) == 0) {
            pkg->file_size = st.st_size;
        }

        printf("✓ SHA-256: %s\n", pkg->hash_value);
    }

    printf("✅ Firmware build completed!\n");
    return 0;
}

/**
 * Verify firmware integrity
 */
int fw_builder_verify(fw_package_t *pkg)
{
    if (!pkg) {
        fprintf(stderr, "Invalid package\n");
        return -1;
    }

    printf("🔍 Verifying firmware...\n");

    int all_valid = 1;

    /* Verify CRC if available */
    if (pkg->crc_enabled && pkg->crc_value) {
        const char *file_to_verify = pkg->compression_enabled ? pkg->compressed_file : pkg->name;
        uint32_t crc_value = crc32_file(file_to_verify);
        char crc_str[16];
        crc32_to_string(crc_value, crc_str, 16);

        if (strcmp(crc_str, pkg->crc_value) == 0) {
            printf("  ✅ CRC: Valid\n");
        } else {
            printf("  ❌ CRC: Invalid\n");
            all_valid = 0;
        }
    }

    /* Verify hash if available */
    if (pkg->hash_enabled && pkg->hash_value) {
        const char *file_to_verify = pkg->compression_enabled ? pkg->compressed_file : pkg->name;

        if (hash_verify(file_to_verify, pkg->hash_algorithm, pkg->hash_value)) {
            printf("  ✅ Hash: Valid\n");
        } else {
            printf("  ❌ Hash: Invalid\n");
            all_valid = 0;
        }
    }

    if (all_valid) {
        printf("✅ Firmware verified successfully!\n");
    } else {
        printf("❌ Firmware verification failed!\n");
    }

    return all_valid ? 0 : -1;
}

/**
 * Extract firmware package
 */
int fw_builder_extract(const char *firmware_file, const char *output_dir, const char *password)
{
    if (!firmware_file || !output_dir) {
        fprintf(stderr, "Invalid parameters\n");
        return -1;
    }

    printf("🔓 Extracting firmware...\n");

    /* Create output directory */
    mkdir(output_dir, 0755);

    /* If encrypted, decrypt first */
    char *current_file = (char *)firmware_file;
    char decrypted_file[512] = {0};

    if (password) {
        snprintf(decrypted_file, sizeof(decrypted_file), "%s/decrypted.bin", output_dir);
        if (decrypt_file(firmware_file, decrypted_file, password) != 0) {
            fprintf(stderr, "Decryption failed\n");
            return -1;
        }
        current_file = decrypted_file;
    }

    /* Decompress if needed */
    char decompressed_file[512] = {0};
    snprintf(decompressed_file, sizeof(decompressed_file), "%s/firmware.tar", output_dir);
    if (decompress_file(current_file, decompressed_file) != 0) {
        fprintf(stderr, "Decompression failed\n");
        return -1;
    }

    printf("✓ Firmware extracted to: %s\n", output_dir);
    return 0;
}

/**
 * Free firmware package structure
 */
void fw_builder_free(fw_package_t *pkg)
{
    if (!pkg) return;

    if (pkg->name) free(pkg->name);
    if (pkg->version) free(pkg->version);
    if (pkg->compression_algorithm) free(pkg->compression_algorithm);
    if (pkg->crc_algorithm) free(pkg->crc_algorithm);
    if (pkg->crc_value) free(pkg->crc_value);
    if (pkg->encryption_algorithm) free(pkg->encryption_algorithm);
    if (pkg->hash_algorithm) free(pkg->hash_algorithm);
    if (pkg->hash_value) free(pkg->hash_value);
    if (pkg->compressed_file) free(pkg->compressed_file);
    if (pkg->encrypted_file) free(pkg->encrypted_file);
    if (pkg->final_file) free(pkg->final_file);
    if (pkg->manifest_file) free(pkg->manifest_file);

    free(pkg);
}

/**
 * Print package information
 */
void fw_builder_print_package(fw_package_t *pkg)
{
    if (!pkg) return;

    printf("\n📋 Build Package Information:\n");
    printf("================================\n");
    printf("Name: %s\n", pkg->name);
    printf("Version: %s\n", pkg->version);
    printf("Timestamp: %ld\n", pkg->timestamp);

    if (pkg->compression_enabled) {
        printf("\nCompression:\n");
        printf("  Algorithm: %s\n", pkg->compression_algorithm);
        printf("  Level: %d\n", pkg->compression_level);
    }

    if (pkg->crc_enabled && pkg->crc_value) {
        printf("\nChecksum:\n");
        printf("  Algorithm: %s\n", pkg->crc_algorithm);
        printf("  Value: %s\n", pkg->crc_value);
    }

    if (pkg->encryption_enabled) {
        printf("\nEncryption:\n");
        printf("  Algorithm: %s\n", pkg->encryption_algorithm);
    }

    if (pkg->hash_enabled && pkg->hash_value) {
        printf("\nHash:\n");
        printf("  Algorithm: %s\n", pkg->hash_algorithm);
        printf("  Value: %s\n", pkg->hash_value);
        printf("  Size: %u bytes\n", pkg->file_size);
    }

    printf("\nFiles:\n");
    if (pkg->compression_enabled) {
        printf("  Compressed: %s\n", pkg->compressed_file);
    }
    if (pkg->encryption_enabled) {
        printf("  Encrypted: %s\n", pkg->encrypted_file);
    }
    printf("  Manifest: %s\n", pkg->manifest_file);
    printf("================================\n\n");
}
