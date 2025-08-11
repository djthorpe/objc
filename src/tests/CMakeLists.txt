
# add include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/runtime-${RUNTIME}
)

include(sys/runtime-sys.cmake)
#include(runtime-hw.cmake)
#TODO: Commented out until pix is implemented
#include(runtime-pix.cmake)
#include(runtime-objc.cmake)
#include(NXFoundation.cmake)
#include(NXApplication.cmake)
