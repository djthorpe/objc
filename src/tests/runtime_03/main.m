#include <objc/objc.h>
#include <runtime-sys/sys.h>
#include <tests/tests.h>

#ifdef SYSTEM_NAME_PICO
// HACK
void *stdout = NULL;
void *stderr = NULL;
#endif

int test_runtime_03(void);

int main(void) { return TestMain("test_runtime_03", test_runtime_03); }

int test_runtime_03() {
  NXConstantString *str1 = @"tests";
  test_assert([str1 length] == 5);
  test_assert(strcmp([str1 cStr], "tests") == 0);

  NXConstantString *str2 = @"tests";
  test_assert([str2 length] == 5);
  test_assert(strcmp([str2 cStr], "tests") == 0);

  NXConstantString *str3 = @"tests3";
  test_assert([str3 length] == 6);
  test_assert(strcmp([str3 cStr], "tests3") == 0);

  // isEqual
  test_assert([str1 isEqual:str2]);
  test_assert(![str1 isEqual:str3]);
  test_assert(![str2 isEqual:str3]);

  // Return success
  return 0;
}
