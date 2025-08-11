#include "TestClass.h"
#include <objc/objc.h>
#include <tests/tests.h>

int test_runtime_02() {
  Test *test = [[Test alloc] init];
  test_assert(test != NULL);

  const char *className = object_getClassName(test);
  test_assert(className != NULL);
  test_assert(sys_strcmp(className, "Test") == 0);

  Class cls = object_getClass(test);
  test_assert(cls != NULL);

  const char *clsName = class_getName(cls);
  test_assert(clsName != NULL);
  test_assert(sys_strcmp(clsName, "Test") == 0);

  // Check class
  test_assert(cls == [test class]);
  test_assert([test isEqual:test] == YES);

  // Dispose of the object
  [test dealloc];

  return 0;
}
