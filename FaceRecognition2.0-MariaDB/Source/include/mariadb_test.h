#ifndef MARIADB_TEST_H
#define MARIADB_TEST_H

#include <stddef.h>

// MariaDB test functions
int mariadb_create_table_and_insert_data(char *result_buffer, size_t buffer_size);
int mariadb_query_and_display_data(char *result_buffer, size_t buffer_size);
void mariadb_demo(void);

// MariaDB connection management
int mariadb_connect_db(const char *host, const char *user, const char *password, const char *database, int port);
void mariadb_disconnect(void);

// MariaDB utility functions
int mariadb_execute_query(const char *query, char *result_buffer, size_t buffer_size);
int mariadb_create_database(const char *database_name);

#endif // MARIADB_TEST_H
