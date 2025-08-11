
#include "runtime-sys.h"
#include <tests/tests.h>

int main(void) {
  sys_init();
  test_sys_00();
  test_sys_01();
  test_sys_02();
  sys_exit();
  return 0;
}
