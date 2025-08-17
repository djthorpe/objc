set(NAME "runtime_mqtt")
add_executable(${NAME}
    main.c
    hack.c
)
target_link_libraries(${NAME} PRIVATE
    runtime-hw
    runtime-net
)

# additional configurations for the Pico SDK
if(CMAKE_SYSTEM_NAME STREQUAL "PICO")
pico_add_extra_outputs(${NAME})
pico_enable_stdio_usb(${NAME} 1)
pico_enable_stdio_uart(${NAME} 0)
endif()

# Wi‑Fi credentials from environment
set(WIFI_SSID "$ENV{WIFI_SSID}")
set(WIFI_PASSWORD "$ENV{WIFI_PASSWORD}")
set(MQTT_SERVER "$ENV{MQTT_SERVER}")

# Add Wi‑Fi and MQTT credentials as compile definitions
target_compile_definitions(${NAME} PRIVATE
    WIFI_SSID="${WIFI_SSID}"
    WIFI_PASSWORD="${WIFI_PASSWORD}"
    MQTT_SERVER="${MQTT_SERVER}"
)
