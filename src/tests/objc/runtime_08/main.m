#include <objc/objc.h>
#include <runtime-sys/sys.h>
#include <tests/tests.h>

#ifdef SYSTEM_NAME_PICO
// HACK
void *stdout = NULL;
void *stderr = NULL;
#endif

@interface Foo : Object
+ (id)foo;
+ (id)bar;
@end

int foocalled = 0;
int barcalled = 0;

@implementation Foo
+ (id)foo {
  test_assert(!foocalled);
  foocalled = 1;
  return self;
}

+ (id)bar {
  test_assert(!barcalled);
  barcalled = 1;
  return self;
}
@end

int test_runtime_08(void);

int main(void) { return TestMain("test_runtime_08", test_runtime_08); }

int test_runtime_08() {
  [[Foo foo] bar];
  return 0;
}
