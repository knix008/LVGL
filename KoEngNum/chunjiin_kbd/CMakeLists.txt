cmake_minimum_required(VERSION 3.10)
project(chunjiin_keyboard C)

set(CMAKE_C_STANDARD 99)

# LVGL directory
set(LVGL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lvgl")

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${LVGL_DIR}
    ${LVGL_DIR}/src
    ${LVGL_DIR}/src/libs
    ${LVGL_DIR}/src/libs/freetype
    ${LVGL_DIR}/src/drivers
    ${LVGL_DIR}/src/drivers/sdl
)

# Find SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Find FreeType
find_package(PkgConfig REQUIRED)
pkg_check_modules(FREETYPE REQUIRED freetype2)
include_directories(${FREETYPE_INCLUDE_DIRS})

# LVGL sources
file(GLOB_RECURSE LVGL_SOURCES "${LVGL_DIR}/src/*.c")
file(GLOB LVGL_DRIVER_SOURCES "${LVGL_DIR}/src/drivers/sdl/*.c")

# Application sources
set(APP_SOURCES
    src/main.c
    src/chunjiin_input.c
    src/english_input.c
    src/number_input.c
    src/keyboard_ui.c
)

# Create executable
add_executable(chunjiin_keyboard
    ${APP_SOURCES}
    ${LVGL_SOURCES}
    ${LVGL_DRIVER_SOURCES}
)

# Link libraries
target_link_libraries(chunjiin_keyboard
    ${SDL2_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    m
    pthread
    dl
)

# Installation
install(TARGETS chunjiin_keyboard DESTINATION bin)
