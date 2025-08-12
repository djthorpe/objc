// Provide weak placeholders for stdout/stderr for bare-metal builds.
// The Pico SDK/picolibc provides strong definitions; these will be ignored.
#include <stddef.h>

__attribute__((weak)) void *stdout = NULL;
__attribute__((weak)) void *stderr = NULL;
