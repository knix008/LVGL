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

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Find FreeType
find_package(PkgConfig REQUIRED)
pkg_check_modules(FREETYPE REQUIRED freetype2)
include_directories(${FREETYPE_INCLUDE_DIRS})

# Find OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

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

    src/svg_example.c
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

# Webcam IPC Application
set(WEBCAM_IPC_SOURCES
    src/webcam_ipc_app.cpp
)

# Create webcam IPC executable
add_executable(webcam_ipc_app ${WEBCAM_IPC_SOURCES})

# Add include directories for webcam IPC
target_include_directories(webcam_ipc_app PRIVATE include onnxruntime/include)

# Link against OpenCV and ONNX Runtime
target_link_libraries(webcam_ipc_app ${OpenCV_LIBS} ${ONNXRUNTIME_DIR}/lib/libonnxruntime.so.1.16.3)

if(UNIX)
    target_link_libraries(webcam_ipc_app pthread)
endif()




