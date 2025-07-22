# CMake configuration for NXFoundation_14 test
# Test NXDate edge cases and fixes

cmake_minimum_required(VERSION 3.15)

# Add executable
add_executable(NXFoundation_14 main.m)

# Link with required libraries
target_link_libraries(NXFoundation_14 PRIVATE NXFoundation objc-gcc sys)

# Set target properties
set_target_properties(NXFoundation_14 PROPERTIES
    OBJC_STANDARD 11
    OBJC_STANDARD_REQUIRED YES
)

# Include directories
target_include_directories(NXFoundation_14 PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)
