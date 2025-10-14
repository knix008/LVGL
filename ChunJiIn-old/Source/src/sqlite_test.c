#include "sqlite_test.h"
#include "sqlite3.h"
#include <stdio.h>
#include <string.h>

// Callback function for SQLite query results (console output)
static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    NotUsed = 0;
    printf("Query result:\n");
    for (int i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

// SQLCipher test function for creating encrypted table and inserting data
int sqlite_create_table_and_insert_data(char *result_buffer, size_t buffer_size) {
    sqlite3 *db;
    char *err_msg = 0;
    int rc = sqlite3_open(":memory:", &db);
    
    if (rc != SQLITE_OK) {
        snprintf(result_buffer, buffer_size, "Database Error!");
        return -1;
    }
    
    // Set encryption key for SQLCipher
    const char *key = "my_secret_key_123";
    rc = sqlite3_key(db, key, strlen(key));
    if (rc != SQLITE_OK) {
        snprintf(result_buffer, buffer_size, "Failed to set encryption key!");
        sqlite3_close(db);
        return -1;
    }
    
    // Create table
    const char *sql = "CREATE TABLE IF NOT EXISTS users ("
                     "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                     "name TEXT NOT NULL,"
                     "email TEXT);";
    
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        snprintf(result_buffer, buffer_size, "Table creation failed!");
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return -1;
    }
    
    // Insert data
    sql = "INSERT INTO users (name, email) VALUES ('John Doe', 'john@example.com');"
          "INSERT INTO users (name, email) VALUES ('Jane Smith', 'jane@example.com');";
    
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        snprintf(result_buffer, buffer_size, "Data insertion failed!");
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return -1;
    }
    
    snprintf(result_buffer, buffer_size, "Encrypted table created & data inserted!");
    sqlite3_close(db);
    return 0;
}

// SQLCipher test function for querying and displaying encrypted data
int sqlite_query_and_display_data(char *result_buffer, size_t buffer_size) {
    sqlite3 *db;
    char *err_msg = 0;
    int rc = sqlite3_open(":memory:", &db);
    
    if (rc != SQLITE_OK) {
        snprintf(result_buffer, buffer_size, "Database Error!");
        return -1;
    }
    
    // Set encryption key for SQLCipher
    const char *key = "my_secret_key_123";
    rc = sqlite3_key(db, key, strlen(key));
    if (rc != SQLITE_OK) {
        snprintf(result_buffer, buffer_size, "Failed to set encryption key!");
        sqlite3_close(db);
        return -1;
    }
    
    // Create table and insert data first
    const char *sql = "CREATE TABLE IF NOT EXISTS users ("
                     "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                     "name TEXT NOT NULL,"
                     "email TEXT);"
                     "INSERT INTO users (name, email) VALUES ('John Doe', 'john@example.com');"
                     "INSERT INTO users (name, email) VALUES ('Jane Smith', 'jane@example.com');";
    
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        snprintf(result_buffer, buffer_size, "Setup failed!");
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return -1;
    }
    
    // Query data
    sql = "SELECT * FROM users;";
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc != SQLITE_OK) {
        snprintf(result_buffer, buffer_size, "Query failed!");
        sqlite3_close(db);
        return -1;
    }
    
    // Build result string
    char result[256] = "Encrypted Users:\n";
    int row_count = 0;
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char *name = (const char*)sqlite3_column_text(stmt, 1);
        const char *email = (const char*)sqlite3_column_text(stmt, 2);
        
        char row[128];
        snprintf(row, sizeof(row), "%d: %s (%s)\n", id, name, email);
        strncat(result, row, sizeof(result) - strlen(result) - 1);
        row_count++;
    }
    
    if (row_count == 0) {
        strncat(result, "No data found", sizeof(result) - strlen(result) - 1);
    }
    
    strncpy(result_buffer, result, buffer_size - 1);
    result_buffer[buffer_size - 1] = '\0';
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}

// SQLCipher demonstration function (for console output)
void sqlite_demo(void) {
    sqlite3 *db;
    char *err_msg = 0;
    int rc = sqlite3_open(":memory:", &db);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
    
    // Set encryption key for SQLCipher
    const char *key = "my_secret_key_123";
    rc = sqlite3_key(db, key, strlen(key));
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to set encryption key: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
    
    printf("SQLCipher encryption key set successfully\n");
    
    // Create a table
    const char *sql = "CREATE TABLE IF NOT EXISTS users ("
                     "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                     "name TEXT NOT NULL,"
                     "email TEXT);";
    
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    } else {
        printf("Encrypted table created successfully\n");
    }
    
    // Insert some data
    sql = "INSERT INTO users (name, email) VALUES ('John Doe', 'john@example.com');"
          "INSERT INTO users (name, email) VALUES ('Jane Smith', 'jane@example.com');";
    
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    } else {
        printf("Data inserted into encrypted database successfully\n");
    }
    
    // Query and display data
    sql = "SELECT * FROM users;";
    rc = sqlite3_exec(db, sql, callback, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to fetch data: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
    
    // Demonstrate SQLCipher pragmas
    printf("\nSQLCipher Database Information:\n");
    sql = "PRAGMA cipher_version;";
    rc = sqlite3_exec(db, sql, callback, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to get cipher version: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
    
    sql = "PRAGMA cipher_page_size;";
    rc = sqlite3_exec(db, sql, callback, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to get page size: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
    
    sql = "PRAGMA kdf_iter;";
    rc = sqlite3_exec(db, sql, callback, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to get KDF iterations: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
    
    sqlite3_close(db);
} 