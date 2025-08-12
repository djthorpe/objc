#include <stdio.h>
#include <objc/objc.h>
#include <tests/tests.h>

/* initialise-1 if a class has no +initialize method, the superclass implementation is called.  */

static int class_variable = 0;

OBJC_ROOT_CLASS
@interface Test {
  Class isa;
}
+(void) initialize;
+(int) classVariable;
@end

@implementation Test
+(void) initialize {
  printf("\n\nTest class initialized\n\n");
  class_variable++;
}

+(int) classVariable {
  return class_variable;
}

@end

@interface TestSubClass : Test
@end

@implementation TestSubClass
@end

int main (void) {
  test_assert([Test classVariable] == 1);
  test_assert([TestSubClass classVariable] == 2);
  return 0;
}
