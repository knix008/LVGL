cmake_minimum_required(VERSION 3.10)
project(lvgl_main C)

set(CMAKE_C_STANDARD 99)

# Force rebuild on every make
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "")

set(LVGL_DIR "lvgl")
set(LV_DRIVERS_DIR "lvgl/src/drivers")
set(SQLCIPHER_DIR "sqlcipher")

# Create lib directory
set(LIB_DIR "${CMAKE_BINARY_DIR}/lib")
file(MAKE_DIRECTORY ${LIB_DIR})

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

# Create LVGL static library
file(GLOB_RECURSE LVGL_SOURCES
    ${LVGL_DIR}/src/*.c
)
file(GLOB LV_DRIVERS_SOURCES
    ${LV_DRIVERS_DIR}/sdl/*.c
)

# Create LVGL library
add_library(lvgl_lib STATIC ${LVGL_SOURCES} ${LV_DRIVERS_SOURCES})
target_include_directories(lvgl_lib PUBLIC 
    ${LVGL_DIR}
    ${LVGL_DIR}/src
    ${LVGL_DIR}/src/libs
    ${LVGL_DIR}/src/libs/freetype
    ${LV_DRIVERS_DIR}
    ${LV_DRIVERS_DIR}/sdl
)

# Set output directory for LVGL library
set_target_properties(lvgl_lib PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${LIB_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${LIB_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${LIB_DIR}
)

# Explicitly list all source files to ensure proper dependency tracking
set(MAIN_SOURCES
    src/main.c
    src/sqlite_test.c
    src/ui_components.c
    src/tab_db.c
    src/tab_settings.c
    src/tab_info.c
    src/tab_number.c
    src/tab_korean.c
    src/tab_chunjiin.c
    src/korean_input.c
    src/chunjiin_input.c
    src/ui_callbacks.c
)

# SQLCipher amalgamation files
set(SQLCIPHER_SOURCES
    ${SQLCIPHER_DIR}/sqlite3.c
)

# Create main executable
add_executable(lvgl_main ${MAIN_SOURCES} ${SQLCIPHER_SOURCES})

# Add include directories
target_include_directories(lvgl_main PRIVATE include sqlcipher sqlite lvgl)

# Link against LVGL library and other dependencies
target_link_libraries(lvgl_main lvgl_lib ${SDL2_LIBRARIES} ${FREETYPE_LIBRARIES})

if(UNIX)
    target_link_libraries(lvgl_main pthread dl m crypto)
endif()

# Force rebuild of main sources when any source file changes
foreach(source_file ${MAIN_SOURCES})
    set_source_files_properties(${source_file} PROPERTIES HEADER_FILE_ONLY FALSE)
endforeach()

# Add custom clean target to ensure complete cleanup
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/CMakeFiles
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/CMakeCache.txt
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/lvgl_main
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/CMakeFiles/lvgl_main.dir/src/*.o
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/CMakeFiles/lvgl_main.dir/src/*.d
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${LIB_DIR}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Cleaning all build files"
)

# Add custom target to clean only main sources (keep LVGL library)
add_custom_target(clean-main
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/lvgl_main
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/CMakeFiles/lvgl_main.dir/src/*.o
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/CMakeFiles/lvgl_main.dir/src/*.d
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Cleaning main application files (keeping LVGL library)"
)