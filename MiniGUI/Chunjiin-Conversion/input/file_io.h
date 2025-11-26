#ifndef FILE_IO_H
#define FILE_IO_H

#include "chunjiin.h"

// Save input text to file
int file_save_input(const ChunjiinState *state, const char *filename);

// Load input text from file
int file_load_input(ChunjiinState *state, const char *filename);

#endif // FILE_IO_H
