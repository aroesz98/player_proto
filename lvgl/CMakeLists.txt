cmake_minimum_required(VERSION 3.12.4)

add_library(lvgl INTERFACE)

file(GLOB_RECURSE SRC_FILES
    ${CMAKE_CURRENT_LIST_DIR}/*.c
)

target_sources(lvgl INTERFACE
    ${SRC_FILES}
)

target_include_directories(lvgl INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/
    ${CMAKE_CURRENT_LIST_DIR}/src
    ${CMAKE_CURRENT_LIST_DIR}/demos
    ${CMAKE_CURRENT_LIST_DIR}/examples
    ${CMAKE_CURRENT_LIST_DIR}/env_support
)

target_link_libraries(lvgl INTERFACE
        pico_stdlib
)

project(lvgl HOMEPAGE_URL https://github.com/lvgl/lvgl)

set(LVGL_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})
