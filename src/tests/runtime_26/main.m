#include <objc/objc.h>
#include <runtime-sys/sys.h>
#include <tests/tests.h>

int test_runtime_26(void);

int main(void) { return TestMain("test_runtime_26", test_runtime_26); }

/* Test defining a protocol, a class adopting it, and using an object
   of type `id <protocol>'. */

@protocol Enabling
- (BOOL)isEnabled;
- (void)setEnabled:(BOOL)flag;
@end

@interface Feature : Object <Enabling> {
  const char *name;
  BOOL isEnabled;
}
@end

@implementation Feature
- (BOOL)isEnabled {
  return isEnabled;
}
- (void)setEnabled:(BOOL)flag {
  isEnabled = flag;
}
@end

int test_runtime_26(void) {
  id<Enabling> object = [[Feature alloc] init];
  test_assert(object != nil);
  [object setEnabled:YES];
  test_assert([object isEnabled]);
  [(Object *)object dealloc];
  return 0;
}
