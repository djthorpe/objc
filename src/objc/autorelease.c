#include "runtime.h"

void *objc_autoreleasePoolPush(void) {
    // TODO: Append a POOL_MARKER to the autorelease pool.
    return 0;
}

void objc_autoreleasePoolPop(void *marker) {
    // TODO
    // Release objects until we see POOL_MARKER
}
