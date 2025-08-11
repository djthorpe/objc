#include <objc/objc.h>
#include <runtime-sys/sys.h>
#include <tests/tests.h>

int test_runtime_25(void);

int main(void) { return TestMain("test_runtime_25", test_runtime_25); }

/* Tests defining a protocol and a class adopting it */

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

int test_runtime_25(void) {
  Feature *object = [[Feature alloc] init];
  [object setEnabled:YES];
  test_assert([object isEnabled]);
  [object dealloc];
  return 0;
}
