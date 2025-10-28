#ifndef CRC_H
#define CRC_H

#include <stdint.h>
#include <stddef.h>

/* CRC API */
uint32_t crc32_calculate(const uint8_t *data, size_t len);
uint32_t crc32_file(const char *filename);
int crc32_verify(const char *filename, uint32_t expected_crc);

uint16_t crc16_calculate(const uint8_t *data, size_t len);
uint16_t crc16_file(const char *filename);
int crc16_verify(const char *filename, uint16_t expected_crc);

uint8_t crc8_calculate(const uint8_t *data, size_t len);
uint8_t crc8_file(const char *filename);
int crc8_verify(const char *filename, uint8_t expected_crc);

/* Utility functions */
char* crc32_to_string(uint32_t crc, char *buffer, size_t size);
char* crc16_to_string(uint16_t crc, char *buffer, size_t size);
char* crc8_to_string(uint8_t crc, char *buffer, size_t size);

#endif /* CRC_H */
