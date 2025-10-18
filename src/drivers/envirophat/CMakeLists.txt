
# https://www.adafruit.com/product/3194
list(APPEND DRIVER_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/envirophat.c
)

# Propagate the updated list to parent scope
set(DRIVER_SOURCES ${DRIVER_SOURCES} PARENT_SCOPE)
