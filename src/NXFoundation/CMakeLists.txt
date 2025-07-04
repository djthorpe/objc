set(NAME "NXFoundation")

add_library(${NAME} STATIC
    NXObject.m
    NXString.m
    NXZone.m
    NXZone+malloc.m
)
target_include_directories(${NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../include/runtime-${RUNTIME}
    ${CMAKE_CURRENT_LIST_DIR}/../../include
)
target_link_libraries(${NAME} PRIVATE
    objc-gcc
)
