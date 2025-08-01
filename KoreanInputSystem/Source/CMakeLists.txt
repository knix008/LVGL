cmake_minimum_required(VERSION 3.10)
project(main C)

set(CMAKE_C_STANDARD 99)

# Force rebuild on every make
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "")

set(LVGL_DIR "lvgl")
set(LV_DRIVERS_DIR "lvgl/src/drivers")

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
)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Find FreeType
find_package(PkgConfig REQUIRED)
pkg_check_modules(FREETYPE REQUIRED freetype2)
include_directories(${FREETYPE_INCLUDE_DIRS})

add_compile_definitions($<$<BOOL:${LV_USE_DRAW_SDL}>:LV_USE_DRAW_SDL=1>)

# Create LVGL static library
file(GLOB_RECURSE LVGL_SOURCES
    ${LVGL_DIR}/src/*.c
)
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

# Set output directory for LVGL library
set_target_properties(lvgl_lib PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${LIB_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${LIB_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${LIB_DIR}
)

# Main application source files
set(MAIN_SOURCES
    src/main.c
    src/ui_components.c
    src/ui_callbacks.c
    src/tab_info.c
    src/tab_number.c
    src/tab_korean.c
    src/tab_chunjiin.c
    src/tab_qwerty.c
    src/korean_input.c
    src/qwerty_korean.c
    src/chunjiin_input.c
)

# Create main executable
add_executable(main ${MAIN_SOURCES})

# Add include directories for main executable
target_include_directories(main PRIVATE include lvgl)

# Link against LVGL library and other dependencies
target_link_libraries(main lvgl_lib ${SDL2_LIBRARIES} ${FREETYPE_LIBRARIES})

if(UNIX)
    target_link_libraries(main pthread dl m)
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

# Test executables
# ChunJiIn input test
set(TEST_CHUNJIIN_SOURCES
    src/test_chunjiin_input.c
    src/chunjiin_input.c
)

add_executable(test_chunjiin ${TEST_CHUNJIIN_SOURCES})
target_include_directories(test_chunjiin PRIVATE include)
target_link_libraries(test_chunjiin lvgl_lib ${SDL2_LIBRARIES} ${FREETYPE_LIBRARIES})

if(UNIX)
    target_link_libraries(test_chunjiin pthread dl m)
endif()

# ChunJiIn input simple test








# Print build information
message(STATUS "Building Korean Input System")
message(STATUS "Main executable: main")
message(STATUS "Test executables: test_chunjiin")
message(STATUS "LVGL library will be built in: ${LIB_DIR}")