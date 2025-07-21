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
# TODO: At the moment the Object+Description.m is compiled out so we use -O0
target_compile_options(${NAME} PRIVATE
    $<$<CONFIG:Release>:-O0>  # Override -O3 with -O0 for Release builds
)
