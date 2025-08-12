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

# Find SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Add compile definitions
add_compile_definitions($<$<BOOL:${LV_USE_DRAW_SDL}>:LV_USE_DRAW_SDL=1>)

# Collect SDL driver source files
file(GLOB SDL_DRIVER_SOURCES
    ${LV_DRIVERS_DIR}/sdl/*.c
)

# Create the main executable
add_executable(main ./src/main.c ${SDL_DRIVER_SOURCES})

# Set library directory and link with the pre-built LVGL library
target_link_directories(main PRIVATE ${CMAKE_SOURCE_DIR}/${LIB_DIR})
target_link_libraries(main 
    lvgl
    lvgl_thorvg
    ${SDL2_LIBRARIES}
    m
)

if(UNIX)
    target_link_libraries(main pthread)
endif()