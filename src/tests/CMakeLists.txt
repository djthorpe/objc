
# add include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/runtime-${RUNTIME}
)

# Test cases - runtime
add_subdirectory(runtime_01)
add_subdirectory(runtime_02)
add_subdirectory(runtime_03)
add_subdirectory(runtime_04)
add_subdirectory(runtime_05)
add_subdirectory(runtime_06)
add_subdirectory(runtime_07)
add_subdirectory(runtime_08)
add_subdirectory(runtime_09)
add_subdirectory(runtime_10)
add_subdirectory(runtime_11)
add_subdirectory(runtime_12)
add_subdirectory(runtime_13)
add_subdirectory(runtime_14)
add_subdirectory(runtime_15)
add_subdirectory(runtime_16)
add_subdirectory(runtime_17)
add_subdirectory(runtime_18)
add_subdirectory(runtime_19)
add_subdirectory(runtime_20)
add_subdirectory(runtime_21)
add_subdirectory(runtime_22)
add_subdirectory(runtime_23)

# Test cases - NXFoundation
add_subdirectory(NXFoundation_01)
add_subdirectory(NXFoundation_02)
