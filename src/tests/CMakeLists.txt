
# add include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/runtime-${RUNTIME}
)

# Runtime System Tests
include(sys/runtime-sys.cmake)
include(objc/runtime-objc.cmake)
include(fs/runtime-fs.cmake)

# TODO: Commented out until we can re-introduce these tests
#include(runtime-hw.cmake)
#include(NXFoundation.cmake)
#include(NXApplication.cmake)
#include(runtime-pix.cmake)
