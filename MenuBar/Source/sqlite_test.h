#ifndef SQLITE_TEST_H
#define SQLITE_TEST_H

#include <sqlite3.h>
#include <stddef.h>

// SQLite test functions
int sqlite_create_table_and_insert_data(char *result_buffer, size_t buffer_size);
int sqlite_query_and_display_data(char *result_buffer, size_t buffer_size);

// SQLite demonstration function (for console output)
void sqlite_demo(void);

#endif // SQLITE_TEST_H 