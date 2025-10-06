# ==============================================================================
# Korean/English/Number QWERTY Keypad - CMake Build Configuration
# ==============================================================================

cmake_minimum_required(VERSION 3.10)
project(KorEngNumQWerty
    VERSION 1.0.0
    DESCRIPTION "Korean/English/Number QWERTY Virtual Keyboard"
    LANGUAGES C
)

# ==============================================================================
# Build Configuration
# ==============================================================================

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")

# ==============================================================================
# Directory Paths
# ==============================================================================

set(LVGL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lvgl")
set(LV_DRIVERS_DIR "${LVGL_DIR}/src/drivers")
set(LIB_DIR "${CMAKE_BINARY_DIR}/lib")

# Create output directories
file(MAKE_DIRECTORY ${LIB_DIR})

# ==============================================================================
# Dependencies - SDL2
# ==============================================================================

find_package(SDL2 REQUIRED)
if(SDL2_FOUND)
    message(STATUS "Found SDL2: ${SDL2_LIBRARIES}")
endif()

# ==============================================================================
# Dependencies - FreeType
# ==============================================================================

find_package(PkgConfig REQUIRED)
pkg_check_modules(FREETYPE REQUIRED freetype2)
if(FREETYPE_FOUND)
    message(STATUS "Found FreeType: ${FREETYPE_LIBRARIES}")
endif()

# ==============================================================================
# LVGL Library Build
# ==============================================================================

# Collect LVGL source files
file(GLOB_RECURSE LVGL_SOURCES
    ${LVGL_DIR}/src/*.c
)

# Collect LVGL SDL driver sources
file(GLOB LV_DRIVERS_SOURCES
    ${LV_DRIVERS_DIR}/sdl/*.c
)

# Create LVGL static library
add_library(lvgl_lib STATIC
    ${LVGL_SOURCES}
    ${LV_DRIVERS_SOURCES}
)

# LVGL include directories
target_include_directories(lvgl_lib PUBLIC
    ${LVGL_DIR}
    ${LVGL_DIR}/src
    ${LVGL_DIR}/src/libs
    ${LVGL_DIR}/src/libs/freetype
    ${LV_DRIVERS_DIR}
    ${LV_DRIVERS_DIR}/sdl
    ${SDL2_INCLUDE_DIRS}
    ${FREETYPE_INCLUDE_DIRS}
)

# LVGL compile definitions
target_compile_definitions(lvgl_lib PUBLIC
    $<$<BOOL:${LV_USE_DRAW_SDL}>:LV_USE_DRAW_SDL=1>
)

# Set LVGL library output directory
set_target_properties(lvgl_lib PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${LIB_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${LIB_DIR}
)

# ==============================================================================
# Application Build
# ==============================================================================

# Application source files
set(APP_SOURCES
    src/main.c
    src/ui_components.c
    src/qwerty.c
    src/qwerty_korean.c
)

# Application header files (for IDE)
set(APP_HEADERS
    include/ui_components.h
    include/ui_callbacks.h
    include/qwerty.h
    include/qwerty_korean.h
    include/font_config.h
)

# Create main executable
add_executable(main ${APP_SOURCES} ${APP_HEADERS})

# Application include directories
target_include_directories(main PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${LVGL_DIR}
)

# Link libraries
target_link_libraries(main PRIVATE
    lvgl_lib
    ${SDL2_LIBRARIES}
    ${FREETYPE_LIBRARIES}
)

# Platform-specific libraries
if(UNIX AND NOT APPLE)
    target_link_libraries(main PRIVATE pthread dl m)
elseif(APPLE)
    target_link_libraries(main PRIVATE pthread dl)
endif()

# Set executable output name
set_target_properties(main PROPERTIES
    OUTPUT_NAME "KorEngNumQWerty"
)

# ==============================================================================
# Custom Targets
# ==============================================================================

# Clean all build files including LVGL library
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/CMakeFiles
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/CMakeCache.txt
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/Makefile
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/main
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/KorEngNumQWerty
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${LIB_DIR}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Cleaning all build files"
)

# Clean only main executable (keep LVGL library)
add_custom_target(clean-main
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/main
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/KorEngNumQWerty
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Cleaning main executable only (keeping LVGL library)"
)

# Rebuild main executable
add_custom_target(rebuild-main
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target clean-main
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target main
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Rebuilding main executable"
)

# ==============================================================================
# Installation (Optional)
# ==============================================================================

install(TARGETS main
    RUNTIME DESTINATION bin
)

# Install fonts
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../assets/
    DESTINATION share/KorEngNumQWerty/assets
    FILES_MATCHING PATTERN "*.ttf"
)

# ==============================================================================
# Build Summary
# ==============================================================================

message(STATUS "")
message(STATUS "==================================================")
message(STATUS "  Korean/English/Number QWERTY Keypad")
message(STATUS "==================================================")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "  Install Prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "==================================================")
message(STATUS "")
