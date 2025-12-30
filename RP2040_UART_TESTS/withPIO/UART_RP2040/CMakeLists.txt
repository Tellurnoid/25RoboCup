cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(pio_uart_example C CXX ASM)

pico_sdk_init()

add_executable(pio_uart_example
    main.c
    uart_tx.pio
    uart_rx.pio
)

# ★ ここが .pio.h を作る命令 ★
pico_generate_pio_header(pio_uart_example ${CMAKE_CURRENT_LIST_DIR}/uart_tx.pio)
pico_generate_pio_header(pio_uart_example ${CMAKE_CURRENT_LIST_DIR}/uart_rx.pio)

target_link_libraries(pio_uart_example
    pico_stdlib
    hardware_uart
    hardware_pio
)

pico_add_extra_outputs(pio_uart_example)
