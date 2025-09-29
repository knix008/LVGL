cmake_minimum_required(VERSION 3.10)
project(CertificateAuthority)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(OPENSSL REQUIRED openssl)
pkg_check_modules(JSON_C REQUIRED json-c)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)

# Include directories
include_directories(include)
include_directories(${OPENSSL_INCLUDE_DIRS})
include_directories(${JSON_C_INCLUDE_DIRS})
include_directories(${SQLITE3_INCLUDE_DIRS})

# Source files
set(SOURCES
    src/main.c
    src/ca_server.c
    src/database.c
    src/http_server.c
    src/config_parser.c
)

# Create executable
add_executable(ca_server ${SOURCES})

# Link libraries
target_link_libraries(ca_server
    ${OPENSSL_LIBRARIES}
    ${JSON_C_LIBRARIES}
    ${SQLITE3_LIBRARIES}
    pthread
)

# Compiler flags
target_compile_options(ca_server PRIVATE
    ${OPENSSL_CFLAGS_OTHER}
    ${JSON_C_CFLAGS_OTHER}
    ${SQLITE3_CFLAGS_OTHER}
    -Wall
    -Wextra
    -O2
)

# Install target
install(TARGETS ca_server DESTINATION bin)

# Create directories for certificates and keys
install(DIRECTORY DESTINATION certs)
install(DIRECTORY DESTINATION keys)
