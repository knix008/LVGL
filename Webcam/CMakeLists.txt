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

# GTK3 for highgui (required when linking static OpenCV built with GTK)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# Make GTK3 headers/flags available
include_directories(${GTK3_INCLUDE_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})

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

add_executable(face_detection 
    src/face_detection_main.cpp
    src/yolo_face_detector.cpp
    src/yolo_face_video_detector.cpp
    src/simple_model_converter.cpp
)

add_executable(model_converter 
    src/model_converter_main.cpp
    src/simple_model_converter.cpp
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
    src/yolov8n_detector.cpp
)

add_executable(webcam_viewer 
    src/webcam_viewer_main.cpp
    src/webcam_viewer.cpp
)

add_executable(webcam_viewer_simple 
    src/webcam_viewer_simple_main.cpp
    src/webcam_viewer_simple.cpp
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

target_include_directories(face_detection PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}
)

target_include_directories(model_converter PRIVATE
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

target_include_directories(webcam_viewer PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}
)

target_include_directories(webcam_viewer_simple PRIVATE
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
    ${GTK3_LIBRARIES}
)

target_link_libraries(face_detection 
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
    ${GTK3_LIBRARIES}
)

target_link_libraries(model_converter 
    # No external libraries needed for simple converter
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
    ${GTK3_LIBRARIES}
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

target_link_libraries(webcam_viewer 
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
    ${GTK3_LIBRARIES}
)

target_link_libraries(webcam_viewer_simple 
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
    ${GTK3_LIBRARIES}
)

# Set output directory
set_target_properties(yolov8n_detection PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

set_target_properties(yolov8n_video_detection PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

set_target_properties(face_detection PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

set_target_properties(model_converter PROPERTIES
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

set_target_properties(webcam_viewer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

set_target_properties(webcam_viewer_simple PROPERTIES
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
    target_compile_options(face_detection PRIVATE -Wall -Wextra -Wpedantic -O2)
    target_compile_options(model_converter PRIVATE -Wall -Wextra -Wpedantic -O2)
    target_compile_options(yolov8n_video_example PRIVATE -Wall -Wextra -Wpedantic -O2)
    target_compile_options(yolo_converter PRIVATE -Wall -Wextra -Wpedantic -O2)
    target_compile_options(yolo_converter_test PRIVATE -Wall -Wextra -Wpedantic -O2)
    target_compile_options(webcam_viewer PRIVATE -Wall -Wextra -Wpedantic -O2)
    target_compile_options(webcam_viewer_simple PRIVATE -Wall -Wextra -Wpedantic -O2)
endif()

if(MSVC)
    target_compile_options(yolov8n_detection PRIVATE /W4)
    target_compile_options(yolov8n_video_detection PRIVATE /W4)
    target_compile_options(face_detection PRIVATE /W4)
    target_compile_options(model_converter PRIVATE /W4)
    target_compile_options(yolov8n_video_example PRIVATE /W4)
    target_compile_options(yolo_converter PRIVATE /W4)
    target_compile_options(yolo_converter_test PRIVATE /W4)
    target_compile_options(webcam_viewer PRIVATE /W4)
    target_compile_options(webcam_viewer_simple PRIVATE /W4)
endif()

# Simple Model Converter Configuration
# No external dependencies required
# ------------------------------------------------------------
# LVGL Graphics Library 추가
add_subdirectory(lvgl)

# LVGL 헤더 경로를 모든 타겟에 추가하려면 아래 예시처럼 사용
include_directories(${CMAKE_SOURCE_DIR}/lvgl)

# LVGL 라이브러리를 링크하려면 원하는 타겟에 아래 예시처럼 추가
# target_link_libraries(yolov8n_detection lvgl)
# ------------------------------------------------------------
