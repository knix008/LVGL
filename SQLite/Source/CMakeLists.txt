cmake_minimum_required(VERSION 3.10)
project(lvgl_main C)

set(CMAKE_C_STANDARD 99)

set(LVGL_DIR "lvgl")
set(LV_DRIVERS_DIR "lvgl/src/drivers")
set(SQLITE_DIR "sqlite")

include_directories(
    ${LVGL_DIR}
    ${LVGL_DIR}/src
    ${LV_DRIVERS_DIR}
    ${LV_DRIVERS_DIR}/sdl
    ${SQLITE_DIR}
)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_compile_definitions($<$<BOOL:${LV_USE_DRAW_SDL}>:LV_USE_DRAW_SDL=1>)

# SQLite compile definitions
add_compile_definitions(
    SQLITE_ENABLE_FTS5=1
    SQLITE_ENABLE_JSON1=1
    SQLITE_ENABLE_RTREE=1
    SQLITE_ENABLE_UNLOCK_NOTIFY=1
    SQLITE_ENABLE_DBSTAT_VTAB=1
    SQLITE_ENABLE_LOAD_EXTENSION=1
    SQLITE_THREADSAFE=1
)

add_executable(lvgl_main main.c)

file(GLOB_RECURSE LVGL_SOURCES
    ${LVGL_DIR}/src/*.c
)
file(GLOB LV_DRIVERS_SOURCES
    ${LV_DRIVERS_DIR}/sdl/*.c
)

# SQLite amalgamation files
set(SQLITE_SOURCES
    ${SQLITE_DIR}/sqlite3.c
)

target_sources(lvgl_main PRIVATE ${LVGL_SOURCES} ${LV_DRIVERS_SOURCES} ${SQLITE_SOURCES})
target_link_libraries(lvgl_main ${SDL2_LIBRARIES})

if(UNIX)
    target_link_libraries(lvgl_main pthread dl m)
endif()