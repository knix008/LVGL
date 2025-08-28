cmake_minimum_required(VERSION 3.10)
project(main C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 14)

# Project directories
set(LVGL_DIR "lvgl")

# Pre-built libraries from lib directory
set(SDL2_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/lib/include/SDL2)
set(SDL2_LIBRARIES ${CMAKE_CURRENT_SOURCE_DIR}/lib/lib/libSDL2.a)
set(FREETYPE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/lib/include/freetype2)
set(FREETYPE_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/lib/lib/libfreetype.a)
set(LVGL_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/lib/lib/liblvgl_lib.a)
set(OPENSSL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/include)
set(OPENSSL_LIBRARIES ${CMAKE_CURRENT_SOURCE_DIR}/lib/lib/libcrypto.a ${CMAKE_CURRENT_SOURCE_DIR}/lib/lib/libssl.a)
set(MARIADB_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/lib/lib/libmariadb.a)

# System libraries
find_package(PkgConfig REQUIRED)
pkg_check_modules(FFMPEG REQUIRED libavformat libavcodec libavutil libswscale libswresample)

# Compile definitions
add_compile_definitions(
    MG_TLS=MG_TLS_OPENSSL
)

# Main application sources
set(MAIN_SOURCES
    src/main.c
    src/mariadb_test.c
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
    src/tab_webcam.c
)

# Create main executable
add_executable(main ${MAIN_SOURCES})

# Include directories for main
target_include_directories(main PRIVATE 
    include 
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/include
    ${CMAKE_CURRENT_SOURCE_DIR}/lvgl
    ${CMAKE_CURRENT_SOURCE_DIR}/lvgl/src
    ${CMAKE_CURRENT_SOURCE_DIR}/lvgl/src/libs
    ${CMAKE_CURRENT_SOURCE_DIR}/lvgl/src/libs/freetype
    ${CMAKE_CURRENT_SOURCE_DIR}/lvgl/src/libs/ffmpeg
    ${CMAKE_CURRENT_SOURCE_DIR}/lvgl/src/drivers
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/include/lvgl_drivers
    ${FREETYPE_INCLUDE_DIRS} 
    ${SDL2_INCLUDE_DIRS} 
    ${FFMPEG_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/include/mysql
)

# Link libraries for main
target_link_libraries(main 
    ${LVGL_LIBRARY} 
    ${SDL2_LIBRARIES} 
    ${FREETYPE_LIBS} 
    ${OPENSSL_LIBRARIES}
    ${FFMPEG_LIBRARIES}
    ${MARIADB_LIBRARY}
)

# System libraries for Unix
if(UNIX)
    target_link_libraries(main pthread dl m crypto z)
endif()

# Copy HTML files to build directory
add_custom_command(
    TARGET main PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/html
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/html ${CMAKE_BINARY_DIR}/html
    COMMENT "Copying HTML files to build directory"
)

