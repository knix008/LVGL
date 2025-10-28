#ifndef HASHER_H
#define HASHER_H

#include <stdint.h>
#include <stddef.h>

/* Hash result structure */
typedef struct {
    char *algorithm;
    char *hash_value;
    uint32_t file_size;
} hash_result_t;

/* Hash API */
int hash_file(const char *filename, const char *algorithm, char **hash_str);
int hash_buffer(const uint8_t *data, size_t len, const char *algorithm, char **hash_str);
int hash_directory(const char *dirname, const char *algorithm, char **hash_str);

int hash_verify(const char *filename, const char *algorithm, const char *expected_hash);

/* Multiple hashes */
typedef struct {
    char *sha256;
    char *sha512;
    char *md5;
} multi_hash_t;

int hash_file_multi(const char *filename, multi_hash_t **hashes);

/* Manifest generation */
int generate_manifest(const char *filename, char **manifest_json);

/* Utility functions */
void hash_result_free(hash_result_t *result);
void multi_hash_free(multi_hash_t *hashes);

#endif /* HASHER_H */
