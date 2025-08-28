# MariaDB Integration for FaceRecognition2.0

This document explains the MariaDB integration in the FaceRecognition2.0 project, which replaces the previous SQLCipher encrypted database with a full MariaDB client-server database solution.

## Overview

The project now uses MariaDB Connector/C (version 3.3.9) as the database client library, allowing the application to connect to MariaDB or MySQL servers for database operations. This provides:

- **Client-Server Architecture**: Connect to remote or local database servers
- **Better Scalability**: Support for multiple concurrent connections
- **Standard SQL**: Full SQL support with MariaDB/MySQL compatibility
- **SSL/TLS Support**: Secure connections with encryption
- **Better Performance**: Optimized for larger datasets and concurrent access

## Architecture

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   LVGL GUI      │    │   MariaDB       │    │   MariaDB       │
│   Application   │◄──►│   Connector/C   │◄──►│   Server        │
│                 │    │   (Client)      │    │                 │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

## Components

### 1. MariaDB Connector/C Library
- **Location**: `Source/lib/lib/libmariadb.a`
- **Headers**: `Source/lib/include/mysql/`
- **Build Script**: `Source/build_mariadb.sh`
- **Version**: 3.3.9

### 2. Database Interface
- **Header**: `Source/include/mariadb_test.h`
- **Implementation**: `Source/src/mariadb_test.c`
- **UI Integration**: `Source/src/tab_db.c`

### 3. Build System
- **CMake Configuration**: Updated `Source/CMakeLists.txt`
- **Library Build**: Integrated into `Source/build_all_libs.sh`
- **Main Runner**: Updated `run.sh`

## Setup Instructions

### 1. Install MariaDB Server

Run the provided setup script:
```bash
cd Source
./setup_mariadb.sh
```

This script will:
- Install MariaDB server if not already installed
- Start and enable the MariaDB service
- Secure the installation
- Create a test database (`test_db`) with sample data

### 2. Build the Application

Build the application with MariaDB support:
```bash
./run.sh
```

This will:
- Build MariaDB Connector/C from source
- Compile the application with MariaDB support
- Create the executable with database connectivity

### 3. Run the Application

```bash
cd Source/build
./main
```

## Database Operations

### Connection Management

The application automatically connects to the MariaDB server with these default settings:
- **Host**: localhost
- **Port**: 3306
- **Database**: test_db
- **User**: root (or configured user)
- **Password**: (as set during MariaDB setup)

### Available Functions

#### Core Functions
- `mariadb_connect()` - Establish database connection
- `mariadb_disconnect()` - Close database connection
- `mariadb_create_database()` - Create new database
- `mariadb_execute_query()` - Execute SQL queries

#### UI Functions
- `mariadb_create_table_and_insert_data()` - Create table and insert sample data
- `mariadb_query_and_display_data()` - Query and display data in UI
- `mariadb_demo()` - Console demonstration of database operations

### Example Usage

```c
// Connect to database
mariadb_connect("localhost", "root", "password", "test_db", 3306);

// Create table and insert data
char result[256];
mariadb_create_table_and_insert_data(result, sizeof(result));

// Query data
mariadb_query_and_display_data(result, sizeof(result));

// Disconnect
mariadb_disconnect();
```

## GUI Integration

The Database tab in the LVGL GUI provides:
- **Create DB Button**: Creates tables and inserts sample data
- **Query DB Button**: Queries and displays data
- **Result Display**: Shows operation results and query data

## Configuration

### Connection Settings

You can modify the connection settings in `Source/src/mariadb_test.c`:

```c
// Default connection parameters
#define MARIADB_HOST "localhost"
#define MARIADB_PORT 3306
#define MARIADB_DATABASE "test_db"
#define MARIADB_USER "root"
#define MARIADB_PASSWORD ""
```

### SSL/TLS Support

The MariaDB Connector/C is built with OpenSSL support, enabling secure connections:
- SSL connections to remote servers
- Certificate verification
- Encrypted data transmission

## Troubleshooting

### Common Issues

1. **Connection Failed**
   - Ensure MariaDB server is running: `sudo systemctl status mariadb`
   - Check if port 3306 is accessible
   - Verify user credentials and permissions

2. **Build Errors**
   - Ensure all dependencies are built: `./run.sh libs`
   - Check that OpenSSL is properly built
   - Verify CMake configuration

3. **Permission Denied**
   - Check MariaDB user permissions
   - Ensure database exists and is accessible
   - Verify firewall settings for remote connections

### Debug Information

The application provides detailed error messages:
- Connection errors with specific details
- SQL query errors with MariaDB error codes
- Build errors with compilation details

## Migration from SQLCipher

### Key Changes

1. **Database Type**: From embedded SQLite to client-server MariaDB
2. **Encryption**: From local encryption to SSL/TLS network encryption
3. **Architecture**: From single-file database to server-based database
4. **Scalability**: From single-user to multi-user capable

### Data Migration

To migrate existing SQLite data to MariaDB:
1. Export data from SQLite database
2. Create corresponding MariaDB tables
3. Import data using MariaDB tools
4. Update application to use MariaDB connection

## Performance Considerations

### Advantages
- Better concurrent access
- Network-based access
- Standard SQL features
- Better for large datasets

### Considerations
- Network latency for remote connections
- Server resource requirements
- Connection pooling for high concurrency

## Security

### Network Security
- SSL/TLS encryption for data transmission
- User authentication and authorization
- Network-level security controls

### Best Practices
- Use dedicated database users with minimal privileges
- Enable SSL/TLS for all connections
- Regular security updates for MariaDB server
- Proper firewall configuration

## Future Enhancements

### Planned Features
- Connection pooling for better performance
- Prepared statements for security
- Transaction support
- Backup and recovery integration
- Monitoring and logging

### Extensibility
- Easy to add new database operations
- Support for stored procedures
- Integration with other database systems
- Custom query builders

## Support

For issues related to MariaDB integration:
1. Check the troubleshooting section
2. Review MariaDB server logs
3. Verify network connectivity
4. Test with MariaDB command-line client

## References

- [MariaDB Connector/C Documentation](https://mariadb.com/kb/en/mariadb-connector-c/)
- [MariaDB Server Documentation](https://mariadb.com/kb/en/documentation/)
- [MySQL C API Reference](https://dev.mysql.com/doc/c-api/8.0/en/)
