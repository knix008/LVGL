cmake_minimum_required(VERSION 3.10)
project(lvgl_main C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 14)

# Set paths
set(LVGL_DIR "lvgl")
set(LV_DRIVERS_DIR "lvgl/src/drivers")
set(LIB_DIR "lib")

# Include directories for headers
include_directories(
    ${LVGL_DIR}
    ${LVGL_DIR}/src
    ${LV_DRIVERS_DIR}
    ${LV_DRIVERS_DIR}/sdl
    include
)

# Use our custom lv_conf.h instead of LVGL's default
add_compile_definitions(LV_CONF_INCLUDE_SIMPLE=1)

# Find SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Add compile definitions
add_compile_definitions($<$<BOOL:${LV_USE_DRAW_SDL}>:LV_USE_DRAW_SDL=1>)

# Create the main executable
add_executable(lvgl_main ./src/main.c)

# Set library directory and link with the pre-built LVGL library
target_link_directories(lvgl_main PRIVATE ${CMAKE_SOURCE_DIR}/${LIB_DIR})
target_link_libraries(lvgl_main 
    lvgl
    lvgl_thorvg
    ${SDL2_LIBRARIES}
    m
)

if(UNIX)
    target_link_libraries(lvgl_main pthread)
endif()