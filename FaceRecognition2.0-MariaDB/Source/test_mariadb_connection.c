#include <mariadb/mysql.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MARIADB_PASSWORD "cybo008"

int main() {
    MYSQL *mysql_conn = NULL;
    
    printf("=== MariaDB Connection Test ===\n");
    
    // Initialize MariaDB connection
    mysql_conn = mysql_init(NULL);
    if (mysql_conn == NULL) {
        printf("Failed to initialize MariaDB connection\n");
        return -1;
    }
    
    printf("MariaDB connection initialized\n");
    
    // Set connection timeout
    int timeout = 10;
    mysql_options(mysql_conn, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);
    
    printf("Attempting to connect to MariaDB...\n");
    printf("Host: localhost\n");
    printf("User: root\n");
    printf("Database: test_db\n");
    printf("Port: 3306\n");
    
    // Connect to MariaDB server
    if (!mysql_real_connect(mysql_conn, "localhost", "root", MARIADB_PASSWORD, "test_db", 3306, "/run/mysqld/mysqld.sock", 0)) {
        printf("Failed to connect to MariaDB: %s\n", mysql_error(mysql_conn));
        mysql_close(mysql_conn);
        return -1;
    }
    
    printf("Successfully connected to MariaDB server!\n");
    printf("Server version: %s\n", mysql_get_server_info(mysql_conn));
    printf("Client version: %s\n", mysql_get_client_info());
    
    // Test a simple query
    if (mysql_query(mysql_conn, "SELECT COUNT(*) FROM users") != 0) {
        printf("Query failed: %s\n", mysql_error(mysql_conn));
    } else {
        MYSQL_RES *result = mysql_store_result(mysql_conn);
        if (result) {
            MYSQL_ROW row = mysql_fetch_row(result);
            if (row) {
                printf("Number of users in database: %s\n", row[0]);
            }
            mysql_free_result(result);
        }
    }
    
    mysql_close(mysql_conn);
    printf("Connection closed\n");
    
    return 0;
}
