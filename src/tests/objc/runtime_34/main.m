#include <objc/objc.h>
#include <runtime-sys/sys.h>
#include <stdarg.h>
#include <tests/tests.h>

int test_runtime_34(void);

int main(void) { return TestMain("test_runtime_34", test_runtime_34); }

/* Test method with variable number of arguments */

@interface MathClass : Object

/* sum positive numbers; -1 ends the list */
+ (int)sum:(int)firstNumber, ...;

@end

@implementation MathClass

+ (int)sum:(int)firstNumber, ... {
  va_list ap;
  va_start(ap, firstNumber);

  int sum = 0;
  int number = firstNumber;
  while (number >= 0) {
    sum += number;
    number = va_arg(ap, int);
  }

  va_end(ap);
  return sum;
}

@end

int test_runtime_34(void) {
  test_assert(([MathClass sum:1, 2, 3, 4, 5, -1] == 15));
  test_assert(([MathClass sum:10, 20, 30, -1] == 60));
  test_assert(([MathClass sum:0, -1] == 0));
  test_assert(([MathClass sum:-1] == 0));
  return 0;
}
