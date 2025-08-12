#include <string.h>
#include <objc/objc.h>
#include <tests/tests.h>

/* Test the hidden argument _cmd to method calls */

OBJC_ROOT_CLASS
@interface TestClass {
  Class isa;
}
+(const char* ) method;
@end

@implementation TestClass

+(const char* ) method {
  return sel_getName(_cmd);
}

@end

int main (void) {
  test_assert([TestClass method] != NULL);
  test_assert(strcmp ([TestClass method], "method") == 0);
  return 0;
}
