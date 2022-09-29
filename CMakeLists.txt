# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

# Include build functions from Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Set name of project (as PROJECT_NAME) and C/C++ standards
project(player C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Create pico-sdk subdirectory in our project
pico_sdk_init()

# Add FreeRTOS as a library
add_library(FreeRTOS STATIC
    ${PROJECT_SOURCE_DIR}/FreeRTOS/event_groups.c
    ${PROJECT_SOURCE_DIR}/FreeRTOS/list.c
    ${PROJECT_SOURCE_DIR}/FreeRTOS/queue.c
    ${PROJECT_SOURCE_DIR}/FreeRTOS/stream_buffer.c
    ${PROJECT_SOURCE_DIR}/FreeRTOS/tasks.c
    ${PROJECT_SOURCE_DIR}/FreeRTOS/timers.c
    ${PROJECT_SOURCE_DIR}/FreeRTOS/portable/MemMang/heap_3.c
    ${PROJECT_SOURCE_DIR}/FreeRTOS/portable/GCC/ARM_CM0/port.c
)

include_directories(
    ${PROJECT_SOURCE_DIR}/FreeRTOS/
    ${PROJECT_SOURCE_DIR}/FreeRTOS/include
    ${PROJECT_SOURCE_DIR}/FreeRTOS/portable/GCC/ARM_CM0

    ${PROJECT_SOURCE_DIR}/XPT2046
)

file(GLOB_RECURSE SRC_FILES
    ${PROJECT_SOURCE_DIR}/simple_ui/*.c
)

add_executable(${PROJECT_NAME}
    ${SRC_FILES}
    main.cpp
    FatFS/ff.c
    FatFS/sdmm.cpp
    FatFS/ffunicode.c
    FatFS/ffsystem.c
    XPT2046/XPT2046.cpp
)

pico_generate_pio_header(${PROJECT_NAME}  
    ${CMAKE_CURRENT_LIST_DIR}/pio_spi/spi.pio
)

target_sources(${PROJECT_NAME} PRIVATE
    pio_spi/pio_spi.c
    pio_spi/pio_spi.h
)

# Add FatFS Library
add_subdirectory(TFTLIB_8BIT)
add_subdirectory(lvgl)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${PROJECT_NAME})

# Link to pico_stdlib (gpio, time etc.)
target_link_libraries(${PROJECT_NAME}
    hardware_gpio
    hardware_clocks
    hardware_vreg
    hardware_spi
    hardware_pio
    hardware_pwm
    
    pico_stdlib
    pico_float
    pico_multicore
    FreeRTOS
    TFTLIB_8BIT
    lvgl
)

pico_define_boot_stage2(slower_boot2 ${PICO_DEFAULT_BOOT_STAGE2_FILE})
target_compile_definitions(slower_boot2 PRIVATE PICO_FLASH_SPI_CLKDIV=4)
pico_set_boot_stage2(${PROJECT_NAME} slower_boot2)