#include <stdio.h>
#include <objc/objc.h>
#include <tests/tests.h>

/* Tests creating a root class and a minimal subclass tree */

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

@interface SubClassA : RootClass
@end

@implementation SubClassA
@end

@interface SubClassB : RootClass
@end

@implementation SubClassB
@end

@interface SubSubClass : SubClassA
@end

@implementation SubSubClass
@end

int main (void) {
  printf("Testing class hierarchy...\n");
  test_class_has_superclass("RootClass", "");
  test_class_has_superclass("SubClassA", "RootClass");
  test_class_has_superclass("SubClassB", "RootClass");
  test_class_has_superclass("SubSubClass", "SubClassA");
  return 0;
}
