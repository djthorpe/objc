
# Set C standard for all tests to avoid C99/C11 mode issues
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS ON)

# Add compile options for C11 standard in Objective-C files
add_compile_options(
    "$<$<COMPILE_LANGUAGE:OBJC>:-std=gnu11>"
    "$<$<COMPILE_LANGUAGE:C>:-std=gnu11>"
)

# add include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/runtime-${RUNTIME}
)

# Test cases - system functions
add_subdirectory(sys_00)
add_subdirectory(sys_01)
add_subdirectory(sys_02)
add_subdirectory(sys_03)
add_subdirectory(sys_04)
add_subdirectory(sys_05)
add_subdirectory(sys_06)
add_subdirectory(sys_07)
add_subdirectory(sys_08)
add_subdirectory(sys_09)
add_subdirectory(sys_10)
add_subdirectory(sys_11)
add_subdirectory(sys_12)
add_subdirectory(sys_13)
add_subdirectory(sys_14)
add_subdirectory(sys_15)

# Test cases - runtime
add_subdirectory(runtime_01)
add_subdirectory(runtime_02)
add_subdirectory(runtime_03)
add_subdirectory(runtime_04)
add_subdirectory(runtime_05)
add_subdirectory(runtime_06)
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
add_subdirectory(runtime_24)
add_subdirectory(runtime_25)
add_subdirectory(runtime_26)
add_subdirectory(runtime_27)
add_subdirectory(runtime_28)
add_subdirectory(runtime_29)
add_subdirectory(runtime_30)
add_subdirectory(runtime_31)
add_subdirectory(runtime_32)
add_subdirectory(runtime_33)
add_subdirectory(runtime_34)
add_subdirectory(runtime_35)
add_subdirectory(runtime_36)
add_subdirectory(runtime_37)

# Test cases - NXFoundation
add_subdirectory(NXFoundation_01)
add_subdirectory(NXFoundation_02)
add_subdirectory(NXFoundation_03)
add_subdirectory(NXFoundation_04)
add_subdirectory(NXFoundation_05)
add_subdirectory(NXFoundation_06)
add_subdirectory(NXFoundation_07)
add_subdirectory(NXFoundation_08)
add_subdirectory(NXFoundation_09)
add_subdirectory(NXFoundation_10)
add_subdirectory(NXFoundation_11)
add_subdirectory(NXFoundation_12)
add_subdirectory(NXFoundation_13)
add_subdirectory(NXFoundation_14)
add_subdirectory(NXFoundation_15)
add_subdirectory(NXFoundation_16)
add_subdirectory(NXFoundation_17)
add_subdirectory(NXFoundation_18)
add_subdirectory(NXFoundation_19)
add_subdirectory(NXFoundation_20)
add_subdirectory(NXFoundation_21)
add_subdirectory(NXFoundation_22)
add_subdirectory(NXFoundation_23)
add_subdirectory(NXFoundation_24)

