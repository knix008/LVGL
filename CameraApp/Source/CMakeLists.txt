cmake_minimum_required(VERSION 3.10)
project(webcam_app CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing
enable_testing()

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")

# Find required packages
find_package(PkgConfig REQUIRED)

# Find OpenCV with better error handling
find_package(OpenCV REQUIRED)
if(NOT OpenCV_FOUND)
    message(FATAL_ERROR "OpenCV not found. Please install OpenCV development libraries.")
endif()

# Check OpenCV version
if(OpenCV_VERSION VERSION_LESS "4.0.0")
    message(WARNING "OpenCV version ${OpenCV_VERSION} is older than 4.0.0. Some features may not work properly.")
endif()

# ONNX Runtime path
set(ONNXRUNTIME_DIR "${CMAKE_CURRENT_SOURCE_DIR}/onnxruntime")

# Check if ONNX Runtime exists
if(NOT EXISTS "${ONNXRUNTIME_DIR}/lib/libonnxruntime.so.1.16.3")
    message(FATAL_ERROR "ONNX Runtime library not found at ${ONNXRUNTIME_DIR}/lib/libonnxruntime.so.1.16.3")
endif()

if(NOT EXISTS "${ONNXRUNTIME_DIR}/include/onnxruntime_cxx_api.h")
    message(FATAL_ERROR "ONNX Runtime headers not found at ${ONNXRUNTIME_DIR}/include/")
endif()

# Check if model files exist
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/models/yolov8n-face.onnx")
    message(WARNING "YOLOv8 face detection model not found. Please ensure models/yolov8n-face.onnx exists.")
else()
    message(STATUS "✓ YOLOv8 face detection model found")
endif()

# Main Webcam Application
add_executable(webcam_app src/main.cpp src/webcam.cpp)

# Include directories
target_include_directories(webcam_app PRIVATE 
    include 
    ${OpenCV_INCLUDE_DIRS}
    ${ONNXRUNTIME_DIR}/include
)

# Link libraries
target_link_libraries(webcam_app 
    ${OpenCV_LIBS} 
    ${ONNXRUNTIME_DIR}/lib/libonnxruntime.so.1.16.3
)

# System-specific libraries
if(UNIX)
    target_link_libraries(webcam_app pthread dl)
    
    # Check for additional system libraries
    find_library(M_LIBRARY m)
    if(M_LIBRARY)
        target_link_libraries(webcam_app ${M_LIBRARY})
    endif()
endif()

# Compile definitions
target_compile_definitions(webcam_app PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
    VERSION_MAJOR=1
    VERSION_MINOR=0
    VERSION_PATCH=0
)

# Set output directory
set_target_properties(webcam_app PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Copy model files to build directory
add_custom_command(
    TARGET webcam_app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/models
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/models ${CMAKE_BINARY_DIR}/models
    COMMENT "Copying model files to build directory"
)

# Create a simple test target
add_custom_target(test_webcam
    COMMAND ${CMAKE_COMMAND} -E echo "Testing webcam application..."
    COMMAND ${CMAKE_COMMAND} -E echo "Run: ./webcam_app --model models/yolov8n-face.onnx"
    DEPENDS webcam_app
)

# Install target
install(TARGETS webcam_app
    RUNTIME DESTINATION bin
)

install(DIRECTORY models/
    DESTINATION share/webcam_app/models
    FILES_MATCHING PATTERN "*.onnx" PATTERN "*.pt"
)

# Print configuration summary
message(STATUS "=== Webcam Application Configuration ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "ONNX Runtime: ${ONNXRUNTIME_DIR}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "=========================================")

# Add custom targets for common operations
add_custom_target(clean-build
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target webcam_app
    COMMENT "Clean build completed"
)

add_custom_target(run-webcam
    COMMAND ${CMAKE_COMMAND} -E echo "Starting webcam application..."
    COMMAND ${CMAKE_BINARY_DIR}/webcam_app --model models/yolov8n-face.onnx
    DEPENDS webcam_app
    COMMENT "Running webcam application"
)
