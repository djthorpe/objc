#include <objc/objc.h>
#include <stdio.h>
#include <tests/tests.h>

@interface Foo : Object
+ (int)replaced;
@end
@implementation Foo
+ (int)replaced {
  printf("INVALID Replaced method called\n");
  return 1;
}
@end

@implementation Foo (bar)
+ (int)replaced {
  printf("CORRECT Replaced method called\n");
  return 2;
}
@end

int main(void) {
  test_assert([Foo replaced] == 2);
  return 0;
}
