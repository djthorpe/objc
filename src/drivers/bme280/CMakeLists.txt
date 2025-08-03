
add_library(${NAME} STATIC
    ${CMAKE_CURRENT_LIST_DIR}/../../../third_party/BME280_SensorAPI/bme280.c
    bme280.c
)
