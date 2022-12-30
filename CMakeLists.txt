cmake_minimum_required(VERSION 3.12)

# +++hack+++ (should be given via -DPICO_SDK_PATH to cmake)
set(ENV{PICO_SDK_PATH} /home/pi/pico/pico-sdk)
set(PICO_BOARD pico_w)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(myPicoW C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

pico_sdk_init()

# =================================================
# node 1 - WLAN access point
# =================================================
add_executable(node1_ap
        node1_ap.c
        dhcpserver/dhcpserver.c
        )
pico_enable_stdio_usb(node1_ap 0) # not using UART
pico_enable_stdio_uart(node1_ap 0) # nor USB serial

target_include_directories(node1_ap PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/dhcpserver
        )

target_link_libraries(node1_ap
        pico_cyw43_arch_lwip_poll
        pico_stdlib
        )

pico_add_extra_outputs(node1_ap)

# =================================================
# node 2 - WLAN "station" (regular WLAN device)
# =================================================
add_executable(node2
  node2.c
  )
pico_enable_stdio_usb(node2 0) # not using UART
pico_enable_stdio_uart(node2 0) # nor USB serial
target_include_directories(node2 PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  )
target_link_libraries(node2
  pico_cyw43_arch_lwip_threadsafe_background
  pico_stdlib
  )
pico_add_extra_outputs(node2)

