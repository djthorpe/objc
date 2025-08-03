
# ST7789 TFT LCD display driver interface
# Append source files to the parent DRIVER_SOURCES list
list(APPEND DRIVER_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/st7789.c
)

# Propagate the updated list to parent scope
set(DRIVER_SOURCES ${DRIVER_SOURCES} PARENT_SCOPE)
