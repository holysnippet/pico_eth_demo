cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)
project(pico_eth_demo VERSION 0.1.0)

include(CTest)
enable_testing()

###
#set(PICO_DEOPTIMIZED_DEBUG 1)
###
pico_sdk_init()

add_subdirectory(pico_eth)

add_executable(pico_eth_demo main.c)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

pico_add_extra_outputs(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
    main.c
    ntptest.c
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    pico_lwip
    pico_lwip_iperf
    pico_lwip_http
    pico_stdlib
    pico_time
    pico_eth
)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)