
# add include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../include)

# add test cases
include(CTest)
add_subdirectory(init_01)
add_test(NAME init_01 COMMAND init_01/init_01)
