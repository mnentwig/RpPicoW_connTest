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
# settings agreed between both nodes
# =================================================
set(WIFI_SSID "myTestLink")
set(WIFI_PASSWORD "buZZword")
set(WIFI_PORT "8080") # arbitrarily using "typical" HTTP port

# =================================================
# node 1 - WLAN access point
# =================================================
set(T "node1_ap")
add_executable(${T}
  node1_ap.c
  dhcpserver/dhcpserver.c
  )
pico_enable_stdio_usb(${T} 0) # not using UART
pico_enable_stdio_uart(${T} 0) # nor USB serial

target_include_directories(${T} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/dhcpserver
  )

target_compile_definitions(${T} PRIVATE
  WIFI_SSID=\"${WIFI_SSID}\"
  WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
  WIFI_PORT=${WIFI_PORT}
  )

target_link_libraries(${T}
  pico_cyw43_arch_lwip_poll
  pico_stdlib
  )

pico_add_extra_outputs(${T})

# =================================================
# node 2 - WLAN "station" (regular WLAN device)
# =================================================
set(T "node2")
add_executable(${T}
  node2.c
  )
pico_enable_stdio_usb(${T} 0) # not using UART
pico_enable_stdio_uart(${T} 0) # nor USB serial
target_include_directories(${T} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  )

target_compile_definitions(${T} PRIVATE
  WIFI_SSID=\"${WIFI_SSID}\"
  WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
  WIFI_PORT=${WIFI_PORT}
  )

target_link_libraries(${T}
  pico_cyw43_arch_lwip_threadsafe_background
  pico_stdlib
  )
pico_add_extra_outputs(${T})

