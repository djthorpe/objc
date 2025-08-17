set(NAME "runtime_wificonnect")
add_executable(${NAME}
    main.c
    hack.c
)
target_link_libraries(${NAME} PRIVATE
    runtime-hw
)

# additional configurations for the Pico SDK
if(CMAKE_SYSTEM_NAME STREQUAL "PICO")
pico_add_extra_outputs(${NAME})
pico_enable_stdio_usb(${NAME} 1)
pico_enable_stdio_uart(${NAME} 0)
endif()

# Wi‑Fi credentials from environment (export WIFI_SSID and WIFI_PASSWORD)
set(WIFI_SSID "$ENV{WIFI_SSID}")
set(WIFI_PASSWORD "$ENV{WIFI_PASSWORD}")

# Add Wi‑Fi credentials as compile definitions
target_compile_definitions(${NAME} PRIVATE
    WIFI_SSID="${WIFI_SSID}"
    WIFI_PASSWORD="${WIFI_PASSWORD}"
)
