cmake_minimum_required(VERSION 3.16)
project(DeviceTest VERSION 1.0.0 LANGUAGES C CXX)

# Set C and C++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Find required packages
find_package(PkgConfig REQUIRED)

# Find OpenCV for camera functionality using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(OpenCV REQUIRED opencv4)

# Find ALSA for speaker functionality
pkg_check_modules(ALSA REQUIRED alsa)

# Find Bluetooth libraries
pkg_check_modules(BLUETOOTH REQUIRED bluez)

# GTK is not used in this project - removed dependency

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${ALSA_INCLUDE_DIRS})
include_directories(${BLUETOOTH_INCLUDE_DIRS})

# Source files
file(GLOB_RECURSE C_SOURCES "src/*.c")
file(GLOB_RECURSE CPP_SOURCES "src/*.cpp")
set(SOURCES ${C_SOURCES} ${CPP_SOURCES})

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBRARIES} ${ALSA_LIBRARIES} ${BLUETOOTH_LIBRARIES} pthread)
target_compile_options(${PROJECT_NAME} PRIVATE ${OpenCV_CFLAGS_OTHER} ${ALSA_CFLAGS_OTHER} ${BLUETOOTH_CFLAGS_OTHER})

# Compiler flags - suppress warnings
target_compile_options(${PROJECT_NAME} PRIVATE -w)

# Install target
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
