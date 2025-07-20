#include <NXFoundation/NXFoundation.h>
#include <sys/sys.h>

@implementation NXThread

///////////////////////////////////////////////////////////////////////////////
// CLASS METHODS

+ (void)sleepForTimeInterval:(NXTimeInterval)interval {
  if (interval < 0) {
    panicf("sleepForTimeInterval called with negative interval: %f", interval);
    return;
  }
  if (interval == 0) {
    // If the interval is zero, just return immediately
    return;
  }
  // Use the system sleep function
  sys_sleep(interval * Millisecond);
}

@end
