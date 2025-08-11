#include <objc/objc.h>
#include <tests/tests.h>

/* function-message-1 */

@interface Foo : Object
+(id) bar;
@end

int foocalled = 0;
int barcalled = 0;

id foo() {
  test_assert(foocalled == 0);
  foocalled = 1;
  return [Foo class];
}

@implementation Foo

+(id) bar {
  test_assert(barcalled == 0);
  barcalled = 1;
  return self;
}

@end

int main(void) {
    Class f = [foo() bar];
    test_assert(f != Nil);
    test_assert(f == [Foo class]);
    return 0;
}
