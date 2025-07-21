set(NAME "NXFoundation")

add_library(${NAME} STATIC
    NXApplication.m
    NXArch.m
    NXAutoreleasePool.m
    NXNumber.m
    NXNumberInt.m
    NXNumberUnsignedInt.m
    NXObject.m
    NXString.m
    NXTimeInterval.m
    NXThread.m
    NXZone.m
    NXZone+arena.c
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
# Ensure Object+Description.m symbols are included without disabling optimizations
target_link_options(${NAME} PRIVATE
    $<$<CONFIG:Release>:-Wl,--whole-archive>
)
