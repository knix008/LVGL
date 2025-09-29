#include "../include/ca_server.h"

static sqlite3 *db = NULL;

int db_init(const char *db_path) {
    int rc = sqlite3_open(db_path, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    // Create certificates table
    const char *sql = "CREATE TABLE IF NOT EXISTS certificates ("
                     "serial_number TEXT PRIMARY KEY,"
                     "common_name TEXT NOT NULL,"
                     "email TEXT,"
                     "status TEXT DEFAULT 'valid',"
                     "created_at INTEGER,"
                     "expires_at INTEGER,"
                     "cert_path TEXT,"
                     "key_path TEXT"
                     ");";
    
    char *err_msg = 0;
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }
    
    return 0;
}

int db_add_certificate(cert_record_t *record) {
    const char *sql = "INSERT INTO certificates (serial_number, common_name, email, status, created_at, expires_at, cert_path, key_path) "
                     "VALUES (?, ?, ?, ?, ?, ?, ?, ?);";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_text(stmt, 1, record->serial_number, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, record->common_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, record->email, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, record->status, -1, SQLITE_STATIC);
    sqlite3_bind_int64(stmt, 5, record->created_at);
    sqlite3_bind_int64(stmt, 6, record->expires_at);
    sqlite3_bind_text(stmt, 7, record->cert_path, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 8, record->key_path, -1, SQLITE_STATIC);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to insert certificate: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    return 0;
}

int db_get_certificate(const char *serial_number, cert_record_t *record) {
    const char *sql = "SELECT serial_number, common_name, email, status, created_at, expires_at, cert_path, key_path "
                     "FROM certificates WHERE serial_number = ?;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_text(stmt, 1, serial_number, -1, SQLITE_STATIC);
    
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        strcpy(record->serial_number, (char*)sqlite3_column_text(stmt, 0));
        strcpy(record->common_name, (char*)sqlite3_column_text(stmt, 1));
        strcpy(record->email, (char*)sqlite3_column_text(stmt, 2));
        strcpy(record->status, (char*)sqlite3_column_text(stmt, 3));
        record->created_at = sqlite3_column_int64(stmt, 4);
        record->expires_at = sqlite3_column_int64(stmt, 5);
        strcpy(record->cert_path, (char*)sqlite3_column_text(stmt, 6));
        strcpy(record->key_path, (char*)sqlite3_column_text(stmt, 7));
        
        sqlite3_finalize(stmt);
        return 0;
    }
    
    sqlite3_finalize(stmt);
    return -1;
}

int db_list_certificates(cert_record_t **records, int *count) {
    const char *sql = "SELECT serial_number, common_name, email, status, created_at, expires_at, cert_path, key_path "
                     "FROM certificates ORDER BY created_at DESC;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    // Count rows
    int row_count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        row_count++;
    }
    sqlite3_reset(stmt);
    
    if (row_count == 0) {
        *records = NULL;
        *count = 0;
        sqlite3_finalize(stmt);
        return 0;
    }
    
    // Allocate memory
    *records = malloc(row_count * sizeof(cert_record_t));
    if (!*records) {
        sqlite3_finalize(stmt);
        return -1;
    }
    
    // Fill records
    int i = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && i < row_count) {
        strcpy((*records)[i].serial_number, (char*)sqlite3_column_text(stmt, 0));
        strcpy((*records)[i].common_name, (char*)sqlite3_column_text(stmt, 1));
        strcpy((*records)[i].email, (char*)sqlite3_column_text(stmt, 2));
        strcpy((*records)[i].status, (char*)sqlite3_column_text(stmt, 3));
        (*records)[i].created_at = sqlite3_column_int64(stmt, 4);
        (*records)[i].expires_at = sqlite3_column_int64(stmt, 5);
        strcpy((*records)[i].cert_path, (char*)sqlite3_column_text(stmt, 6));
        strcpy((*records)[i].key_path, (char*)sqlite3_column_text(stmt, 7));
        i++;
    }
    
    *count = i;
    sqlite3_finalize(stmt);
    return 0;
}

int db_revoke_certificate(const char *serial_number) {
    const char *sql = "UPDATE certificates SET status = 'revoked' WHERE serial_number = ?;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_text(stmt, 1, serial_number, -1, SQLITE_STATIC);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to revoke certificate: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    return 0;
}

int db_cleanup(void) {
    if (db) {
        sqlite3_close(db);
        db = NULL;
    }
    return 0;
}
