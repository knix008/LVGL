cmake_minimum_required(VERSION 3.10)
project(lvgl_lib C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directory for the library
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../lib)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers
    ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/sdl
)

# Find SDL2 for drivers
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Add compile definitions
add_compile_definitions($<$<BOOL:${LV_USE_DRAW_SDL}>:LV_USE_DRAW_SDL=1>)

# Collect all LVGL source files (C and C++)
file(GLOB_RECURSE LVGL_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
)

# Collect ThorVG source files separately
file(GLOB_RECURSE THORVG_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/libs/thorvg/*.cpp
)

# Exclude problematic ThorVG files that require OpenGL
list(REMOVE_ITEM THORVG_SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/libs/thorvg/tvgGlCanvas.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/libs/thorvg/tvgWgCanvas.cpp
)

# Collect SDL driver source files
file(GLOB LV_DRIVERS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/sdl/*.c
)

# Create static library
add_library(lvgl STATIC ${LVGL_SOURCES} ${LV_DRIVERS_SOURCES})

# Create ThorVG library
add_library(lvgl_thorvg STATIC ${THORVG_SOURCES})
target_include_directories(lvgl_thorvg PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/libs/thorvg)

# Link ThorVG and SDL2 to LVGL
target_link_libraries(lvgl PRIVATE lvgl_thorvg ${SDL2_LIBRARIES})

# Set library properties
set_target_properties(lvgl PROPERTIES
    OUTPUT_NAME "lvgl"
    POSITION_INDEPENDENT_CODE ON
)

# Install target (optional)
install(TARGETS lvgl
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)
