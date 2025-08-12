
#include "runtime-objc.h"
#include <tests/tests.h>

int main(void) {
  sys_init();

  test_runtime_01();
  test_runtime_02();
  test_runtime_03();
  test_runtime_04();
  test_runtime_05();

  sys_exit();
  return 0;
}
