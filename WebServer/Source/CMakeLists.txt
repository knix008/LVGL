cmake_minimum_required(VERSION 3.10)
project(main C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 14)

# Force rebuild on every make
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "")

# ============================================================================
# CPU ARCHITECTURE DETECTION
# ============================================================================
message(STATUS "========== System Architecture Detection ==========")
message(STATUS "System: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Processor: ${CMAKE_SYSTEM_PROCESSOR}")

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64)")
    set(ARCH_TYPE "aarch64")
    set(ARCH_BITS "64")
    message(STATUS "✓ Detected ARM64 (aarch64) architecture")
    message(STATUS "  - 64-bit ARM processor")
    message(STATUS "  - Suitable for: Raspberry Pi 4/5, Apple Silicon (native), other ARM64 boards")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64|x86_64|x64)")
    set(ARCH_TYPE "x86_64")
    set(ARCH_BITS "64")
    message(STATUS "✓ Detected x86-64 (AMD64/Intel64) architecture")
    message(STATUS "  - 64-bit Intel/AMD x86 processor")
    message(STATUS "  - Suitable for: Desktop computers, laptops, servers")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm|armv7l|armv7)")
    set(ARCH_TYPE "arm")
    set(ARCH_BITS "32")
    message(STATUS "✓ Detected ARM 32-bit architecture")
    message(STATUS "  - 32-bit ARM processor")
    message(STATUS "  - Suitable for: Older Raspberry Pi models, older ARM boards")
else()
    set(ARCH_TYPE "unknown")
    set(ARCH_BITS "unknown")
    message(WARNING "✗ Unknown architecture: ${CMAKE_SYSTEM_PROCESSOR}")
    message(WARNING "  Build may fail due to architecture mismatch")
    message(WARNING "  Please report this issue with your system information")
endif()

message(STATUS "Architecture Set: ${ARCH_TYPE} (${ARCH_BITS}-bit)")
message(STATUS "====================================================")

# ============================================================================
# DEPENDENCY VERIFICATION
# ============================================================================
message(STATUS "")
message(STATUS "========== Dependency Verification ==========")

# Function to check if a command exists
function(check_program PROGRAM_NAME DISPLAY_NAME)
    find_program(PROGRAM_PATH ${PROGRAM_NAME})
    if(PROGRAM_PATH)
        message(STATUS "✓ ${DISPLAY_NAME}: FOUND")
        set(${PROGRAM_NAME}_FOUND TRUE PARENT_SCOPE)
    else()
        message(WARNING "✗ ${DISPLAY_NAME}: NOT FOUND")
        set(${PROGRAM_NAME}_FOUND FALSE PARENT_SCOPE)
    endif()
endfunction()

# Check build tools
check_program(cmake "CMake")
check_program(make "GNU Make")
check_program(gcc "GCC Compiler")
check_program(g++ "G++ Compiler")
check_program(pkg-config "pkg-config")

# Check system packages
message(STATUS "")
message(STATUS "Checking system libraries...")

find_package(SDL2 QUIET)
if(SDL2_FOUND)
    message(STATUS "✓ SDL2: FOUND")
else()
    message(WARNING "✗ SDL2: NOT FOUND (libsdl2-dev)")
endif()

find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(FREETYPE QUIET freetype2)
    if(FREETYPE_FOUND)
        message(STATUS "✓ FreeType2: FOUND")
    else()
        message(WARNING "✗ FreeType2: NOT FOUND (libfreetype6-dev)")
    endif()

    pkg_check_modules(OPENSSL QUIET openssl)
    if(OPENSSL_FOUND)
        message(STATUS "✓ OpenSSL: FOUND")
    else()
        message(WARNING "✗ OpenSSL: NOT FOUND (libssl-dev)")
    endif()

    pkg_check_modules(FFMPEG QUIET libavformat libavcodec libavutil libswscale)
    if(FFMPEG_FOUND)
        message(STATUS "✓ FFmpeg: FOUND")
    else()
        message(WARNING "✗ FFmpeg: NOT FOUND (libavformat-dev libavcodec-dev libavutil-dev libswscale-dev)")
    endif()
endif()

message(STATUS "============================================")
message(STATUS "")

set(LVGL_DIR "lvgl")
set(LV_DRIVERS_DIR "lvgl/src/drivers")
set(SQLCIPHER_DIR "sqlcipher")

# Create lib directory
set(LIB_DIR "${CMAKE_BINARY_DIR}/lib")
file(MAKE_DIRECTORY ${LIB_DIR})

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/${LVGL_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/${LVGL_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/${LVGL_DIR}/src/libs
    ${CMAKE_CURRENT_SOURCE_DIR}/${LVGL_DIR}/src/libs/freetype
    ${CMAKE_CURRENT_SOURCE_DIR}/${LV_DRIVERS_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/${LV_DRIVERS_DIR}/sdl
    ${CMAKE_CURRENT_SOURCE_DIR}/${SQLCIPHER_DIR}
)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Find FreeType
find_package(PkgConfig REQUIRED)
pkg_check_modules(FREETYPE REQUIRED freetype2)
include_directories(${FREETYPE_INCLUDE_DIRS})

# TLS Configuration - Using OpenSSL for better compatibility
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})
add_compile_definitions(MG_TLS=MG_TLS_OPENSSL)

# Use local ONNX Runtime installation
set(ONNXRUNTIME_DIR "${CMAKE_CURRENT_SOURCE_DIR}/onnxruntime")
include_directories(${ONNXRUNTIME_DIR}/include)

# Verify ONNX Runtime library architecture matches system
set(ONNXRUNTIME_LIB "${ONNXRUNTIME_DIR}/lib/libonnxruntime.so.1.16.3")
if(EXISTS ${ONNXRUNTIME_LIB})
    execute_process(
        COMMAND file ${ONNXRUNTIME_LIB}
        OUTPUT_VARIABLE ONNXRUNTIME_FILE_INFO
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if(ARCH_TYPE STREQUAL "aarch64")
        if(NOT ONNXRUNTIME_FILE_INFO MATCHES "ARM aarch64")
            message(FATAL_ERROR
                "ONNX Runtime library architecture mismatch!\n"
                "System is ARM64 but library is: ${ONNXRUNTIME_FILE_INFO}\n"
                "Please download the ARM64 version from:\n"
                "https://github.com/microsoft/onnxruntime/releases/download/v1.16.3/onnxruntime-linux-aarch64-1.16.3.tgz")
        endif()
    elseif(ARCH_TYPE STREQUAL "x86_64")
        if(NOT ONNXRUNTIME_FILE_INFO MATCHES "x86-64")
            message(FATAL_ERROR
                "ONNX Runtime library architecture mismatch!\n"
                "System is x86-64 but library is: ${ONNXRUNTIME_FILE_INFO}\n"
                "Please download the x86-64 version from:\n"
                "https://github.com/microsoft/onnxruntime/releases/download/v1.16.3/onnxruntime-linux-x64-1.16.3.tgz")
        endif()
    endif()
    message(STATUS "ONNX Runtime library architecture verified: ${ARCH_TYPE}")
else()
    message(FATAL_ERROR "ONNX Runtime library not found at: ${ONNXRUNTIME_LIB}")
endif()

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
    ${LVGL_DIR}/src/*.cpp
)

# Exclude all ThorVG files
list(FILTER LVGL_SOURCES EXCLUDE REGEX ".*thorvg/.*\\.cpp$")
list(FILTER LVGL_SOURCES EXCLUDE REGEX ".*thorvg/.*\\.c$")
file(GLOB LV_DRIVERS_SOURCES
    ${LV_DRIVERS_DIR}/sdl/*.c
)

# Create LVGL library
add_library(lvgl_lib STATIC ${LVGL_SOURCES} ${LV_DRIVERS_SOURCES})
target_include_directories(lvgl_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${LVGL_DIR}
    ${LVGL_DIR}/src
    ${LVGL_DIR}/src/libs
    ${LVGL_DIR}/src/libs/freetype
    ${LV_DRIVERS_DIR}
    ${LV_DRIVERS_DIR}/sdl
)

# Copy HTML files to build directory
add_custom_command(
    TARGET lvgl_lib POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/html
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/html ${CMAKE_BINARY_DIR}/html
    COMMENT "Copying HTML files to build directory"
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
    src/tab_qwerty.c
    src/tab_calendar.c
    src/tab_clock.c
    src/video.c
    src/tab_video.c
    src/audio.c
    src/clock.c
    src/calendar.c
    src/korean_input.c
    src/qwerty_korean.c
    src/chunjiin_input.c
    src/ui_callbacks.c
    src/web_server.c
    src/mongoose.c
    src/web_control.c
)

# SQLCipher amalgamation files
set(SQLCIPHER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/${SQLCIPHER_DIR}/sqlite3.c
)

# Add custom command to build SQLCipher with architecture check
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${SQLCIPHER_DIR}/sqlite3.c ${CMAKE_CURRENT_SOURCE_DIR}/${SQLCIPHER_DIR}/sqlite3.h
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/build_sqlcipher.sh
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Building SQLCipher amalgamation files with architecture verification for ${ARCH_TYPE}"
    VERBATIM
)

# Mark SQLCipher sources as generated
set_source_files_properties(
    ${CMAKE_CURRENT_SOURCE_DIR}/${SQLCIPHER_DIR}/sqlite3.c
    PROPERTIES GENERATED TRUE
)

# Create main executable
add_executable(main ${MAIN_SOURCES} ${SQLCIPHER_SOURCES})

# Add include directories
target_include_directories(main PRIVATE include sqlcipher sqlite lvgl lvgl/src/libs/svg lvgl/src/libs/ffmpeg .)

# Link against LVGL library and other dependencies
target_link_libraries(main lvgl_lib ${SDL2_LIBRARIES} ${FREETYPE_LIBRARIES} ${OPENSSL_LIBRARIES})



# Link ONNX Runtime library
target_link_libraries(main ${ONNXRUNTIME_DIR}/lib/libonnxruntime.so.1.16.3)

# Link FFmpeg libraries for video support
target_link_libraries(main avformat avcodec avutil swscale z)

if(UNIX)
    target_link_libraries(main pthread dl m crypto)
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
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/main
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/CMakeFiles/main.dir/src/*.o
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/CMakeFiles/main.dir/src/*.d
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${LIB_DIR}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Cleaning all build files"
)

# Add custom target to clean only main sources (keep LVGL library)
add_custom_target(clean-main
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/main
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/CMakeFiles/main.dir/src/*.o
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/CMakeFiles/main.dir/src/*.d
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Cleaning main application files (keeping LVGL library)"
)

# Test executable for ChunJiIn input testing
set(TEST_SOURCES
    src/test_chunjiin_input.c
    src/chunjiin_input.c
)

# Create test executable
add_executable(test_chunjiin ${TEST_SOURCES})

# Add include directories for test
target_include_directories(test_chunjiin PRIVATE include)

# Link against LVGL library and other dependencies
target_link_libraries(test_chunjiin lvgl_lib ${SDL2_LIBRARIES} ${FREETYPE_LIBRARIES})

# Link FFmpeg libraries for video support
target_link_libraries(test_chunjiin avformat avcodec avutil swscale z)

if(UNIX)
    target_link_libraries(test_chunjiin pthread dl m crypto)
endif()

# Calendar test executable
set(CALENDAR_TEST_SOURCES
    src/test_calendar.c
    src/calendar.c
)

# Create calendar test executable
add_executable(calendar_test ${CALENDAR_TEST_SOURCES})

# Add include directories for calendar test
target_include_directories(calendar_test PRIVATE include)

# Link against LVGL library and other dependencies
target_link_libraries(calendar_test lvgl_lib ${SDL2_LIBRARIES} ${FREETYPE_LIBRARIES})

# Link FFmpeg libraries for video support
target_link_libraries(calendar_test avformat avcodec avutil swscale z)

if(UNIX)
    target_link_libraries(calendar_test pthread dl m crypto)
endif()

# Clock test executable
set(CLOCK_TEST_SOURCES
    src/test_clock.c
    src/clock.c
)

# Create clock test executable
add_executable(clock_test ${CLOCK_TEST_SOURCES})

# Add include directories for clock test
target_include_directories(clock_test PRIVATE include)

# Link against LVGL library and other dependencies
target_link_libraries(clock_test lvgl_lib ${SDL2_LIBRARIES} ${FREETYPE_LIBRARIES})

# Link FFmpeg libraries for video support
target_link_libraries(clock_test avformat avcodec avutil swscale z)

if(UNIX)
    target_link_libraries(clock_test pthread dl m crypto)
endif()

# Qwerty Korean test executable
set(QWERTY_KOREAN_TEST_SOURCES
    src/test_qwerty_korea.c
    src/qwerty_korean.c
)

# Create qwerty Korean test executable
add_executable(qwerty_korean_test ${QWERTY_KOREAN_TEST_SOURCES})

# Add include directories for qwerty Korean test
target_include_directories(qwerty_korean_test PRIVATE include)

# Link against LVGL library and other dependencies
target_link_libraries(qwerty_korean_test lvgl_lib ${SDL2_LIBRARIES} ${FREETYPE_LIBRARIES})

# Link FFmpeg libraries for video support
target_link_libraries(qwerty_korean_test avformat avcodec avutil swscale z)

if(UNIX)
    target_link_libraries(qwerty_korean_test pthread dl m crypto)
endif()

# Video test executable
set(VIDEO_TEST_SOURCES
    src/test_video.c
)

# Create video test executable
add_executable(video_test ${VIDEO_TEST_SOURCES})

# Add include directories for video test
target_include_directories(video_test PRIVATE include lvgl/src/libs/ffmpeg)

# Link against LVGL library and other dependencies
target_link_libraries(video_test lvgl_lib ${SDL2_LIBRARIES} ${FREETYPE_LIBRARIES})

# Link FFmpeg libraries for video support
target_link_libraries(video_test avformat avcodec avutil swscale z)

if(UNIX)
    target_link_libraries(video_test pthread dl m crypto)
endif()




