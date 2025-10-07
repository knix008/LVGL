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
# LVGL Library Configuration
# ==============================================================================

# Use local LVGL directory
set(LVGL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lvgl")

# Check if LVGL directory exists
if(NOT EXISTS "${LVGL_DIR}")
    message(FATAL_ERROR "LVGL directory not found at ${LVGL_DIR}")
endif()

# Add LVGL as subdirectory
add_subdirectory("${LVGL_DIR}" lvgl_build)

# Configure LVGL with FreeType
target_include_directories(lvgl PUBLIC
    ${FREETYPE_INCLUDE_DIRS}
)

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
    # Application include directories
target_include_directories(main PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/lvgl/src
    ${CMAKE_CURRENT_SOURCE_DIR}/lvgl/src/drivers
    ${CMAKE_CURRENT_SOURCE_DIR}/lvgl/src/libs/freetype
)
)

# Link libraries
target_link_libraries(main PRIVATE
    lvgl
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
# Install Configuration
# ==============================================================================

install(TARGETS main
    RUNTIME DESTINATION bin
)

# ==============================================================================
# Development Helpers
# ==============================================================================

# Print configuration summary
message(STATUS "")
message(STATUS "==== Build Configuration ====")
message(STATUS "Project: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C Standard: ${CMAKE_C_STANDARD}")
message(STATUS "LVGL Directory: ${LVGL_DIR}")
message(STATUS "SDL2 Libraries: ${SDL2_LIBRARIES}")
message(STATUS "FreeType Libraries: ${FREETYPE_LIBRARIES}")
message(STATUS "===============================")
message(STATUS "")