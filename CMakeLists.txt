
# Define project
project(objc-gcc C ASM CXX)

# objc runtime and tests
add_subdirectory(src/runtime-gcc)

include(CTest)
add_subdirectory(src/tests)
