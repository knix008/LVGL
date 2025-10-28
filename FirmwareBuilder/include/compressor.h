#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <stdint.h>
#include <stddef.h>

/* Compression options */
typedef struct {
    int level;          /* 0-9, 6 is default */
    int algorithm;      /* 0: gzip (default), 1: deflate */
} compress_options_t;

/* Compression API */
int compress_file(const char *input_file, const char *output_file, int level);
int compress_directory(const char *input_dir, const char *output_file, int level);
int decompress_file(const char *input_file, const char *output_file);
int compress_buffer(const uint8_t *input, size_t input_len, uint8_t **output, size_t *output_len, int level);
int decompress_buffer(const uint8_t *input, size_t input_len, uint8_t **output, size_t *output_len);

#endif /* COMPRESSOR_H */
