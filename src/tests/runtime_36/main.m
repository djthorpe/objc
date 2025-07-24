#include <objc/objc.h>
#include <runtime-sys/sys.h>
#include <stdarg.h>
#include <tests/tests.h>

int test_runtime_36(void);

int main(void) { return TestMain("test_runtime_36", test_runtime_36); }

/* Test defining a static function *inside* a class implementation */

@interface Test : Object
+ (int)test;
@end

@implementation Test

static int test(void) { return 1; }

+ (int)test {
  return test();
}

+ initialize {
  return self;
}
@end

int test_runtime_36(void) {
  test_assert([Test test] == 1);
  return 0;
}
