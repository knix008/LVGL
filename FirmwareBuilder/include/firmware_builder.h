#ifndef FIRMWARE_BUILDER_H
#define FIRMWARE_BUILDER_H

#include <stdint.h>
#include <time.h>

/* Build options structure */
typedef struct {
    int compress;
    int compression_level;
    int add_crc;
    int encrypt;
    char *encryption_password;
    int generate_hash;
} fw_build_options_t;

/* CRC Algorithm enum */
typedef enum {
    CRC32,
    CRC16,
    CRC8
} crc_algorithm_t;

/* Hash Algorithm enum */
typedef enum {
    HASH_SHA256,
    HASH_SHA512,
    HASH_MD5
} hash_algorithm_t;

/* Firmware package structure */
typedef struct {
    char *name;
    char *version;
    time_t timestamp;

    /* Compression info */
    int compression_enabled;
    char *compression_algorithm;
    int compression_level;

    /* CRC info */
    int crc_enabled;
    char *crc_algorithm;
    char *crc_value;

    /* Encryption info */
    int encryption_enabled;
    char *encryption_algorithm;

    /* Hash info */
    int hash_enabled;
    char *hash_algorithm;
    char *hash_value;
    uint32_t file_size;

    /* Output files */
    char *compressed_file;
    char *encrypted_file;
    char *final_file;
    char *manifest_file;
} fw_package_t;

/* Firmware Builder API */
fw_package_t* fw_builder_create(const char *source, const char *output_dir, fw_build_options_t options);
int fw_builder_build(fw_package_t *pkg);
int fw_builder_verify(fw_package_t *pkg);
int fw_builder_extract(const char *firmware_file, const char *output_dir, const char *password);
void fw_builder_free(fw_package_t *pkg);
void fw_builder_print_package(fw_package_t *pkg);

#endif /* FIRMWARE_BUILDER_H */
