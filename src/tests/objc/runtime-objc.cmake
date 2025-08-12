
# Test cases - objc runtime
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/runtime_01)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/runtime_02)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/runtime_03)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/runtime_04)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/runtime_05)

# On the Pico, we combine the tests into a single executable which 
# so all tests can be run together.
if(CMAKE_SYSTEM_NAME STREQUAL "PICO")
    add_executable(tests-runtime-objc
        ${CMAKE_CURRENT_LIST_DIR}/runtime-objc.m
    )
    target_link_libraries(tests-runtime-objc
        malloc # we use system-provided malloc for these tests
        runtime_01
        runtime_02
        runtime_03
        runtime_04
        runtime_05
    )
    pico_add_extra_outputs(tests-runtime-objc)
    pico_enable_stdio_usb(tests-runtime-objc 1)
    pico_enable_stdio_uart(tests-runtime-objc 0)
endif()
