#include <objc/objc.h>
#include <tests/tests.h>

/* fdecl */

@class AClass;

OBJC_ROOT_CLASS
@interface test {
  AClass* foo;
}
@end

@implementation test
@end

int main (void)
{
  return 0;
}

