#include <objc/objc.h>
#include <tests/tests.h>

/* Tests RootClass */

OBJC_ROOT_CLASS
@interface RootClass
{
  Class isa;
}

+ (Class) class;
+ (Class) superclass;

@end

@implementation RootClass
+ (Class) class
{
  return self;
}

+ (Class) superclass 
{
  return Nil; // TODO: No superclass for RootClass
}
@end

int main (void) {
  Class class = objc_lookupClass("RootClass");;
  test_assert(class != Nil);
  test_assert([class class] == class);

  // TODO: Test superclass

  return 0;
}
