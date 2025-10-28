#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crc.h"

#define CHUNK_SIZE 262144  /* 256KB */

/* CRC32 lookup table */
static uint32_t crc32_table[256];
static int crc32_table_initialized = 0;

/* CRC16 lookup table */
static uint16_t crc16_table[256];
static int crc16_table_initialized = 0;

/* CRC8 lookup table */
static uint8_t crc8_table[256];
static int crc8_table_initialized = 0;

/**
 * Initialize CRC32 lookup table
 */
static void crc32_init_table(void)
{
    if (crc32_table_initialized) return;

    uint32_t poly = 0xedb88320;
    for (int i = 0; i < 256; i++) {
        uint32_t crc = i;
        for (int j = 0; j < 8; j++) {
            crc = (crc & 1) ? (crc >> 1) ^ poly : crc >> 1;
        }
        crc32_table[i] = crc;
    }
    crc32_table_initialized = 1;
}

/**
 * Initialize CRC16 lookup table (CCITT variant)
 */
static void crc16_init_table(void)
{
    if (crc16_table_initialized) return;

    uint16_t poly = 0x1021;
    for (int i = 0; i < 256; i++) {
        uint16_t crc = (uint16_t)i << 8;
        for (int j = 0; j < 8; j++) {
            crc = (crc << 1) ^ ((crc & 0x8000) ? poly : 0);
        }
        crc16_table[i] = crc;
    }
    crc16_table_initialized = 1;
}

/**
 * Initialize CRC8 lookup table
 */
static void crc8_init_table(void)
{
    if (crc8_table_initialized) return;

    uint8_t poly = 0x07;
    for (int i = 0; i < 256; i++) {
        uint8_t crc = i;
        for (int j = 0; j < 8; j++) {
            crc = (crc << 1) ^ ((crc & 0x80) ? poly : 0);
        }
        crc8_table[i] = crc;
    }
    crc8_table_initialized = 1;
}

/**
 * Calculate CRC32 for buffer
 */
uint32_t crc32_calculate(const uint8_t *data, size_t len)
{
    crc32_init_table();

    uint32_t crc = 0xffffffff;
    for (size_t i = 0; i < len; i++) {
        uint8_t byte = data[i];
        uint32_t idx = (crc ^ byte) & 0xff;
        crc = (crc >> 8) ^ crc32_table[idx];
    }
    return crc ^ 0xffffffff;
}

/**
 * Calculate CRC32 for file
 */
uint32_t crc32_file(const char *filename)
{
    FILE *f = fopen(filename, "rb");
    if (!f) {
        perror("Cannot open file");
        return 0;
    }

    uint8_t buf[CHUNK_SIZE];
    uint32_t crc = 0xffffffff;
    size_t bytes;

    crc32_init_table();

    while ((bytes = fread(buf, 1, CHUNK_SIZE, f)) > 0) {
        for (size_t i = 0; i < bytes; i++) {
            uint8_t byte = buf[i];
            uint32_t idx = (crc ^ byte) & 0xff;
            crc = (crc >> 8) ^ crc32_table[idx];
        }
    }

    fclose(f);
    return crc ^ 0xffffffff;
}

/**
 * Verify CRC32
 */
int crc32_verify(const char *filename, uint32_t expected_crc)
{
    uint32_t calculated = crc32_file(filename);
    return calculated == expected_crc;
}

/**
 * Calculate CRC16 for buffer
 */
uint16_t crc16_calculate(const uint8_t *data, size_t len)
{
    crc16_init_table();

    uint16_t crc = 0xffff;
    for (size_t i = 0; i < len; i++) {
        uint8_t byte = data[i];
        uint16_t idx = ((crc >> 8) ^ byte) & 0xff;
        crc = ((crc << 8) ^ crc16_table[idx]) & 0xffff;
    }
    return crc;
}

/**
 * Calculate CRC16 for file
 */
uint16_t crc16_file(const char *filename)
{
    FILE *f = fopen(filename, "rb");
    if (!f) {
        perror("Cannot open file");
        return 0;
    }

    uint8_t buf[CHUNK_SIZE];
    uint16_t crc = 0xffff;
    size_t bytes;

    crc16_init_table();

    while ((bytes = fread(buf, 1, CHUNK_SIZE, f)) > 0) {
        for (size_t i = 0; i < bytes; i++) {
            uint8_t byte = buf[i];
            uint16_t idx = ((crc >> 8) ^ byte) & 0xff;
            crc = ((crc << 8) ^ crc16_table[idx]) & 0xffff;
        }
    }

    fclose(f);
    return crc;
}

/**
 * Verify CRC16
 */
int crc16_verify(const char *filename, uint16_t expected_crc)
{
    uint16_t calculated = crc16_file(filename);
    return calculated == expected_crc;
}

/**
 * Calculate CRC8 for buffer
 */
uint8_t crc8_calculate(const uint8_t *data, size_t len)
{
    crc8_init_table();

    uint8_t crc = 0;
    for (size_t i = 0; i < len; i++) {
        uint8_t byte = data[i];
        crc = crc8_table[crc ^ byte];
    }
    return crc;
}

/**
 * Calculate CRC8 for file
 */
uint8_t crc8_file(const char *filename)
{
    FILE *f = fopen(filename, "rb");
    if (!f) {
        perror("Cannot open file");
        return 0;
    }

    uint8_t buf[CHUNK_SIZE];
    uint8_t crc = 0;
    size_t bytes;

    crc8_init_table();

    while ((bytes = fread(buf, 1, CHUNK_SIZE, f)) > 0) {
        for (size_t i = 0; i < bytes; i++) {
            crc = crc8_table[crc ^ buf[i]];
        }
    }

    fclose(f);
    return crc;
}

/**
 * Verify CRC8
 */
int crc8_verify(const char *filename, uint8_t expected_crc)
{
    uint8_t calculated = crc8_file(filename);
    return calculated == expected_crc;
}

/**
 * Convert CRC32 to hex string
 */
char* crc32_to_string(uint32_t crc, char *buffer, size_t size)
{
    if (size < 9) return NULL;
    snprintf(buffer, size, "%08X", crc);
    return buffer;
}

/**
 * Convert CRC16 to hex string
 */
char* crc16_to_string(uint16_t crc, char *buffer, size_t size)
{
    if (size < 5) return NULL;
    snprintf(buffer, size, "%04X", crc);
    return buffer;
}

/**
 * Convert CRC8 to hex string
 */
char* crc8_to_string(uint8_t crc, char *buffer, size_t size)
{
    if (size < 3) return NULL;
    snprintf(buffer, size, "%02X", crc);
    return buffer;
}
