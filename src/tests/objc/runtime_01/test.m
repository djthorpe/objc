#include <objc/objc.h>
#include <tests/tests.h>

int test_runtime_01(void) {
  Object *obj = [[Object alloc] init];
  test_assert(obj != NULL);

  sys_printf("Step 1\n");

  const char *className = object_getClassName(obj);
  test_assert(className != NULL);
  test_cstrings_equal(className, "Object");

  sys_printf("Step 2\n");

  Class cls = object_getClass(obj);
  test_assert(cls != NULL);

  const char *clsName = class_getName(cls);
  test_assert(clsName != NULL);
  test_cstrings_equal(clsName, "Object");

  // Check class
  test_assert(cls == [obj class]);
  test_assert([obj isEqual:obj] == YES);

  // Dispose of the object
  [obj dealloc];

  sys_printf("All Tests Passed!\n");

  return 0;
}
