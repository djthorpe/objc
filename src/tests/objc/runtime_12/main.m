#include <objc/objc.h>
#include <tests/tests.h>

/* Tests creating a root class and a subclass */

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

@interface SubClass : RootClass
@end

@implementation SubClass
@end

int main (void) {
  Class class = objc_lookupClass("RootClass");
  test_assert(class != Nil);
  test_assert([class class] == class);

  // Test superclass
  Class superclass = [class superclass];
  test_assert(superclass == Nil);

  Class subclass = objc_lookupClass("SubClass");
  test_assert(subclass != Nil);
  test_assert([subclass class] == subclass);

  // Test subclass
  Class superclass_of_subclass = [subclass superclass];
  test_assert(superclass_of_subclass == class);

  return 0;
}
