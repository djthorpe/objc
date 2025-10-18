
# add include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/runtime-${RUNTIME}
)

# Runtime System Tests
include(sys/runtime-sys.cmake)
include(objc/runtime-objc.cmake)
include(fs/runtime-fs.cmake)
include(hw/runtime-hw.cmake)
include(drivers/drivers.cmake)

# TODO: Commented out until we can re-introduce these tests
#include(NXFoundation.cmake)
#include(NXApplication.cmake)
#include(runtime-pix.cmake)
