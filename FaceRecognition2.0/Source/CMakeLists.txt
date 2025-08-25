cmake_minimum_required(VERSION 3.10)
project(main C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 14)

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

# Use pre-built SDL2 from lib directory
set(SDL2_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/lib/include/SDL2)
set(SDL2_LIBRARIES ${CMAKE_CURRENT_SOURCE_DIR}/lib/lib/libSDL2.a)

# Use pre-built FreeType from lib directory
set(FREETYPE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/lib/include/freetype2)
set(FREETYPE_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/lib/lib/libfreetype.a)

# Use system FFmpeg libraries
find_package(PkgConfig REQUIRED)
pkg_check_modules(FFMPEG REQUIRED libavformat libavcodec libavutil libswscale)

# Set FFmpeg variables for our project
set(FFMPEG_INCLUDE_DIRS ${FFMPEG_INCLUDE_DIRS})
set(FFMPEG_LIBS ${FFMPEG_LIBRARIES})

# Use system OpenCV for webcam IPC app only
find_package(OpenCV REQUIRED)

# TLS Configuration - Using OpenSSL for better compatibility
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})
add_compile_definitions(MG_TLS=MG_TLS_OPENSSL)

# Use local ONNX Runtime installation
set(ONNXRUNTIME_DIR "${CMAKE_CURRENT_SOURCE_DIR}/onnxruntime")
include_directories(${ONNXRUNTIME_DIR}/include)

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

# Exclude problematic ThorVG files
list(FILTER LVGL_SOURCES EXCLUDE REGEX ".*tvgGl.*\\.cpp$")
list(FILTER LVGL_SOURCES EXCLUDE REGEX ".*tvgWg.*\\.cpp$")
list(FILTER LVGL_SOURCES EXCLUDE REGEX ".*thorvg.*\\.cpp$")
list(FILTER LVGL_SOURCES EXCLUDE REGEX ".*thorvg.*\\.h$")
file(GLOB LV_DRIVERS_SOURCES
    ${LV_DRIVERS_DIR}/sdl/*.c
)

# Use pre-built LVGL library from lib directory
set(LVGL_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/lib/lib/liblvgl_lib.a)

# Main application sources
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
    src/web_server.c
    src/mongoose.c
    src/web_control.c
    src/tab_opencv.c
)

# SQLCipher amalgamation files
set(SQLCIPHER_SOURCES
    ${SQLCIPHER_DIR}/sqlite3.c
)

# Create main executable
add_executable(main ${MAIN_SOURCES} ${SQLCIPHER_SOURCES})

# Add include directories
target_include_directories(main PRIVATE include sqlcipher sqlite lvgl lvgl/src/libs/svg lvgl/src/libs/ffmpeg . ${FREETYPE_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} ${FFMPEG_INCLUDE_DIRS})

# No custom build dependencies needed for system libraries

# Link against LVGL library and other dependencies
target_link_libraries(main ${LVGL_LIBRARY} ${SDL2_LIBRARIES} ${FREETYPE_LIBS} ${OPENSSL_LIBRARIES})

# Copy HTML files to build directory
add_custom_command(
    TARGET main PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/html
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/html ${CMAKE_BINARY_DIR}/html
    COMMENT "Copying HTML files to build directory"
)

# Main app doesn't use OpenCV

# Link ONNX Runtime library
target_link_libraries(main ${ONNXRUNTIME_DIR}/lib/libonnxruntime.so.1.16.3)

# Link FFmpeg libraries for video support
target_link_libraries(main ${FFMPEG_LIBS})

if(UNIX)
    target_link_libraries(main pthread dl m crypto)
endif()

# Webcam IPC Application
set(WEBCAM_IPC_SOURCES
    src/webcam_ipc_app.cpp
)

# Create webcam IPC executable
add_executable(webcam_ipc_app ${WEBCAM_IPC_SOURCES})

# Add include directories for webcam IPC
target_include_directories(webcam_ipc_app PRIVATE include onnxruntime/include ${OpenCV_INCLUDE_DIRS})

# Link against system OpenCV and ONNX Runtime
target_link_libraries(webcam_ipc_app ${OpenCV_LIBS} ${ONNXRUNTIME_DIR}/lib/libonnxruntime.so.1.16.3)

if(UNIX)
    target_link_libraries(webcam_ipc_app pthread)
endif()