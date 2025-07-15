#include <stdio.h>
#include <unistd.h>
#include <sqlite3.h>
#include <string.h>
#include "lvgl.h"
#include "lv_drivers.h"

static void btn_event_cb(lv_event_t * e) {
    lv_obj_t * label = lv_event_get_user_data(e);
    
    // SQLite test for Button 1 - Create table and insert data
    sqlite3 *db;
    char *err_msg = 0;
    int rc = sqlite3_open(":memory:", &db);
    
    if (rc != SQLITE_OK) {
        lv_label_set_text(label, "Database Error!");
        return;
    }
    
    // Create table
    const char *sql = "CREATE TABLE IF NOT EXISTS users ("
                     "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                     "name TEXT NOT NULL,"
                     "email TEXT);";
    
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        lv_label_set_text(label, "Table creation failed!");
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return;
    }
    
    // Insert data
    sql = "INSERT INTO users (name, email) VALUES ('John Doe', 'john@example.com');"
          "INSERT INTO users (name, email) VALUES ('Jane Smith', 'jane@example.com');";
    
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        lv_label_set_text(label, "Data insertion failed!");
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return;
    }
    
    lv_label_set_text(label, "Table created & data inserted!");
    sqlite3_close(db);
}

static void btn2_event_cb(lv_event_t * e) {
    lv_obj_t * label = lv_event_get_user_data(e);
    
    // SQLite test for Button 2 - Query and display data
    sqlite3 *db;
    char *err_msg = 0;
    int rc = sqlite3_open(":memory:", &db);
    
    if (rc != SQLITE_OK) {
        lv_label_set_text(label, "Database Error!");
        return;
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
        lv_label_set_text(label, "Setup failed!");
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return;
    }
    
    // Query data
    sql = "SELECT * FROM users;";
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc != SQLITE_OK) {
        lv_label_set_text(label, "Query failed!");
        sqlite3_close(db);
        return;
    }
    
    // Build result string
    char result[256] = "Users:\n";
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
    
    lv_label_set_text(label, result);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

// Callback function for SQLite query results
static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    NotUsed = 0;
    printf("Query result:\n");
    for (int i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

// SQLite demonstration function
void sqlite_demo(void) {
    sqlite3 *db;
    char *err_msg = 0;
    int rc = sqlite3_open(":memory:", &db);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
    
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
        printf("Table created successfully\n");
    }
    
    // Insert some data
    sql = "INSERT INTO users (name, email) VALUES ('John Doe', 'john@example.com');"
          "INSERT INTO users (name, email) VALUES ('Jane Smith', 'jane@example.com');";
    
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    } else {
        printf("Data inserted successfully\n");
    }
    
    // Query and display data
    sql = "SELECT * FROM users;";
    rc = sqlite3_exec(db, sql, callback, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to fetch data: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
    
    sqlite3_close(db);
}

void lv_example_simple_gui(void) {
    // Create a screen
    lv_obj_t * scr = lv_scr_act();

    // Create a label
    lv_obj_t * label = lv_label_create(scr);
    lv_label_set_text(label, "SQLite Test with LVGL");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 20);

    // Create first button
    lv_obj_t * btn = lv_btn_create(scr);
    lv_obj_align(btn, LV_ALIGN_CENTER, -60, 0);

    // Create a label on the first button
    lv_obj_t * btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "Create DB");

    // Create second button
    lv_obj_t * btn2 = lv_btn_create(scr);
    lv_obj_align(btn2, LV_ALIGN_CENTER, 60, 0);

    // Create a label on the second button
    lv_obj_t * btn2_label = lv_label_create(btn2);
    lv_label_set_text(btn2_label, "Query DB");

    // Assign event callbacks to the buttons, pass label as user data
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, label);
    lv_obj_add_event_cb(btn2, btn2_event_cb, LV_EVENT_CLICKED, label);
}

// In your main loop, call lv_timer_handler() periodically, e.g., every 5-10 ms
int main(void)
{
    lv_init();
    
    // Initialize SDL display driver
    lv_display_t * disp = lv_sdl_window_create(240, 480);
    if (disp == NULL) {
        printf("Failed to create SDL window!\n");
        return -1;
    }
    
    // Set window title
    lv_sdl_window_set_title(disp, "SQLite Test with LVGL");
    
    // Create input devices
    lv_indev_t * mouse = lv_sdl_mouse_create();
    lv_indev_t * mousewheel = lv_sdl_mousewheel_create();
    lv_indev_t * keyboard = lv_sdl_keyboard_create();

    // Run SQLite demonstration
    printf("Running SQLite demonstration...\n");
    sqlite_demo();

    lv_example_simple_gui();

    printf("GUI created successfully. Window should appear now.\n");
    printf("Press Ctrl+C to exit.\n");

    while(1) {
        lv_timer_handler();
        usleep(5000);
    }

    return 0;
}