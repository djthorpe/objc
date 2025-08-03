set(NAME "drivers")

# BME280 Temperature and Humidity Sensor
add_subdirectory("bme280")

# Add include directories for the drivers
target_include_directories(${NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../include
    ${CMAKE_CURRENT_LIST_DIR}/../../third_party
)
# add BME280_32BIT_ENABLE as a compile definition
target_compile_definitions(${NAME} PRIVATE
    BME280_32BIT_ENABLE
)
