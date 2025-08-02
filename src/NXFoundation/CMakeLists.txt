set(NAME "NXFoundation")

add_library(${NAME} STATIC
    NXArch.m
    NXArray.m
    NXAutoreleasePool.m
    NXData.m
    NXDate.m
    NXLog.m
    NXNotFound.m
    NXNull.m
    NXNumber.m
    NXNumberBool.m
    NXNumberInt16.m
    NXNumberInt32.m
    NXNumberInt64.m
    NXNumberUnsignedInt16.m
    NXNumberUnsignedInt32.m
    NXNumberUnsignedInt64.m
    NXNumberZero.m
    NXMap.m
    NXMap+hash.c
    NXObject.m
    NXString.m
    NXTimeInterval.m
    NXThread.m
    NXZone.m
    NXZone+arena.c
    NXZone+malloc.m
)
target_include_directories(${NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../include/runtime-${RUNTIME}
    ${CMAKE_CURRENT_LIST_DIR}/../../include
)
target_link_libraries(${NAME} PRIVATE
    objc-gcc
    runtime-sys
)

