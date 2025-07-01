#include <objc/objc.h>
#include <tests/tests.h>

/* Test calling a class method on self where self has been redefined
   to be another class - the call requires a cast */


/* The first class */
struct d
{
  int a;
};

OBJC_ROOT_CLASS
@interface ClassA
{
  Class isa;
}
+ (Class) class;
+ (struct d) method;
@end

@implementation ClassA
+ (Class) class
{
  return self;
}

+ (struct d) method
{
  struct d u;
  u.a = 5;
  
  return u;
}
+ initialize { return self; }
@end

/* The second class */
OBJC_ROOT_CLASS
@interface TestClass
{
  Class isa;
}
+ (void) test;
@end

@implementation TestClass
+ (void) test
{
  self = [ClassA class];  
  test_assert([(Class)self method].a == 5);
}

+ initialize { return self; }
@end


int main (void)
{
  [TestClass test];

  return 0;
}
