cmake_minimum_required(VERSION 3.10)
project(webcam_app CXX)

# Build configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")

# Find packages
find_package(PkgConfig REQUIRED)

# OpenCV configuration
set(OpenCV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/opencv/lib/cmake/opencv4")
set(OpenCV_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/opencv/include/opencv4")
set(OpenCV_LIBS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/opencv/lib")

find_package(OpenCV REQUIRED)
if(NOT OpenCV_FOUND)
    message(FATAL_ERROR "Local OpenCV not found. Please ensure OpenCV is built in the opencv directory.")
endif()

if(OpenCV_VERSION VERSION_LESS "4.0.0")
    message(WARNING "OpenCV version ${OpenCV_VERSION} is older than 4.0.0. Some features may not work properly.")
endif()

# ONNX Runtime configuration
set(ONNXRUNTIME_DIR "${CMAKE_CURRENT_SOURCE_DIR}/onnxruntime-linux-x64-1.16.3")

if(NOT EXISTS "${ONNXRUNTIME_DIR}/lib/libonnxruntime.so")
    message(FATAL_ERROR "ONNX Runtime prebuilt library not found at ${ONNXRUNTIME_DIR}/lib/libonnxruntime.so")
endif()

if(NOT EXISTS "${ONNXRUNTIME_DIR}/include/onnxruntime_c_api.h")
    message(FATAL_ERROR "ONNX Runtime headers not found at ${ONNXRUNTIME_DIR}/include/")
endif()

# Check model files
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/models/yolov8n-face.onnx")
    message(WARNING "YOLOv8 face detection model not found. Please ensure models/yolov8n-face.onnx exists.")
else()
    message(STATUS "✓ YOLOv8 face detection model found")
endif()

# Main executable
add_executable(webcam_app src/main.cpp src/webcam.cpp)

# Include directories
target_include_directories(webcam_app PRIVATE 
    include 
    ${OpenCV_INCLUDE_DIRS}
    ${ONNXRUNTIME_DIR}/include
)

# Link libraries
target_link_libraries(webcam_app 
    ${ONNXRUNTIME_DIR}/lib/libonnxruntime.so
    ${OpenCV_LIBS}
    pthread dl rt m stdc++
)

# RPATH configuration
set_target_properties(webcam_app PROPERTIES
    INSTALL_RPATH "${OpenCV_LIBS_DIR}:${ONNXRUNTIME_DIR}/lib"
    BUILD_WITH_INSTALL_RPATH TRUE
    SKIP_BUILD_RPATH FALSE
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Compile definitions
target_compile_definitions(webcam_app PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
    VERSION_MAJOR=1 VERSION_MINOR=0 VERSION_PATCH=0
)

# Copy model files
add_custom_command(
    TARGET webcam_app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/models
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/models ${CMAKE_BINARY_DIR}/models
    COMMENT "Copying model files to build directory"
)

# Custom targets
add_custom_target(run-webcam
    COMMAND ${CMAKE_COMMAND} -E echo "Starting webcam application..."
    COMMAND ${CMAKE_COMMAND} -E env LD_LIBRARY_PATH=${OpenCV_LIBS_DIR}:${ONNXRUNTIME_DIR}/lib:$ENV{LD_LIBRARY_PATH} ${CMAKE_BINARY_DIR}/webcam_app
    DEPENDS webcam_app
    COMMENT "Running webcam application"
)

add_custom_target(test_webcam
    COMMAND ${CMAKE_COMMAND} -E echo "Testing webcam application..."
    COMMAND ${CMAKE_COMMAND} -E echo "Run: make run-webcam"
    DEPENDS webcam_app
)

add_custom_target(clean-build
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target webcam_app
    COMMENT "Clean build completed"
)

add_custom_target(info
    COMMAND ${CMAKE_COMMAND} -E echo "=== Build Information ==="
    COMMAND ${CMAKE_COMMAND} -E echo "Build directory: ${CMAKE_BINARY_DIR}"
    COMMAND ${CMAKE_COMMAND} -E echo "OpenCV: ${OpenCV_LIBS_DIR}"
    COMMAND ${CMAKE_COMMAND} -E echo "ONNX Runtime: ${ONNXRUNTIME_DIR}/lib"
    COMMAND ${CMAKE_COMMAND} -E echo "Models: ${CMAKE_CURRENT_SOURCE_DIR}/models"
    COMMAND ${CMAKE_COMMAND} -E echo "Executable: ${CMAKE_BINARY_DIR}/webcam_app"
    COMMAND ${CMAKE_COMMAND} -E echo "To run: make run-webcam"
    COMMAND ${CMAKE_COMMAND} -E echo "========================"
    COMMENT "Displaying build information"
)

# Install targets
install(TARGETS webcam_app RUNTIME DESTINATION bin)
install(DIRECTORY models/ DESTINATION share/webcam_app/models FILES_MATCHING PATTERN "*.onnx" PATTERN "*.pt")

# Configuration summary
message(STATUS "=== Webcam Application Configuration ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "ONNX Runtime: ${ONNXRUNTIME_DIR} (1.16.3)")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "=========================================")
