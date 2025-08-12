#include <objc/objc.h>
#include <runtime-sys/sys.h>
#include <tests/tests.h>

int test_runtime_33(void);

int main(void) { return TestMain("test_runtime_33", test_runtime_33); }

@protocol Foo1
- (void)foo1;
@end

@protocol Foo2
- (void)foo2;
@end

int test_runtime_33(void) {
  test_assert(@protocol(Foo1) != @protocol(Foo2));
  test_assert([@protocol(Foo1) isEqual:@protocol(Foo1)] == YES);
  test_assert([@protocol(Foo1) isEqual:@protocol(Foo2)] == NO);
  test_assert([@protocol(Foo2) isEqual:@protocol(Foo2)] == YES);
  test_assert([@protocol(Foo1) isEqual:nil] == NO);
  test_assert([@protocol(Foo2) isEqual:nil] == NO);

  Class protocol = objc_lookupClass("Protocol");
  Class object = objc_lookupClass("Object");
  test_assert(protocol != nil);
  test_assert(object != nil);
  test_assert(class_getSuperclass(protocol) == object);
  test_cstrings_equal(class_getName(protocol), "Protocol");

  return 0;
}
