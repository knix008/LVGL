#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include "encryptor.h"

#define AES_KEY_SIZE 32         /* 256-bit key */
#define AES_IV_SIZE 16          /* 128-bit IV */
#define SALT_SIZE 32            /* 32-byte salt */
#define PBKDF2_ITERATIONS 100000
#define CHUNK_SIZE 262144

/**
 * Derive key from password using PBKDF2
 */
static int derive_key_pbkdf2(const char *password, const uint8_t *salt, size_t salt_len,
                            uint8_t *key, size_t key_len)
{
    int ret = PKCS5_PBKDF2_HMAC(password, strlen(password),
                                salt, salt_len,
                                PBKDF2_ITERATIONS,
                                EVP_sha256(),
                                key_len, key);
    return ret ? 0 : -1;
}

/**
 * Encrypt buffer with AES-256-CBC
 */
int encrypt_buffer(const uint8_t *plaintext, size_t plaintext_len, const char *password,
                   encrypted_data_t **encrypted, encrypt_options_t *options)
{
    (void)options;
    if (!plaintext || !password || !encrypted) {
        fprintf(stderr, "Invalid parameters\n");
        return -1;
    }

    /* Allocate encrypted data structure */
    *encrypted = (encrypted_data_t *)malloc(sizeof(encrypted_data_t));
    if (!*encrypted) {
        fprintf(stderr, "Memory allocation failed\n");
        return -1;
    }

    encrypted_data_t *enc = *encrypted;
    memset(enc, 0, sizeof(encrypted_data_t));

    /* Generate random IV and salt */
    enc->iv = (uint8_t *)malloc(AES_IV_SIZE);
    enc->salt = (uint8_t *)malloc(SALT_SIZE);
    if (!enc->iv || !enc->salt) {
        fprintf(stderr, "Memory allocation failed\n");
        encrypted_data_free(enc);
        return -1;
    }

    if (!RAND_bytes(enc->iv, AES_IV_SIZE) || !RAND_bytes(enc->salt, SALT_SIZE)) {
        fprintf(stderr, "Random generation failed\n");
        encrypted_data_free(enc);
        return -1;
    }

    enc->iv_len = AES_IV_SIZE;
    enc->salt_len = SALT_SIZE;

    /* Derive key from password */
    uint8_t key[AES_KEY_SIZE];
    if (derive_key_pbkdf2(password, enc->salt, enc->salt_len, key, AES_KEY_SIZE) != 0) {
        fprintf(stderr, "Key derivation failed\n");
        encrypted_data_free(enc);
        return -1;
    }

    /* Create cipher context */
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        fprintf(stderr, "Cipher context creation failed\n");
        encrypted_data_free(enc);
        return -1;
    }

    /* Initialize encryption */
    if (!EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, enc->iv)) {
        fprintf(stderr, "Encryption init failed\n");
        EVP_CIPHER_CTX_free(ctx);
        encrypted_data_free(enc);
        return -1;
    }

    /* Allocate output buffer (plaintext + block size for padding) */
    enc->ciphertext_len = plaintext_len + EVP_MAX_BLOCK_LENGTH;
    enc->ciphertext = (uint8_t *)malloc(enc->ciphertext_len);
    if (!enc->ciphertext) {
        fprintf(stderr, "Memory allocation failed\n");
        EVP_CIPHER_CTX_free(ctx);
        encrypted_data_free(enc);
        return -1;
    }

    /* Encrypt */
    int len = 0;
    if (!EVP_EncryptUpdate(ctx, enc->ciphertext, &len, plaintext, plaintext_len)) {
        fprintf(stderr, "Encryption failed\n");
        EVP_CIPHER_CTX_free(ctx);
        encrypted_data_free(enc);
        return -1;
    }

    int ciphertext_len = len;

    /* Finalize encryption */
    if (!EVP_EncryptFinal_ex(ctx, enc->ciphertext + len, &len)) {
        fprintf(stderr, "Encryption finalization failed\n");
        EVP_CIPHER_CTX_free(ctx);
        encrypted_data_free(enc);
        return -1;
    }

    ciphertext_len += len;
    enc->ciphertext_len = ciphertext_len;

    EVP_CIPHER_CTX_free(ctx);

    /* Set metadata */
    enc->algorithm = (char *)malloc(32);
    if (enc->algorithm) {
        strcpy(enc->algorithm, "aes-256-cbc");
    }
    enc->iterations = PBKDF2_ITERATIONS;

    return 0;
}

/**
 * Decrypt buffer with AES-256-CBC
 */
int decrypt_buffer(const encrypted_data_t *encrypted, const char *password,
                   uint8_t **plaintext, size_t *plaintext_len)
{
    if (!encrypted || !password || !plaintext) {
        fprintf(stderr, "Invalid parameters\n");
        return -1;
    }

    /* Derive key from password */
    uint8_t key[AES_KEY_SIZE];
    if (derive_key_pbkdf2(password, encrypted->salt, encrypted->salt_len, key, AES_KEY_SIZE) != 0) {
        fprintf(stderr, "Key derivation failed\n");
        return -1;
    }

    /* Create cipher context */
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        fprintf(stderr, "Cipher context creation failed\n");
        return -1;
    }

    /* Initialize decryption */
    if (!EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, encrypted->iv)) {
        fprintf(stderr, "Decryption init failed\n");
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }

    /* Allocate output buffer */
    *plaintext = (uint8_t *)malloc(encrypted->ciphertext_len + EVP_MAX_BLOCK_LENGTH);
    if (!*plaintext) {
        fprintf(stderr, "Memory allocation failed\n");
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }

    /* Decrypt */
    int len = 0;
    if (!EVP_DecryptUpdate(ctx, *plaintext, &len, encrypted->ciphertext, encrypted->ciphertext_len)) {
        fprintf(stderr, "Decryption failed\n");
        EVP_CIPHER_CTX_free(ctx);
        free(*plaintext);
        return -1;
    }

    int result_len = len;

    /* Finalize decryption */
    if (!EVP_DecryptFinal_ex(ctx, *plaintext + len, &len)) {
        fprintf(stderr, "Decryption finalization failed\n");
        EVP_CIPHER_CTX_free(ctx);
        free(*plaintext);
        return -1;
    }

    result_len += len;
    *plaintext_len = result_len;

    EVP_CIPHER_CTX_free(ctx);

    return 0;
}

/**
 * Encrypt file with AES-256-CBC
 */
int encrypt_file(const char *input_file, const char *output_file, const char *password, encrypt_options_t *options)
{
    (void)options;
    FILE *in = NULL, *out = NULL;
    uint8_t in_buf[CHUNK_SIZE];
    uint8_t out_buf[CHUNK_SIZE + EVP_MAX_BLOCK_LENGTH];
    size_t bytes_read;

    in = fopen(input_file, "rb");
    if (!in) {
        perror("Cannot open input file");
        return -1;
    }

    out = fopen(output_file, "wb");
    if (!out) {
        perror("Cannot open output file");
        fclose(in);
        return -1;
    }

    /* Generate random IV and salt */
    uint8_t iv[AES_IV_SIZE];
    uint8_t salt[SALT_SIZE];

    if (!RAND_bytes(iv, AES_IV_SIZE) || !RAND_bytes(salt, SALT_SIZE)) {
        fprintf(stderr, "Random generation failed\n");
        fclose(in);
        fclose(out);
        return -1;
    }

    /* Write IV and salt to output file */
    if (fwrite(iv, 1, AES_IV_SIZE, out) != AES_IV_SIZE ||
        fwrite(salt, 1, SALT_SIZE, out) != SALT_SIZE) {
        fprintf(stderr, "Write error\n");
        fclose(in);
        fclose(out);
        return -1;
    }

    /* Derive key from password */
    uint8_t key[AES_KEY_SIZE];
    if (derive_key_pbkdf2(password, salt, SALT_SIZE, key, AES_KEY_SIZE) != 0) {
        fprintf(stderr, "Key derivation failed\n");
        fclose(in);
        fclose(out);
        return -1;
    }

    /* Create cipher context */
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        fprintf(stderr, "Cipher context creation failed\n");
        fclose(in);
        fclose(out);
        return -1;
    }

    /* Initialize encryption */
    if (!EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
        fprintf(stderr, "Encryption init failed\n");
        EVP_CIPHER_CTX_free(ctx);
        fclose(in);
        fclose(out);
        return -1;
    }

    /* Encrypt file */
    while ((bytes_read = fread(in_buf, 1, CHUNK_SIZE, in)) > 0) {
        int len = 0;
        if (!EVP_EncryptUpdate(ctx, out_buf, &len, in_buf, bytes_read)) {
            fprintf(stderr, "Encryption failed\n");
            EVP_CIPHER_CTX_free(ctx);
            fclose(in);
            fclose(out);
            return -1;
        }

        if (fwrite(out_buf, 1, len, out) != (size_t)len) {
            fprintf(stderr, "Write error\n");
            EVP_CIPHER_CTX_free(ctx);
            fclose(in);
            fclose(out);
            return -1;
        }
    }

    /* Finalize encryption */
    int len = 0;
    if (!EVP_EncryptFinal_ex(ctx, out_buf, &len)) {
        fprintf(stderr, "Encryption finalization failed\n");
        EVP_CIPHER_CTX_free(ctx);
        fclose(in);
        fclose(out);
        return -1;
    }

    if (len > 0 && fwrite(out_buf, 1, len, out) != (size_t)len) {
        fprintf(stderr, "Write error\n");
        EVP_CIPHER_CTX_free(ctx);
        fclose(in);
        fclose(out);
        return -1;
    }

    EVP_CIPHER_CTX_free(ctx);
    fclose(in);
    fclose(out);

    printf("✓ File encrypted: %s\n", output_file);
    return 0;
}

/**
 * Decrypt file with AES-256-CBC
 */
int decrypt_file(const char *input_file, const char *output_file, const char *password)
{
    FILE *in = NULL, *out = NULL;
    uint8_t iv[AES_IV_SIZE];
    uint8_t salt[SALT_SIZE];
    uint8_t in_buf[CHUNK_SIZE];
    uint8_t out_buf[CHUNK_SIZE + EVP_MAX_BLOCK_LENGTH];
    size_t bytes_read;

    in = fopen(input_file, "rb");
    if (!in) {
        perror("Cannot open input file");
        return -1;
    }

    out = fopen(output_file, "wb");
    if (!out) {
        perror("Cannot open output file");
        fclose(in);
        return -1;
    }

    /* Read IV and salt from file */
    if (fread(iv, 1, AES_IV_SIZE, in) != AES_IV_SIZE ||
        fread(salt, 1, SALT_SIZE, in) != SALT_SIZE) {
        fprintf(stderr, "Read error\n");
        fclose(in);
        fclose(out);
        return -1;
    }

    /* Derive key from password */
    uint8_t key[AES_KEY_SIZE];
    if (derive_key_pbkdf2(password, salt, SALT_SIZE, key, AES_KEY_SIZE) != 0) {
        fprintf(stderr, "Key derivation failed\n");
        fclose(in);
        fclose(out);
        return -1;
    }

    /* Create cipher context */
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        fprintf(stderr, "Cipher context creation failed\n");
        fclose(in);
        fclose(out);
        return -1;
    }

    /* Initialize decryption */
    if (!EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
        fprintf(stderr, "Decryption init failed\n");
        EVP_CIPHER_CTX_free(ctx);
        fclose(in);
        fclose(out);
        return -1;
    }

    /* Decrypt file */
    while ((bytes_read = fread(in_buf, 1, CHUNK_SIZE, in)) > 0) {
        int len = 0;
        if (!EVP_DecryptUpdate(ctx, out_buf, &len, in_buf, bytes_read)) {
            fprintf(stderr, "Decryption failed\n");
            EVP_CIPHER_CTX_free(ctx);
            fclose(in);
            fclose(out);
            return -1;
        }

        if (fwrite(out_buf, 1, len, out) != (size_t)len) {
            fprintf(stderr, "Write error\n");
            EVP_CIPHER_CTX_free(ctx);
            fclose(in);
            fclose(out);
            return -1;
        }
    }

    /* Finalize decryption */
    int len = 0;
    if (!EVP_DecryptFinal_ex(ctx, out_buf, &len)) {
        fprintf(stderr, "Decryption finalization failed\n");
        EVP_CIPHER_CTX_free(ctx);
        fclose(in);
        fclose(out);
        return -1;
    }

    if (len > 0 && fwrite(out_buf, 1, len, out) != (size_t)len) {
        fprintf(stderr, "Write error\n");
        EVP_CIPHER_CTX_free(ctx);
        fclose(in);
        fclose(out);
        return -1;
    }

    EVP_CIPHER_CTX_free(ctx);
    fclose(in);
    fclose(out);

    printf("✓ File decrypted: %s\n", output_file);
    return 0;
}

/**
 * Free encrypted data structure
 */
void encrypted_data_free(encrypted_data_t *data)
{
    if (!data) return;

    if (data->iv) free(data->iv);
    if (data->salt) free(data->salt);
    if (data->ciphertext) free(data->ciphertext);
    if (data->algorithm) free(data->algorithm);

    free(data);
}

/**
 * Convert encrypted data to JSON (stub implementation)
 */
int encrypted_data_to_json(const encrypted_data_t *data, char **json_str)
{
    /* This would require a JSON library - simplified version */
    (void)data;
    (void)json_str;
    return -1;
}

/**
 * Convert JSON to encrypted data (stub implementation)
 */
int json_to_encrypted_data(const char *json_str, encrypted_data_t **data)
{
    /* This would require a JSON library - simplified version */
    (void)json_str;
    (void)data;
    return -1;
}
