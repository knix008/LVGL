# SQLite Integration in LVGL Project

This project now includes SQLite database functionality integrated with the LVGL GUI framework.

## Features

- **SQLite 3.x Integration**: Full SQLite database support with all major features
- **Memory Database**: Uses in-memory database for fast operations
- **GUI Integration**: LVGL GUI with SQLite backend
- **Cross-platform**: Works on Linux, Windows, and other platforms

## Build Requirements

- CMake 3.10 or higher
- SDL2 development libraries
- Build tools (gcc, make)
- TCL (for SQLite amalgamation generation)

## Build Process

1. **Generate SQLite Amalgamation Files**:
   ```bash
   cd Source/sqlite
   make -f Makefile.linux-generic sqlite3.h
   make -f Makefile.linux-generic sqlite3.c
   ```

2. **Build the Project**:
   ```bash
   cd Source
   mkdir build
   cd build
   cmake ..
   make
   ```

## SQLite Features Enabled

The following SQLite features are enabled in the build:

- `SQLITE_ENABLE_FTS5`: Full-text search
- `SQLITE_ENABLE_JSON1`: JSON functions
- `SQLITE_ENABLE_RTREE`: R-tree spatial indexing
- `SQLITE_ENABLE_UNLOCK_NOTIFY`: Unlock notification
- `SQLITE_ENABLE_DBSTAT_VTAB`: Database statistics virtual table
- `SQLITE_ENABLE_LOAD_EXTENSION`: Extension loading support
- `SQLITE_THREADSAFE`: Thread-safe operation

## Usage Example

The main application demonstrates:

1. **Database Creation**: Creates an in-memory SQLite database
2. **Table Creation**: Creates a users table with id, name, and email fields
3. **Data Insertion**: Inserts sample user data
4. **Data Querying**: Queries and displays the inserted data
5. **GUI Display**: Shows an LVGL window with a button

## Code Structure

- `main.c`: Main application with SQLite demonstration
- `CMakeLists.txt`: Build configuration with SQLite integration
- `sqlite/sqlite3.h`: SQLite header file (generated)
- `sqlite/sqlite3.c`: SQLite amalgamation source (generated)

## Running the Application

```bash
cd Source/build
./lvgl_main
```

The application will:
1. Run SQLite demonstration (create table, insert data, query data)
2. Display the results in the console
3. Open an LVGL GUI window
4. Show a clickable button

## Database Operations

The demonstration includes:

- **CREATE TABLE**: Creates a users table
- **INSERT**: Adds sample user records
- **SELECT**: Queries and displays all users
- **Memory Database**: Uses `:memory:` for fast in-memory operations

## Extending the Application

To add more database functionality:

1. Add new SQL operations in the `sqlite_demo()` function
2. Create new GUI elements in `lv_example_simple_gui()`
3. Connect GUI events to database operations
4. Add error handling for database operations

## Troubleshooting

- **Build Errors**: Ensure SQLite amalgamation files are generated
- **Linker Errors**: Check that math library (`-lm`) is linked
- **Runtime Errors**: Verify SQLite operations have proper error handling 