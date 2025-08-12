#include <objc/objc.h>
#include <runtime-sys/sys.h>
#include <stdarg.h>
#include <tests/tests.h>

int test_runtime_37(void);

int main(void) { return TestMain("test_runtime_37", test_runtime_37); }

int test_runtime_37(void) {
  SEL selector = @selector(alloc);
  const char *selname = sel_getName(selector);
  test_cstrings_equal(selname, "alloc");
  return 0;
}
