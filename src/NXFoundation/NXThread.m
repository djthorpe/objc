#include <NXFoundation/NXFoundation.h>

@implementation NXThread

#pragma mark - Class methods

+ (void)sleepForTimeInterval:(NXTimeInterval)interval {
    if (interval < 0) {
        panicf("sleepForTimeInterval called with negative interval: %f", interval);
        return;
    }
    
    // Convert seconds to milliseconds
    unsigned int milliseconds = (unsigned int)(interval * 1000.0);
    
    // Use the system sleep function
    objc_sleep(milliseconds);
}

@end
