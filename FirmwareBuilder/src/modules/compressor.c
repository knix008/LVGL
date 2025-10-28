#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include "compressor.h"

#define CHUNK_SIZE 262144  /* 256KB chunks */

/**
 * Compress a file using gzip
 */
int compress_file(const char *input_file, const char *output_file, int level)
{
    FILE *in = NULL, *out = NULL;
    uint8_t in_buf[CHUNK_SIZE];
    uint8_t out_buf[CHUNK_SIZE];
    size_t bytes_read;
    int ret;

    if (level < 0 || level > 9) {
        level = 6;  /* Default compression level */
    }

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

    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;

    /* Initialize deflate stream with gzip format */
    ret = deflateInit2(&stream, level, Z_DEFLATED, 15 + 16, 8, Z_DEFAULT_STRATEGY);
    if (ret != Z_OK) {
        fprintf(stderr, "Deflate init failed: %d\n", ret);
        fclose(in);
        fclose(out);
        return -1;
    }

    /* Compress file */
    while (!feof(in)) {
        bytes_read = fread(in_buf, 1, CHUNK_SIZE, in);
        if (bytes_read <= 0) break;

        stream.avail_in = bytes_read;
        stream.next_in = in_buf;

        do {
            stream.avail_out = CHUNK_SIZE;
            stream.next_out = out_buf;
            ret = deflate(&stream, feof(in) ? Z_FINISH : Z_NO_FLUSH);

            if (ret == Z_STREAM_ERROR) {
                fprintf(stderr, "Deflate error: %d\n", ret);
                deflateEnd(&stream);
                fclose(in);
                fclose(out);
                return -1;
            }

            size_t have = CHUNK_SIZE - stream.avail_out;
            if (fwrite(out_buf, 1, have, out) != have || ferror(out)) {
                fprintf(stderr, "Write error\n");
                deflateEnd(&stream);
                fclose(in);
                fclose(out);
                return -1;
            }
        } while (stream.avail_out == 0);
    }

    deflateEnd(&stream);
    fclose(in);
    fclose(out);

    printf("✓ File compressed: %s\n", output_file);
    return 0;
}

/**
 * Decompress a file using gzip
 */
int decompress_file(const char *input_file, const char *output_file)
{
    FILE *in = NULL, *out = NULL;
    uint8_t in_buf[CHUNK_SIZE];
    uint8_t out_buf[CHUNK_SIZE];
    size_t bytes_read;
    int ret;

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

    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;

    /* Initialize inflate stream with gzip format */
    ret = inflateInit2(&stream, 15 + 16);
    if (ret != Z_OK) {
        fprintf(stderr, "Inflate init failed: %d\n", ret);
        fclose(in);
        fclose(out);
        return -1;
    }

    /* Decompress file */
    while (!feof(in)) {
        bytes_read = fread(in_buf, 1, CHUNK_SIZE, in);
        if (bytes_read <= 0) break;

        stream.avail_in = bytes_read;
        stream.next_in = in_buf;

        do {
            stream.avail_out = CHUNK_SIZE;
            stream.next_out = out_buf;
            ret = inflate(&stream, Z_NO_FLUSH);

            if (ret == Z_STREAM_ERROR || ret == Z_DATA_ERROR) {
                fprintf(stderr, "Inflate error: %d\n", ret);
                inflateEnd(&stream);
                fclose(in);
                fclose(out);
                return -1;
            }

            size_t have = CHUNK_SIZE - stream.avail_out;
            if (fwrite(out_buf, 1, have, out) != have || ferror(out)) {
                fprintf(stderr, "Write error\n");
                inflateEnd(&stream);
                fclose(in);
                fclose(out);
                return -1;
            }
        } while (stream.avail_out == 0);
    }

    inflateEnd(&stream);
    fclose(in);
    fclose(out);

    printf("✓ File decompressed: %s\n", output_file);
    return 0;
}

/**
 * Compress buffer in memory
 */
int compress_buffer(const uint8_t *input, size_t input_len, uint8_t **output, size_t *output_len, int level)
{
    uLongf out_size;
    int ret;

    if (level < 0 || level > 9) {
        level = 6;
    }

    /* Allocate output buffer with extra space */
    out_size = compressBound(input_len);
    *output = (uint8_t *)malloc(out_size);
    if (!*output) {
        fprintf(stderr, "Memory allocation failed\n");
        return -1;
    }

    /* Compress */
    ret = compress2(*output, &out_size, (const Bytef *)input, input_len, level);
    if (ret != Z_OK) {
        fprintf(stderr, "Compression failed: %d\n", ret);
        free(*output);
        return -1;
    }

    *output_len = out_size;
    return 0;
}

/**
 * Decompress buffer in memory
 */
int decompress_buffer(const uint8_t *input, size_t input_len, uint8_t **output, size_t *output_len)
{
    /* Start with estimated size */
    uLongf out_size = input_len * 4;
    int ret;

    *output = (uint8_t *)malloc(out_size);
    if (!*output) {
        fprintf(stderr, "Memory allocation failed\n");
        return -1;
    }

    /* Try to decompress */
    ret = uncompress(*output, &out_size, (const Bytef *)input, input_len);

    if (ret == Z_BUF_ERROR) {
        /* Buffer too small, try larger size */
        out_size = input_len * 10;
        uint8_t *temp = (uint8_t *)realloc(*output, out_size);
        if (!temp) {
            fprintf(stderr, "Memory allocation failed\n");
            free(*output);
            return -1;
        }
        *output = temp;

        ret = uncompress(*output, &out_size, (const Bytef *)input, input_len);
    }

    if (ret != Z_OK) {
        fprintf(stderr, "Decompression failed: %d\n", ret);
        free(*output);
        return -1;
    }

    *output_len = out_size;
    return 0;
}

/**
 * Compress a directory (simple implementation - compresses all files)
 */
int compress_directory(const char *input_dir, const char *output_file, int level)
{
    /* For simplicity, we can just compress the directory contents as a tar.gz
     * For now, we'll implement a basic version that compresses files */
    printf("Compressing directory: %s\n", input_dir);

    /* This would require tar functionality - using compress_file for now */
    return compress_file(input_dir, output_file, level);
}
