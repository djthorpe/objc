set(NAME "drivers")

# BME280 Temperature and Humidity Sensor
add_subdirectory("bme280")

# Add include directories for the drivers
target_include_directories(${NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../include
    ${CMAKE_CURRENT_LIST_DIR}/../../third_party
)
#target_link_libraries(${NAME} PRIVATE
#    runtime-sys
#    runtime-hw
#)
