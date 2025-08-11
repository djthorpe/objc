
# Test cases - runtime-sys
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/sys_00)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/sys_01)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/sys_02)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/sys_03)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/sys_04)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/sys_05)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/sys_06)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/sys_07)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/sys_08)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/sys_09)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/sys_10)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/sys_11)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/sys_12)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/sys_13)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/sys_14)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/sys_15)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/sys_16)

# On the Pico, we combine the tests into a single executable which 
# so all tests can be run together.
if(CMAKE_SYSTEM_NAME STREQUAL "PICO")
    add_executable(tests-runtime-sys
        ${CMAKE_CURRENT_LIST_DIR}/runtime-sys.c
    )
    target_link_libraries(tests-runtime-sys
        sys_00
        sys_01
        sys_02
        sys_03
        sys_04
        sys_05
        sys_06
        sys_07
        sys_08
        sys_09
        sys_10
        sys_11
        sys_12
        sys_13
        sys_14
        sys_15
        sys_16
    )
    pico_add_extra_outputs(tests-runtime-sys)
    pico_enable_stdio_usb(tests-runtime-sys 1)
    pico_enable_stdio_uart(tests-runtime-sys 0)
endif()
