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
  return class_getSuperclass(self);
}
@end

int main (void) {
  Class class = objc_lookupClass("RootClass");
  test_assert(class != Nil);
  test_assert([class class] == class);

  // Test superclass
  Class superclass = [class superclass];
  test_assert(superclass == Nil);

  return 0;
}
