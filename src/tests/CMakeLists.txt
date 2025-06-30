
# add include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/runtime-${RUNTIME}
)

# add test cases - runtime
add_subdirectory(init_01)
add_subdirectory(init_02)
add_subdirectory(init_03)
add_subdirectory(init_04)
add_subdirectory(init_05)
add_subdirectory(init_06)

# add test cases - NXFoundation
add_subdirectory(NXFoundation_01)
