#include <objc/objc.h>
#include <tests/tests.h>

/* Test that +initialize is automatically called before the class is accessed */

static int class_variable = 0;

OBJC_ROOT_CLASS
@interface TestClass {
  Class isa;
}

+(void) initialize;
+(int) classVariable;

@end

@implementation TestClass

+(void) initialize {
  class_variable = 1;
}

+(int) classVariable {
  return class_variable;
}

@end

int main (void) {
  test_assert([TestClass classVariable] == 1);
  return 0;
}
