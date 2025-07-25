#include <objc/objc.h>
#include <tests/tests.h>

/* Test calling a class method when there is an instance method 
   with conflicting types */

/* This class should be unused but on broken compilers its instance
   method might get picked up and used instead of the class method of
   another class ! */
struct d
{
  int a;
};

OBJC_ROOT_CLASS
@interface UnusedClass 
{
  Class isa;
}
- (struct d) method;
@end

@implementation UnusedClass
- (struct d) method
{
  struct d u;
  u.a = 0;
  
  return u;
}
@end

/* The real class */
OBJC_ROOT_CLASS
@interface TestClass
{
  Class isa;
}
+ (void) test;
+ (int) method;
@end

@implementation TestClass
+ (void) test
{
  test_assert([self method] == 4);
}

+ (int) method
{
  return 4;
}
+ (id) initialize { return self; }
@end


int main (void)
{
  [TestClass test];

  return 0;
}
