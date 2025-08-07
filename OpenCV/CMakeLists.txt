cmake_minimum_required(VERSION 3.16)
project(YOLOv8Detection)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Enable code coverage for Debug builds
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage")
endif()

# Find OpenCV from lib directory
set(OpenCV_DIR ${CMAKE_SOURCE_DIR}/src/lib/lib/cmake/opencv4)
find_package(OpenCV REQUIRED)

# Find Eigen3
find_package(Eigen3 REQUIRED)

# Add executables
add_executable(yolov8n_detection 
    src/main.cpp
    src/yolov8n_detector.cpp
)

add_executable(yolov8n_video_detection 
    src/video_detection_main.cpp
    src/yolov8n_detector.cpp
    src/yolov8n_video_detector.cpp
)

add_executable(yolov8n_video_example 
    src/video_detection_example.cpp
    src/yolov8n_detector.cpp
    src/yolov8n_video_detector.cpp
)

add_executable(yolo_converter 
    src/yolo_converter_main.cpp
    src/yolo_converter.cpp
)

add_executable(yolo_converter_test 
    src/yolo_converter_test_main.cpp
    src/yolo_converter_test.cpp
)

# Include directories
target_include_directories(yolov8n_detection PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}
)

target_include_directories(yolov8n_video_detection PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}
)

target_include_directories(yolov8n_video_example PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}
)

target_include_directories(yolo_converter PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}
)

target_include_directories(yolo_converter_test PRIVATE
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

target_link_libraries(yolov8n_video_detection 
    ${CMAKE_SOURCE_DIR}/src/lib/lib/libopencv_world.a
    ${CMAKE_SOURCE_DIR}/src/lib/lib/opencv4/3rdparty/liblibprotobuf.a
    ${CMAKE_SOURCE_DIR}/src/lib/lib/opencv4/3rdparty/libade.a
    ${CMAKE_SOURCE_DIR}/src/lib/lib/opencv4/3rdparty/libittnotify.a
    ${CMAKE_SOURCE_DIR}/src/lib/lib/opencv4/3rdparty/liblibwebp.a
    jpeg
    png
    z
    avcodec
    avformat
    swscale
    avutil
)

target_link_libraries(yolov8n_video_example 
    ${CMAKE_SOURCE_DIR}/src/lib/lib/libopencv_world.a
    ${CMAKE_SOURCE_DIR}/src/lib/lib/opencv4/3rdparty/liblibprotobuf.a
    ${CMAKE_SOURCE_DIR}/src/lib/lib/opencv4/3rdparty/libade.a
    ${CMAKE_SOURCE_DIR}/src/lib/lib/opencv4/3rdparty/libittnotify.a
    ${CMAKE_SOURCE_DIR}/src/lib/lib/opencv4/3rdparty/liblibwebp.a
    jpeg
    png
    z
    avcodec
    avformat
    swscale
    avutil
)

target_link_libraries(yolo_converter 
    ${CMAKE_SOURCE_DIR}/src/lib/lib/libopencv_world.a
    ${CMAKE_SOURCE_DIR}/src/lib/lib/opencv4/3rdparty/liblibprotobuf.a
    ${CMAKE_SOURCE_DIR}/src/lib/lib/opencv4/3rdparty/libade.a
    ${CMAKE_SOURCE_DIR}/src/lib/lib/opencv4/3rdparty/libittnotify.a
    ${CMAKE_SOURCE_DIR}/src/lib/lib/opencv4/3rdparty/liblibwebp.a
    jpeg
    png
    z
)

target_link_libraries(yolo_converter_test 
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

set_target_properties(yolov8n_video_detection PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

set_target_properties(yolov8n_video_example PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

set_target_properties(yolo_converter PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

set_target_properties(yolo_converter_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Print OpenCV information
message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "    libraries: ${OpenCV_LIBS}")

# Compiler-specific options
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(yolov8n_detection PRIVATE -Wall -Wextra -Wpedantic -O2)
    target_compile_options(yolov8n_video_detection PRIVATE -Wall -Wextra -Wpedantic -O2)
    target_compile_options(yolov8n_video_example PRIVATE -Wall -Wextra -Wpedantic -O2)
    target_compile_options(yolo_converter PRIVATE -Wall -Wextra -Wpedantic -O2)
    target_compile_options(yolo_converter_test PRIVATE -Wall -Wextra -Wpedantic -O2)
endif()

if(MSVC)
    target_compile_options(yolov8n_detection PRIVATE /W4)
    target_compile_options(yolov8n_video_detection PRIVATE /W4)
    target_compile_options(yolov8n_video_example PRIVATE /W4)
    target_compile_options(yolo_converter PRIVATE /W4)
    target_compile_options(yolo_converter_test PRIVATE /W4)
endif()
