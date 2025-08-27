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

# Set OpenCV to use local static installation
set(OpenCV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/opencv/lib/cmake/opencv4")
set(OpenCV_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/opencv/include/opencv4")
set(OpenCV_LIBS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/opencv/lib")

# Force OpenCV to use static libraries
set(BUILD_SHARED_LIBS OFF)
set(OpenCV_STATIC ON)

# Find OpenCV with better error handling
find_package(OpenCV REQUIRED)
if(NOT OpenCV_FOUND)
    message(FATAL_ERROR "Local OpenCV not found. Please ensure OpenCV is built in the opencv directory.")
endif()

# Check OpenCV version
if(OpenCV_VERSION VERSION_LESS "4.0.0")
    message(WARNING "OpenCV version ${OpenCV_VERSION} is older than 4.0.0. Some features may not work properly.")
endif()

message(STATUS "Using OpenCV version: ${OpenCV_VERSION}")
message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")
message(STATUS "OpenCV include dirs: ${OpenCV_INCLUDE_DIRS}")

# Set ONNX Runtime directory
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

# Find system libraries needed for static linking
find_library(JPEG_LIBRARY NAMES jpeg libjpeg)
find_library(PNG_LIBRARY NAMES png libpng)
find_library(TIFF_LIBRARY NAMES tiff libtiff)
find_library(OPENJP2_LIBRARY NAMES openjp2 libopenjp2 libopenjp2.so.7)
if(NOT OPENJP2_LIBRARY)
    set(OPENJP2_LIBRARY "/usr/lib/x86_64-linux-gnu/libopenjp2.so.7")
endif()
find_library(ZLIB_LIBRARY NAMES z libz)
find_library(THREADS_LIBRARY NAMES pthread)

# Check if required libraries are found
if(NOT JPEG_LIBRARY)
    message(FATAL_ERROR "JPEG library not found")
endif()
if(NOT PNG_LIBRARY)
    message(FATAL_ERROR "PNG library not found")
endif()
if(NOT TIFF_LIBRARY)
    message(FATAL_ERROR "TIFF library not found")
endif()
if(NOT OPENJP2_LIBRARY)
    message(FATAL_ERROR "OpenJP2 library not found")
endif()
if(NOT ZLIB_LIBRARY)
    message(FATAL_ERROR "ZLIB library not found")
endif()
if(NOT THREADS_LIBRARY)
    message(FATAL_ERROR "Pthread library not found")
endif()

# Link libraries - use static OpenCV libraries with system dependencies
target_link_libraries(webcam_app 
    ${ONNXRUNTIME_DIR}/lib/libonnxruntime.so.1.16.3
    ${JPEG_LIBRARY}
    ${PNG_LIBRARY}
    ${TIFF_LIBRARY}
    ${OPENJP2_LIBRARY}
    ${ZLIB_LIBRARY}
    ${THREADS_LIBRARY}
    dl
    m
    rt
    ${OpenCV_LIBS}
)

# Set RPATH to include local OpenCV libraries
set_target_properties(webcam_app PROPERTIES
    INSTALL_RPATH "${OpenCV_LIBS_DIR}"
    BUILD_WITH_INSTALL_RPATH TRUE
)

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
