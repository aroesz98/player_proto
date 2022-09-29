add_library(TFTLIB_8BIT INTERFACE)

target_sources(TFTLIB_8BIT INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/TFTLIB_8BIT.cpp
    ${CMAKE_CURRENT_LIST_DIR}/WString.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Print.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dtostrf.c
)

target_include_directories(TFTLIB_8BIT INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/
    ${CMAKE_CURRENT_LIST_DIR}/lcd_drivers
    ${CMAKE_CURRENT_LIST_DIR}/GFXFF
    ${CMAKE_CURRENT_LIST_DIR}/GFXFF/Custom
)

target_link_libraries(TFTLIB_8BIT INTERFACE
        hardware_gpio
        hardware_timer
        pico_stdlib
        pico_float
)