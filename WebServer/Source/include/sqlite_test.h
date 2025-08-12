#ifndef SQLITE_TEST_H
#define SQLITE_TEST_H

#include <stddef.h>

// SQLCipher test functions
int sqlite_create_table_and_insert_data(char *result_buffer, size_t buffer_size);
int sqlite_query_and_display_data(char *result_buffer, size_t buffer_size);
void sqlite_demo(void);

#endif // SQLITE_TEST_H 