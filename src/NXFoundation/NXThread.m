#include <NXFoundation/NXFoundation.h>
#include <sys/sys.h>

@implementation NXThread

#pragma mark - Class methods

+ (void)sleepForTimeInterval:(NXTimeInterval)interval {
    if (interval < 0) {
        panicf("sleepForTimeInterval called with negative interval: %f", interval);
        return;
    }

    // Use the system sleep function
    sys_sleep(interval * Millisecond);
}

@end
