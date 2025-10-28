#ifndef ENCRYPTOR_H
#define ENCRYPTOR_H

#include <stdint.h>
#include <stddef.h>

/* Encryption options */
typedef struct {
    char *algorithm;        /* "aes-256-cbc", "aes-192-cbc", "aes-128-cbc" */
    char *key_derivation;   /* "pbkdf2", "scrypt" */
    int iterations;         /* PBKDF2 iterations, default 100000 */
} encrypt_options_t;

/* Encrypted data structure */
typedef struct {
    uint8_t *iv;
    size_t iv_len;
    uint8_t *salt;
    size_t salt_len;
    uint8_t *ciphertext;
    size_t ciphertext_len;
    char *algorithm;
    int iterations;
} encrypted_data_t;

/* Encryption API */
int encrypt_file(const char *input_file, const char *output_file, const char *password, encrypt_options_t *options);
int decrypt_file(const char *input_file, const char *output_file, const char *password);

int encrypt_buffer(const uint8_t *plaintext, size_t plaintext_len, const char *password,
                   encrypted_data_t **encrypted, encrypt_options_t *options);
int decrypt_buffer(const encrypted_data_t *encrypted, const char *password,
                   uint8_t **plaintext, size_t *plaintext_len);

/* Utility functions */
void encrypted_data_free(encrypted_data_t *data);
int encrypted_data_to_json(const encrypted_data_t *data, char **json_str);
int json_to_encrypted_data(const char *json_str, encrypted_data_t **data);

#endif /* ENCRYPTOR_H */
