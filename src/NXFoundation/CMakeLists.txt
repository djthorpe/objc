set(NAME "NXFoundation")

add_library(${NAME} STATIC
    NXApplication.m
    NXAutoreleasePool.m
    NXObject.m
    NXString.m
    NXString+format.c
    NXTimeInterval.m
    NXThread.m
    NXZone.m
    NXZone+malloc.m
    Object+Description.m
)
target_include_directories(${NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../include/runtime-${RUNTIME}
    ${CMAKE_CURRENT_LIST_DIR}/../../include
)
target_link_libraries(${NAME} PRIVATE
    objc-gcc
    sys
)
