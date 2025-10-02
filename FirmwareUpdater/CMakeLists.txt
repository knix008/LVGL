cmake_minimum_required(VERSION 3.10)
project(DeviceSimulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(OpenSSL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)

# Include directories
include_directories(${OPENSSL_INCLUDE_DIR})
include_directories(${JSONCPP_INCLUDE_DIRS})

# Device Simulator executable
add_executable(device_simulator device_simulator.cpp)

target_link_libraries(device_simulator
    ${OPENSSL_LIBRARIES}
    ${JSONCPP_LIBRARIES}
)

target_compile_options(device_simulator PRIVATE -Wall -Wextra)

# Firmware Uploader executable
add_executable(firmware_uploader firmware_uploader.cpp)

target_link_libraries(firmware_uploader
    ${OPENSSL_LIBRARIES}
    ${JSONCPP_LIBRARIES}
)

target_compile_options(firmware_uploader PRIVATE -Wall -Wextra)

# Install targets
install(TARGETS device_simulator firmware_uploader DESTINATION bin)
