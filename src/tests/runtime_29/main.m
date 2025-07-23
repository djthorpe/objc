#include <objc/objc.h>
#include <runtime-sys/sys.h>
#include <tests/tests.h>

int test_runtime_29(void);

int main(void) { return TestMain("test_runtime_29", test_runtime_29); }

/* Test defining a protocol, and accessing it using @protocol */

@protocol Evaluating
- (int)importance;
@end

/* A class adopting the protocol */
@interface Test : Object <Evaluating>
@end

@implementation Test
- (int)importance {
  return 1000;
}
@end

int test_runtime_29(void) {
  Protocol *protocol = @protocol(Evaluating);
  test_cstrings_equal(proto_getName((objc_protocol_t *)protocol), "Evaluating");
  return 0;
}
