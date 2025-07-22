cmake_minimum_required(VERSION 3.10)
project(lvgl_main C)

set(CMAKE_C_STANDARD 99)

set(LVGL_DIR "lvgl")
set(LV_DRIVERS_DIR "lvgl/src/drivers")
set(SQLCIPHER_DIR "sqlcipher")

include_directories(
    ${LVGL_DIR}
    ${LVGL_DIR}/src
    ${LVGL_DIR}/src/libs
    ${LVGL_DIR}/src/libs/freetype
    ${LV_DRIVERS_DIR}
    ${LV_DRIVERS_DIR}/sdl
    ${SQLCIPHER_DIR}/src
)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Find FreeType
find_package(PkgConfig REQUIRED)
pkg_check_modules(FREETYPE REQUIRED freetype2)
include_directories(${FREETYPE_INCLUDE_DIRS})

add_compile_definitions($<$<BOOL:${LV_USE_DRAW_SDL}>:LV_USE_DRAW_SDL=1>)

# SQLCipher compile definitions
add_compile_definitions(
    SQLITE_HAS_CODEC=1
    SQLITE_EXTRA_INIT=sqlcipher_extra_init
    SQLITE_EXTRA_SHUTDOWN=sqlcipher_extra_shutdown
    SQLITE_THREADSAFE=1
    SQLITE_TEMP_STORE=2
    SQLITE_ENABLE_FTS5=1
    SQLITE_ENABLE_JSON1=1
    SQLITE_ENABLE_RTREE=1
    SQLITE_ENABLE_UNLOCK_NOTIFY=1
    SQLITE_ENABLE_DBSTAT_VTAB=1
    SQLITE_ENABLE_LOAD_EXTENSION=1
    SQLCIPHER_CRYPTO_OPENSSL=1
    SQLCIPHER_OMIT_LOG=1
)

add_executable(lvgl_main
    src/main.c
    src/sqlite_test.c
    src/ui_components.c
    src/tab_db.c
    src/tab_settings.c
    src/tab_info.c
    src/tab_number.c
    src/tab_korean.c
    src/ui_callbacks.c
)

# Add include directories
target_include_directories(lvgl_main PRIVATE include sqlcipher sqlite lvgl)

file(GLOB_RECURSE LVGL_SOURCES
    ${LVGL_DIR}/src/*.c
)
file(GLOB LV_DRIVERS_SOURCES
    ${LV_DRIVERS_DIR}/sdl/*.c
)

# SQLCipher amalgamation files
set(SQLCIPHER_SOURCES
    ${SQLCIPHER_DIR}/sqlite3.c
)

target_sources(lvgl_main PRIVATE ${LVGL_SOURCES} ${LV_DRIVERS_SOURCES} ${SQLCIPHER_SOURCES})
target_link_libraries(lvgl_main ${SDL2_LIBRARIES} ${FREETYPE_LIBRARIES})

if(UNIX)
    target_link_libraries(lvgl_main pthread dl m crypto)
endif()