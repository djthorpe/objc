
# add include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/runtime-${RUNTIME}
)

# Test cases - runtime
add_subdirectory(init_01)
add_subdirectory(init_02)
add_subdirectory(init_03)
add_subdirectory(init_04)
add_subdirectory(init_05)
add_subdirectory(init_06)
add_subdirectory(init_07)
add_subdirectory(runtime_08)
add_subdirectory(runtime_09)
add_subdirectory(runtime_10)

# Test cases - NXFoundation
add_subdirectory(NXFoundation_01)
