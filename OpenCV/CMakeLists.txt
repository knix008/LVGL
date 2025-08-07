cmake_minimum_required(VERSION 3.16)
project(YOLOv8Detection)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Find OpenCV from lib directory
set(OpenCV_DIR ${CMAKE_SOURCE_DIR}/src/lib/lib/cmake/opencv4)
find_package(OpenCV REQUIRED)

# Find Eigen3
find_package(Eigen3 REQUIRED)

# Add executable
add_executable(yolov8n_detection 
    src/main.cpp
    src/yolov8n_detector.cpp
)

# Include directories
target_include_directories(yolov8n_detection PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}
)

# Link libraries
target_link_libraries(yolov8n_detection 
    ${CMAKE_SOURCE_DIR}/src/lib/lib/libopencv_world.a
    ${CMAKE_SOURCE_DIR}/src/lib/lib/opencv4/3rdparty/liblibprotobuf.a
    ${CMAKE_SOURCE_DIR}/src/lib/lib/opencv4/3rdparty/libade.a
    ${CMAKE_SOURCE_DIR}/src/lib/lib/opencv4/3rdparty/libittnotify.a
    ${CMAKE_SOURCE_DIR}/src/lib/lib/opencv4/3rdparty/liblibwebp.a
    jpeg
    png
    z
)

# Set output directory
set_target_properties(yolov8n_detection PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Print OpenCV information
message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "    libraries: ${OpenCV_LIBS}")

# Compiler-specific options
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(yolov8n_detection PRIVATE -Wall -Wextra -O2)
endif()

if(MSVC)
    target_compile_options(yolov8n_detection PRIVATE /W4)
endif()
