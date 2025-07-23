#include <objc/objc.h>
#include <tests/tests.h>

// informal_protocol

static int stop_called = 0;

@interface Object (StopProtocol)
- (void)stop;
@end

@implementation Object (StopProtocol)
- (void)stop {
  test_assert(stop_called == 0);
  stop_called = 1;
}
@end

int main(void) {
  Object *obj = [[Object alloc] init];
  test_assert(obj != nil);
  [obj stop];
  test_assert(stop_called == 1);
  [obj dealloc];
  return 0;
}
