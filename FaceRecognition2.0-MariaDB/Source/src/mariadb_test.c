#include "mariadb_test.h"
#include <mariadb/mysql.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// MariaDB connection configuration
// You can either:
// 1. Set the password directly here (less secure)
// 2. Set the MARIADB_PASSWORD environment variable (more secure)
#ifndef MARIADB_PASSWORD
#define MARIADB_PASSWORD "cybo008"
#endif

// Global MariaDB connection
static MYSQL *mysql_conn = NULL;

// MariaDB connection function
int mariadb_connect_db(const char *host, const char *user, const char *password, const char *database, int port) {
    mysql_conn = mysql_init(NULL);
    if (mysql_conn == NULL) {
        fprintf(stderr, "Failed to initialize MariaDB connection\n");
        return -1;
    }
    
    // Set connection timeout
    int timeout = 10;
    mysql_options(mysql_conn, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);
    
    // Connect to MariaDB server
    if (!mysql_real_connect(mysql_conn, host, user, password, database, port, "/run/mysqld/mysqld.sock", 0)) {
        fprintf(stderr, "Failed to connect to MariaDB: %s\n", mysql_error(mysql_conn));
        mysql_close(mysql_conn);
        mysql_conn = NULL;
        return -1;
    }
    
    printf("Successfully connected to MariaDB server\n");
    return 0;
}

// MariaDB disconnect function
void mariadb_disconnect(void) {
    if (mysql_conn != NULL) {
        mysql_close(mysql_conn);
        mysql_conn = NULL;
        printf("Disconnected from MariaDB server\n");
    }
}

// Create database function
int mariadb_create_database(const char *database_name) {
    if (mysql_conn == NULL) {
        return -1;
    }
    
    char query[256];
    snprintf(query, sizeof(query), "CREATE DATABASE IF NOT EXISTS %s", database_name);
    
    if (mysql_query(mysql_conn, query) != 0) {
        fprintf(stderr, "Failed to create database: %s\n", mysql_error(mysql_conn));
        return -1;
    }
    
    printf("Database '%s' created or already exists\n", database_name);
    return 0;
}

// Execute query function
int mariadb_execute_query(const char *query, char *result_buffer, size_t buffer_size) {
    if (mysql_conn == NULL) {
        snprintf(result_buffer, buffer_size, "No database connection");
        return -1;
    }
    
    if (mysql_query(mysql_conn, query) != 0) {
        snprintf(result_buffer, buffer_size, "Query failed: %s", mysql_error(mysql_conn));
        return -1;
    }
    
    MYSQL_RES *result = mysql_store_result(mysql_conn);
    if (result == NULL) {
        // Query doesn't return data (INSERT, UPDATE, DELETE, etc.)
        snprintf(result_buffer, buffer_size, "Query executed successfully");
        return 0;
    }
    
    // Process result set
    MYSQL_ROW row;
    int row_count = 0;
    size_t offset = 0;
    
    snprintf(result_buffer + offset, buffer_size - offset, "Query results:\n");
    offset = strlen(result_buffer);
    
    while ((row = mysql_fetch_row(result)) != NULL) {
        if (offset >= buffer_size - 100) break; // Leave space for safety
        
        int num_fields = mysql_num_fields(result);
        for (int i = 0; i < num_fields; i++) {
            if (offset >= buffer_size - 50) break;
            if (row[i] != NULL) {
                offset += snprintf(result_buffer + offset, buffer_size - offset, "%s ", row[i]);
            } else {
                offset += snprintf(result_buffer + offset, buffer_size - offset, "NULL ");
            }
        }
        offset += snprintf(result_buffer + offset, buffer_size - offset, "\n");
        row_count++;
    }
    
    mysql_free_result(result);
    
    if (row_count == 0) {
        snprintf(result_buffer + offset, buffer_size - offset, "No rows returned");
    }
    
    return 0;
}

// Create table and insert data function
int mariadb_create_table_and_insert_data(char *result_buffer, size_t buffer_size) {
    if (mysql_conn == NULL) {
        // Try to connect to local MariaDB server
        if (mariadb_connect_db("localhost", "root", MARIADB_PASSWORD, "test_db", 3306) != 0) {
            snprintf(result_buffer, buffer_size, "Failed to connect to MariaDB server. Please ensure MariaDB is running.");
            return -1;
        }
    }
    
    // Create table
    const char *create_table_sql = 
        "CREATE TABLE IF NOT EXISTS users ("
        "id INT AUTO_INCREMENT PRIMARY KEY,"
        "name VARCHAR(100) NOT NULL,"
        "email VARCHAR(100)"
        ")";
    
    if (mysql_query(mysql_conn, create_table_sql) != 0) {
        snprintf(result_buffer, buffer_size, "Failed to create table: %s", mysql_error(mysql_conn));
        return -1;
    }
    
    // Clear existing data to avoid duplicates
    if (mysql_query(mysql_conn, "DELETE FROM users") != 0) {
        snprintf(result_buffer, buffer_size, "Failed to clear table: %s", mysql_error(mysql_conn));
        return -1;
    }
    
    // Reset auto-increment counter
    if (mysql_query(mysql_conn, "ALTER TABLE users AUTO_INCREMENT = 1") != 0) {
        snprintf(result_buffer, buffer_size, "Failed to reset auto-increment: %s", mysql_error(mysql_conn));
        return -1;
    }
    
    // Insert data
    const char *insert_sql = 
        "INSERT INTO users (name, email) VALUES "
        "('Alice Johnson', 'alice@techcorp.com'), "
        "('Bob Wilson', 'bob@innovate.com'), "
        "('Carol Davis', 'carol@startup.io'), "
        "('David Brown', 'david@future.net')";
    
    if (mysql_query(mysql_conn, insert_sql) != 0) {
        snprintf(result_buffer, buffer_size, "Failed to insert data: %s", mysql_error(mysql_conn));
        return -1;
    }
    
    snprintf(result_buffer, buffer_size, "Table created and data inserted successfully!");
    return 0;
}

// Query and display data function
int mariadb_query_and_display_data(char *result_buffer, size_t buffer_size) {
    if (mysql_conn == NULL) {
        snprintf(result_buffer, buffer_size, "No database connection");
        return -1;
    }
    
    const char *query = "SELECT * FROM users";
    return mariadb_execute_query(query, result_buffer, buffer_size);
}

// MariaDB demonstration function
void mariadb_demo(void) {
    printf("=== MariaDB Database Demo ===\n");
    
    // Try to connect to MariaDB server
    if (mariadb_connect_db("localhost", "root", MARIADB_PASSWORD, "test_db", 3306) != 0) {
        printf("Failed to connect to MariaDB server.\n");
        printf("Please ensure MariaDB server is running and accessible.\n");
        printf("You can start MariaDB with: sudo systemctl start mariadb\n");
        printf("Or install it with: sudo apt install mariadb-server\n");
        return;
    }
    
    // Create database if it doesn't exist
    mariadb_create_database("test_db");
    
    // Use the test_db database
    if (mysql_query(mysql_conn, "USE test_db") != 0) {
        fprintf(stderr, "Failed to use database: %s\n", mysql_error(mysql_conn));
        mariadb_disconnect();
        return;
    }
    
    // Create table
    const char *create_table_sql = 
        "CREATE TABLE IF NOT EXISTS users ("
        "id INT AUTO_INCREMENT PRIMARY KEY,"
        "name VARCHAR(100) NOT NULL,"
        "email VARCHAR(100)"
        ")";
    
    if (mysql_query(mysql_conn, create_table_sql) != 0) {
        fprintf(stderr, "Failed to create table: %s\n", mysql_error(mysql_conn));
        mariadb_disconnect();
        return;
    }
    
    printf("Table 'users' created successfully\n");
    
    // Clear existing data to avoid duplicates
    if (mysql_query(mysql_conn, "DELETE FROM users") != 0) {
        fprintf(stderr, "Failed to clear table: %s\n", mysql_error(mysql_conn));
        mariadb_disconnect();
        return;
    }
    
    // Reset auto-increment counter
    if (mysql_query(mysql_conn, "ALTER TABLE users AUTO_INCREMENT = 1") != 0) {
        fprintf(stderr, "Failed to reset auto-increment: %s\n", mysql_error(mysql_conn));
        mariadb_disconnect();
        return;
    }
    
    // Insert data
    const char *insert_sql = 
        "INSERT INTO users (name, email) VALUES "
        "('Alice Johnson', 'alice@techcorp.com'), "
        "('Bob Wilson', 'bob@innovate.com'), "
        "('Carol Davis', 'carol@startup.io'), "
        "('David Brown', 'david@future.net')";
    
    if (mysql_query(mysql_conn, insert_sql) != 0) {
        fprintf(stderr, "Failed to insert data: %s\n", mysql_error(mysql_conn));
        mariadb_disconnect();
        return;
    }
    
    printf("Data inserted successfully\n");
    
    // Query and display data
    const char *query = "SELECT * FROM users";
    if (mysql_query(mysql_conn, query) != 0) {
        fprintf(stderr, "Failed to query data: %s\n", mysql_error(mysql_conn));
        mariadb_disconnect();
        return;
    }
    
    MYSQL_RES *result = mysql_store_result(mysql_conn);
    if (result == NULL) {
        fprintf(stderr, "Failed to store result\n");
        mariadb_disconnect();
        return;
    }
    
    printf("\nQuery Results:\n");
    printf("ID | Name        | Email\n");
    printf("---+-------------+------------------\n");
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) != NULL) {
        printf("%s | %-12s | %s\n", 
               row[0] ? row[0] : "NULL", 
               row[1] ? row[1] : "NULL", 
               row[2] ? row[2] : "NULL");
    }
    
    mysql_free_result(result);
    
    // Show MariaDB version
    printf("\nMariaDB Server Information:\n");
    printf("Server version: %s\n", mysql_get_server_info(mysql_conn));
    printf("Client version: %s\n", mysql_get_client_info());
    
    mariadb_disconnect();
    printf("\n=== MariaDB Demo completed ===\n");
}
