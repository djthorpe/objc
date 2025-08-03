set(NAME "drivers")

# Initialize the list of driver sources
set(DRIVER_SOURCES "")

# Add subdirectories - each will append to DRIVER_SOURCES
add_subdirectory("bme280") # BME280 Temperature and Humidity Sensor
add_subdirectory("uc8151") # UC8151 E-Paper Display Driver
add_subdirectory("st7789") # ST7789 TFT LCD Display Driver

# Create the drivers library with all collected sources
add_library(${NAME} STATIC ${DRIVER_SOURCES})

# Add include directories for the drivers
target_include_directories(${NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../include
    ${CMAKE_CURRENT_LIST_DIR}/../../third_party
)

# add BME280_32BIT_ENABLE as a compile definition
target_compile_definitions(${NAME} PRIVATE
    BME280_32BIT_ENABLE
)
