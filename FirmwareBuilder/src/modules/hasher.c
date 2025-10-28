#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <openssl/md5.h>
#include <dirent.h>
#include <sys/stat.h>
#include "hasher.h"

/* Suppress OpenSSL 3.0 deprecation warnings for SHA256, SHA512, MD5 */
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#define CHUNK_SIZE 262144  /* 256KB */

/**
 * Convert binary hash to hex string
 */
static void bin_to_hex(const unsigned char *bin, size_t len, char *hex)
{
    for (size_t i = 0; i < len; i++) {
        sprintf(hex + (i * 2), "%02x", bin[i]);
    }
    hex[len * 2] = '\0';
}

/**
 * Hash file with SHA-256
 */
static int hash_file_sha256(const char *filename, char **hash_str)
{
    FILE *f = fopen(filename, "rb");
    if (!f) {
        perror("Cannot open file");
        return -1;
    }

    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    uint8_t buf[CHUNK_SIZE];
    size_t bytes;

    while ((bytes = fread(buf, 1, CHUNK_SIZE, f)) > 0) {
        SHA256_Update(&sha256, buf, bytes);
    }

    fclose(f);

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_Final(hash, &sha256);

    *hash_str = (char *)malloc(SHA256_DIGEST_LENGTH * 2 + 1);
    if (!*hash_str) {
        fprintf(stderr, "Memory allocation failed\n");
        return -1;
    }

    bin_to_hex(hash, SHA256_DIGEST_LENGTH, *hash_str);
    return 0;
}

/**
 * Hash file with SHA-512
 */
static int hash_file_sha512(const char *filename, char **hash_str)
{
    FILE *f = fopen(filename, "rb");
    if (!f) {
        perror("Cannot open file");
        return -1;
    }

    SHA512_CTX sha512;
    SHA512_Init(&sha512);

    uint8_t buf[CHUNK_SIZE];
    size_t bytes;

    while ((bytes = fread(buf, 1, CHUNK_SIZE, f)) > 0) {
        SHA512_Update(&sha512, buf, bytes);
    }

    fclose(f);

    unsigned char hash[SHA512_DIGEST_LENGTH];
    SHA512_Final(hash, &sha512);

    *hash_str = (char *)malloc(SHA512_DIGEST_LENGTH * 2 + 1);
    if (!*hash_str) {
        fprintf(stderr, "Memory allocation failed\n");
        return -1;
    }

    bin_to_hex(hash, SHA512_DIGEST_LENGTH, *hash_str);
    return 0;
}

/**
 * Hash file with MD5
 */
static int hash_file_md5(const char *filename, char **hash_str)
{
    FILE *f = fopen(filename, "rb");
    if (!f) {
        perror("Cannot open file");
        return -1;
    }

    MD5_CTX md5;
    MD5_Init(&md5);

    uint8_t buf[CHUNK_SIZE];
    size_t bytes;

    while ((bytes = fread(buf, 1, CHUNK_SIZE, f)) > 0) {
        MD5_Update(&md5, buf, bytes);
    }

    fclose(f);

    unsigned char hash[MD5_DIGEST_LENGTH];
    MD5_Final(hash, &md5);

    *hash_str = (char *)malloc(MD5_DIGEST_LENGTH * 2 + 1);
    if (!*hash_str) {
        fprintf(stderr, "Memory allocation failed\n");
        return -1;
    }

    bin_to_hex(hash, MD5_DIGEST_LENGTH, *hash_str);
    return 0;
}

/**
 * Hash buffer with SHA-256
 */
static int hash_buffer_sha256(const uint8_t *data, size_t len, char **hash_str)
{
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data, len);

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_Final(hash, &sha256);

    *hash_str = (char *)malloc(SHA256_DIGEST_LENGTH * 2 + 1);
    if (!*hash_str) {
        fprintf(stderr, "Memory allocation failed\n");
        return -1;
    }

    bin_to_hex(hash, SHA256_DIGEST_LENGTH, *hash_str);
    return 0;
}

/**
 * Hash a file
 */
int hash_file(const char *filename, const char *algorithm, char **hash_str)
{
    if (!filename || !algorithm || !hash_str) {
        fprintf(stderr, "Invalid parameters\n");
        return -1;
    }

    if (strcmp(algorithm, "sha256") == 0) {
        return hash_file_sha256(filename, hash_str);
    } else if (strcmp(algorithm, "sha512") == 0) {
        return hash_file_sha512(filename, hash_str);
    } else if (strcmp(algorithm, "md5") == 0) {
        return hash_file_md5(filename, hash_str);
    } else {
        fprintf(stderr, "Unsupported hash algorithm: %s\n", algorithm);
        return -1;
    }
}

/**
 * Hash a buffer
 */
int hash_buffer(const uint8_t *data, size_t len, const char *algorithm, char **hash_str)
{
    if (!data || !algorithm || !hash_str) {
        fprintf(stderr, "Invalid parameters\n");
        return -1;
    }

    if (strcmp(algorithm, "sha256") == 0) {
        return hash_buffer_sha256(data, len, hash_str);
    } else {
        fprintf(stderr, "Unsupported hash algorithm: %s\n", algorithm);
        return -1;
    }
}

/**
 * Hash a directory (simplified - hashes directory name and file list)
 */
int hash_directory(const char *dirname, const char *algorithm, char **hash_str)
{
    if (!dirname || !algorithm || !hash_str) {
        fprintf(stderr, "Invalid parameters\n");
        return -1;
    }

    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    DIR *dir = opendir(dirname);
    if (!dir) {
        perror("Cannot open directory");
        return -1;
    }

    /* Hash directory name */
    SHA256_Update(&sha256, dirname, strlen(dirname));

    /* Hash each file in directory */
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        SHA256_Update(&sha256, entry->d_name, strlen(entry->d_name));
    }

    closedir(dir);

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_Final(hash, &sha256);

    *hash_str = (char *)malloc(SHA256_DIGEST_LENGTH * 2 + 1);
    if (!*hash_str) {
        fprintf(stderr, "Memory allocation failed\n");
        return -1;
    }

    bin_to_hex(hash, SHA256_DIGEST_LENGTH, *hash_str);
    return 0;
}

/**
 * Verify file hash
 */
int hash_verify(const char *filename, const char *algorithm, const char *expected_hash)
{
    if (!filename || !algorithm || !expected_hash) {
        fprintf(stderr, "Invalid parameters\n");
        return -1;
    }

    char *calculated = NULL;
    if (hash_file(filename, algorithm, &calculated) != 0) {
        return -1;
    }

    int result = strcmp(calculated, expected_hash) == 0 ? 1 : 0;
    free(calculated);

    return result;
}

/**
 * Hash file with multiple algorithms
 */
int hash_file_multi(const char *filename, multi_hash_t **hashes)
{
    if (!filename || !hashes) {
        fprintf(stderr, "Invalid parameters\n");
        return -1;
    }

    *hashes = (multi_hash_t *)malloc(sizeof(multi_hash_t));
    if (!*hashes) {
        fprintf(stderr, "Memory allocation failed\n");
        return -1;
    }

    memset(*hashes, 0, sizeof(multi_hash_t));

    /* Calculate SHA-256 */
    if (hash_file_sha256(filename, &(*hashes)->sha256) != 0) {
        return -1;
    }

    /* Calculate SHA-512 */
    if (hash_file_sha512(filename, &(*hashes)->sha512) != 0) {
        free((*hashes)->sha256);
        return -1;
    }

    /* Calculate MD5 */
    if (hash_file_md5(filename, &(*hashes)->md5) != 0) {
        free((*hashes)->sha256);
        free((*hashes)->sha512);
        return -1;
    }

    return 0;
}

/**
 * Generate manifest (simplified JSON-like format)
 */
int generate_manifest(const char *filename, char **manifest_json)
{
    if (!filename || !manifest_json) {
        fprintf(stderr, "Invalid parameters\n");
        return -1;
    }

    multi_hash_t *hashes = NULL;
    if (hash_file_multi(filename, &hashes) != 0) {
        return -1;
    }

    /* Get file size */
    struct stat st;
    if (stat(filename, &st) != 0) {
        perror("Cannot stat file");
        multi_hash_free(hashes);
        return -1;
    }

    /* Create simple JSON manifest */
    size_t json_size = 2048;
    *manifest_json = (char *)malloc(json_size);
    if (!*manifest_json) {
        fprintf(stderr, "Memory allocation failed\n");
        multi_hash_free(hashes);
        return -1;
    }

    snprintf(*manifest_json, json_size,
        "{\n"
        "  \"file\": \"%s\",\n"
        "  \"size\": %ld,\n"
        "  \"hashes\": {\n"
        "    \"sha256\": \"%s\",\n"
        "    \"sha512\": \"%s\",\n"
        "    \"md5\": \"%s\"\n"
        "  }\n"
        "}",
        filename, st.st_size, hashes->sha256, hashes->sha512, hashes->md5);

    multi_hash_free(hashes);
    return 0;
}

/**
 * Free hash result
 */
void hash_result_free(hash_result_t *result)
{
    if (!result) return;
    if (result->algorithm) free(result->algorithm);
    if (result->hash_value) free(result->hash_value);
    free(result);
}

/**
 * Free multi hash
 */
void multi_hash_free(multi_hash_t *hashes)
{
    if (!hashes) return;
    if (hashes->sha256) free(hashes->sha256);
    if (hashes->sha512) free(hashes->sha512);
    if (hashes->md5) free(hashes->md5);
    free(hashes);
}
