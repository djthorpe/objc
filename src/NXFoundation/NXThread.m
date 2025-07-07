#include <NXFoundation/NXFoundation.h>

@implementation NXThread

#pragma mark - Class methods

+ (void)sleepForTimeInterval:(NXTimeInterval)interval {
    if (interval < 0) {
        panicf("sleepForTimeInterval called with negative interval: %f", interval);
        return;
    }

    // Use the system sleep function
    objc_sleep(interval * Millisecond);
}

@end
