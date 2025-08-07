cmake_minimum_required(VERSION 3.10)
project(YOLOv8Detection)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenCV
find_package(OpenCV REQUIRED)

# Include directories
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(include)

# Add executable
add_executable(yolo_detection 
    src/main.cpp
    src/yolov8n_detector.cpp
)

# Link libraries
target_link_libraries(yolo_detection ${OpenCV_LIBS})

# Set output directory
set_target_properties(yolo_detection PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Print OpenCV information
message(STATUS "OpenCV library status:")
message(STATUS "    config: ${OpenCV_DIR}")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

# Compiler-specific options
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(yolo_detection PRIVATE -Wall -Wextra -O2)
endif()

if(MSVC)
    target_compile_options(yolo_detection PRIVATE /W4)
endif()
