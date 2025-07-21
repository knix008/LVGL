# SQLCipher Integration in LVGL Project

This project now includes SQLCipher database functionality integrated with the LVGL GUI framework. SQLCipher is an open-source extension to SQLite that provides transparent 256-bit AES encryption of database files.

## Features

- **SQLCipher 4.x Integration**: Full SQLCipher database support with AES-256 encryption
- **Transparent Encryption**: Database files are automatically encrypted/decrypted
- **Memory Database**: Uses in-memory database for fast operations
- **GUI Integration**: LVGL GUI with SQLCipher backend
- **Cross-platform**: Works on Linux, Windows, and other platforms
- **OpenSSL Crypto Provider**: Uses OpenSSL for cryptographic operations

## Build Requirements

- CMake 3.10 or higher
- SDL2 development libraries
- Build tools (gcc, make)
- OpenSSL development libraries
- **TCL (optional)**: Only required for generating SQLCipher amalgamation files

## Build Process

### Option 1: Using Pre-generated Files (Recommended)
If the SQLCipher amalgamation files are already present, you can build directly:

```bash
cd Source
mkdir build
cd build
cmake ..
make
```

### Option 2: Generate Amalgamation Files (Requires TCL)
If you need to generate the SQLCipher amalgamation files:

1. **Install Dependencies**:
   ```bash
   # Ubuntu/Debian
   sudo apt-get install libssl-dev tcl
   
   # CentOS/RHEL
   sudo yum install openssl-devel tcl
   ```

2. **Generate SQLCipher Amalgamation Files**:
   ```bash
   cd Source
   ./build_sqlcipher.sh
   ```

3. **Build the Project**:
   ```bash
   cd Source
   mkdir build
   cd build
   cmake ..
   make
   ```

## When Do You Need TCL?

**TCL is only required if:**
- The SQLCipher amalgamation files (`sqlite3.h` and `sqlite3.c`) don't exist
- You want to regenerate these files from source
- You're developing or modifying SQLCipher itself

**You DON'T need TCL if:**
- The amalgamation files are already present (which they are in this project)
- You're just building and running the application
- You're using the project as-is

## SQLCipher Features Enabled

The following SQLCipher features are enabled in the build:

- `SQLITE_HAS_CODEC`: Enables encryption/decryption functionality
- `SQLCIPHER_CRYPTO_OPENSSL`: Uses OpenSSL as the crypto provider
- `SQLITE_ENABLE_FTS5`: Full-text search
- `SQLITE_ENABLE_JSON1`: JSON functions
- `SQLITE_ENABLE_RTREE`: R-tree spatial indexing
- `SQLITE_ENABLE_UNLOCK_NOTIFY`: Unlock notification
- `SQLITE_ENABLE_DBSTAT_VTAB`: Database statistics virtual table
- `SQLITE_ENABLE_LOAD_EXTENSION`: Extension loading support
- `SQLITE_THREADSAFE`: Thread-safe operation

## Usage Example

The main application demonstrates:

1. **Database Creation**: Creates an encrypted in-memory SQLCipher database
2. **Key Setting**: Sets an encryption key using `sqlite3_key()`
3. **Table Creation**: Creates a users table with id, name, and email fields
4. **Data Insertion**: Inserts sample user data into the encrypted database
5. **Data Querying**: Queries and displays the encrypted data
6. **SQLCipher Pragma**: Demonstrates SQLCipher-specific pragmas
7. **GUI Display**: Shows an LVGL window with a button

## Code Structure

- `main.c`: Main application with SQLCipher demonstration
- `CMakeLists.txt`: Build configuration with SQLCipher integration
- `sqlcipher/sqlite3.h`: SQLCipher header file (pre-generated)
- `sqlcipher/sqlite3.c`: SQLCipher amalgamation source (pre-generated)
- `build_sqlcipher.sh`: Script to generate SQLCipher amalgamation files (optional)

## Running the Application

```bash
cd Source/build
./lvgl_main
```

The application will:
1. Run SQLCipher demonstration (set key, create encrypted table, insert data, query data)
2. Display the results in the console including SQLCipher pragmas
3. Open an LVGL GUI window
4. Show a clickable button

## Database Operations

The demonstration includes:

- **CREATE TABLE**: Creates an encrypted users table
- **INSERT**: Adds sample user records to encrypted database
- **SELECT**: Queries and displays all users from encrypted database
- **Memory Database**: Uses `:memory:` for fast in-memory operations
- **Encryption Key**: Sets encryption key using `sqlite3_key()`
- **SQLCipher Pragma**: Shows cipher version, page size, and KDF iterations

## Encryption Features

SQLCipher provides:

- **AES-256 Encryption**: Industry-standard encryption algorithm
- **Transparent Operation**: Applications work normally with encrypted databases
- **Key Derivation**: PBKDF2 key derivation with configurable iterations
- **HMAC Integrity**: Optional HMAC for data integrity verification
- **Configurable Settings**: Adjustable page size, KDF iterations, and algorithms

## Extending the Application

To add more database functionality:

1. Add new SQL operations in the `sqlite_demo()` function
2. Create new GUI elements in `lv_example_simple_gui()`
3. Connect GUI events to database operations
4. Add error handling for database operations
5. Use SQLCipher pragmas to configure encryption settings

## Troubleshooting

- **Build Errors**: Ensure SQLCipher amalgamation files are present
- **Linker Errors**: Check that OpenSSL library (`-lcrypto`) is linked
- **Runtime Errors**: Verify SQLCipher operations have proper error handling
- **Encryption Errors**: Ensure encryption key is set before database operations
- **TCL Errors**: Only needed for generating amalgamation files, not for normal usage 